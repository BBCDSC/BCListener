#define PORT 20000

//Clases generales
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <tuple>
#include <atomic>
#include <mutex>
#include <map>

//Clases locales
#include "clases/config/config_socket.h"
#include "clases/tools/tool_security.h"
#include "clases/tools/tool_log.h"
#include "clases/tools/tool_general.h"
#include "clases/config/config_cons.h"
#include "clases/config/config_postgres.h"
#include "clases/config/config_sala.h"
#include "clases/control/log_registros.h"
#include "clases/control/mv_bconnects.h"
#include "clases/control/mv_tarjetas.h"
#include "clases/objetos/bconnect.h"
#include "clases/objetos/btarjeta.h"

using namespace std;

std::atomic<bool> clienteActivo(false);
std::atomic<int> totalThreadsActivos(0);
vector<Objetos::bConnect> Bconects;
mutex clientInfoMutex;
mutex threadsMutex;
string idsala;

// Estructura para mantener información del hilo
struct ThreadInfo {
    std::thread::id id;
    string mac;
    bool activo;
    std::thread handle;
};

std::map<int, ThreadInfo> mapaThreads;

// Función para obtener MAC del mensaje inicial
string obtenerMacDelMensaje(const string& mensaje) {
    vector<string> partes = Herramientas::Generales::split(mensaje, "|");
    if (partes.size() >= 2 && partes[0] == "CQ") {
        return partes[1];
    }
    return "";
}

// Función para cerrar un hilo específico
void cerrarHilo(int socketId) {
    std::lock_guard<std::mutex> lock(threadsMutex);
    if (mapaThreads.find(socketId) != mapaThreads.end()) {
        auto& threadInfo = mapaThreads[socketId];
        threadInfo.activo = false;
        
        // Cerrar socket
        close(socketId);
        
        // Remover de la lista de BConnects
        {
            std::lock_guard<std::mutex> bconnectLock(clientInfoMutex);
            auto it = find_if(Bconects.begin(), Bconects.end(),
                [socketId](const Objetos::bConnect& bconect) { 
                    return bconect.iSocket == socketId; 
                });
            
            if (it != Bconects.end()) {
                Objetos::bConnect clienteDesconectado =  Transacciones::MvBConnects::CambioStatus(*it, 13);
                Bconects.erase(it);
            }
        }
        
        // Actualizar contador de hilos
        totalThreadsActivos--;
        
        cout << "Cliente con MAC " << threadInfo.mac << " desconectado. Hilos activos: " << totalThreadsActivos << endl;
        
        // Log de la desconexión
        Logs::log::setAccionPG("Cliente desconectado", "MAC: " + threadInfo.mac + " - Hilos activos: " + to_string(totalThreadsActivos), 1);
        
        // Eliminar del mapa
        mapaThreads.erase(socketId);
    }
}

void evalua (vector<string> datagrama, int& new_sd)
{
    try{
        Objetos::bConnect bconect;
        auto shilo = this_thread::get_id();
        stringstream ss;
        ss << shilo;
        string idhilo = ss.str();
        Seguridad::Aes3 objEnc;

        if(datagrama[0] == "CQ"){
            bconect = Transacciones::MvBConnects::ValidacionLocal(datagrama[1]);
            if(bconect.IdBConnect == 0){
                pair <int, int> respuesta = Transacciones::MvBConnects::ValidacionCentral({datagrama[1],idsala});

                //la mac existe pero esta en otra sala pasa a estatus bloqueado.
                if(respuesta.first == -1){
                     bconect = Transacciones::MvBConnects::CambioStatus(bconect, 11);
                }
                //la mac no existe y se inserta en central, pasa a estatus pendiente 
                else if(respuesta.first == 1){
                    bconect = Transacciones::MvBConnects::Nuevo(respuesta.second, datagrama[1], 12);
                }
                //la mac si existe en central y es la sala correcta, pasa a estatus Conectado.
                else if(respuesta.first == 2) {
                    bconect = Transacciones::MvBConnects::Nuevo(respuesta.second, datagrama[1], 10);
                }
            }

            // Si el estaus estaba en inactivo, se cambia a conectado
            if(bconect.IdStatus == 13  ){
                cout << "ENtra a hacer el cambio con el idConect: " + bconect.IdBConnect;
                bconect = Transacciones::MvBConnects::CambioStatus(bconect, 10);
            }
            //si el estatus está bloqueado, Pendiente o Baja, se rechaza la conexión
            else if(bconect.IdStatus == 11 || bconect.IdStatus == 12 || bconect.IdStatus == 14){

                cout << "Envia: CR|"+to_string(bconect.IdBConnect)+"|"+to_string(bconect.IdStatus)+"|"+bconect.status<< endl;
                string resDatagrama = objEnc.Encriptar("CR|"+to_string(bconect.IdBConnect)+"|"+to_string(bconect.IdStatus)+"|"+bconect.status);
                const char* sdata = resDatagrama.c_str();
                send(new_sd,sdata, strlen(sdata),0);
                cerrarHilo(new_sd);
                Logs::log::setErrorPG("Bconect con estatus: "+bconect.status+" Se rechasa la conexión", 1);
                return;
            }
            
            bconect.iSocket = new_sd;

            cout << "Envia: CR|"+to_string(bconect.IdBConnect)+"|"+to_string(bconect.IdStatus)+"|"+bconect.status<< endl;
            auto duration = std::chrono::system_clock::now();
            string resDatagrama = objEnc.Encriptar("CR|"+to_string(bconect.IdBConnect)+"|"+to_string(bconect.IdStatus)+"|"+bconect.status);
            const char* sdata = resDatagrama.c_str();

            lock_guard<mutex> lock(clientInfoMutex);
            Bconects.push_back(bconect);
            send(new_sd,sdata, strlen(sdata),0);
            Logs::log::setAccionPG("Bconect se conecto con el lisener","Conexión establecida correctamente", 1);

        } else if (datagrama[0] == "TQ") {

            vector<Objetos::bTarjeta> saldos;
            string resDatagrama = "TR|";
            saldos = Transacciones::MvTarjetas::ConsultaTarjetaByUid(datagrama[2]);
            if(saldos.size() > 0){
                resDatagrama += to_string(saldos[0].IdTarjeta)+"|"+ saldos[0].Ctarjeta+"|";
                bool primerRegistro = true;
                for (const Objetos::bTarjeta& saldo : saldos ){
                    if(!primerRegistro){
                        resDatagrama += ",";
                    }
                    resDatagrama += to_string(saldo.IdTipoMoneda) + ":" + to_string(saldo.Msaldo);
                    primerRegistro = false;
                }
            }else{
                resDatagrama = resDatagrama + "0|0|0:0|No existen movimientos de la tarjeta o no existe la tarjeta! Validar con un cajero";
            }
            cout << "Se envia la respuesta: "+ resDatagrama;
            resDatagrama = objEnc.Encriptar(resDatagrama);
            const char* sdata = resDatagrama.c_str();
            send(new_sd,sdata, strlen(sdata),0);
            Logs::log::setAccionPG("Consulta el sado de la tarejta: "+ datagrama[2], "El saldo de las tarjetas es: ", 1);
    
        } else if (datagrama[0] == "SQ"){
            vector<string> saldos = Herramientas::Generales::split(datagrama[3],",");
            for(const string& saldo: saldos){
                size_t pos = saldo.find(':');
                if(pos != string::npos){
                    string resDatagrama = "SR|";
                    bool operacion = Transacciones::MvTarjetas::ActualizaSaldoByIdTarjetaAndIdTipoMoneda(datagrama[2], stoi(saldo.substr(0, pos)),stod(saldo.substr(pos + 1)));
                    if(operacion){
                        resDatagrama += "1";
                    }else{
                        resDatagrama += "0";
                        int iResult = Logs::log::setErrorPG("Error al actualizar el saldo de la tarjeta: "+ datagrama[2]+"con los datos: "+saldo,1 );
                    }
                    cout << "Actulizar saldo: "+ resDatagrama;
                    resDatagrama = objEnc.Encriptar(resDatagrama);
                    const char* sdata = resDatagrama.c_str();
                    send(new_sd,sdata, strlen(sdata),0);
                }else{
                    int iResult = Logs::log::setErrorPG("Error al actualizar el saldo de la tarjeta: "+ datagrama[2]+"con los datos: "+saldo,1 );
                }
            }

        } else {
            int iResult = Logs::log::setErrorPG("Error al intetar registrar MAC con el thread, no coincide el DataGrama",1 );
            string Datagrama = "No conicide con ningun DataGrama";
            auto duration = std::chrono::system_clock::now();               
                        
            Datagrama = objEnc.Encriptar(Datagrama);
            const char* sdata = Datagrama.c_str();
            send(new_sd,sdata, strlen(sdata),0);
            return;
        }

    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","server", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
    }
}

/**
 * @brief Funcion encargada de mandar el mensaje de caja a los BConect
 * 
 * @param mensaje 
 * @param tipo 
 * @param idConect 
 */
void sendMessage(const string& mensaje, const string& tipo, const int& idConect = 0) {
   Seguridad::Aes3 objEnc;
   string mensajeEncriptado = objEnc.Encriptar("SM|D|" + mensaje);
   
   lock_guard<mutex> lock(clientInfoMutex);
   
   if (tipo == "A") {
       // Envía a todos los clientes
       cout << "Mensaje para todos: "+Bconects.size() << endl;
       if(Bconects.size() > 0 ){ 
            for (const auto& bconect : Bconects) {
                send(bconect.iSocket, mensajeEncriptado.c_str(), mensajeEncriptado.length(), 0);
                cout << "Mensaje broadcast enviado al Bconect: " << bconect.IdBConnect << endl;
            }
       }else{
            cout << "No hay Bconects en linea :("<<endl;
       }
   } else if (tipo == "E") {
       // Busca y envía al cliente específico
       auto it = find_if(Bconects.begin(), Bconects.end(),
           [&idConect](const Objetos::bConnect& bconect) { return bconect.IdBConnect == idConect; });
           
       if (it != Bconects.end()) {
           send(it->iSocket, mensajeEncriptado.c_str(), mensajeEncriptado.length(), 0);
           cout << "Mensaje específico enviado al BConect: " << idConect << endl;
       } else {
           cout << "MAC no encontrada: " << idConect << endl;
       }
   }
}

/**
 * @brief Maneja un único cliente de la caja en el servidor secundario
 * 
 * @param clientSocket Socket del cliente
 */
void manejarClienteUnico(int clientSocket) {
    try {
        Seguridad::Aes3 objEnc;
        char buffer[1024] = {0};

        while (true) {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0) break;

            string mensajeEncriptado(buffer, bytesRead);
            string mensajeDesencriptado = objEnc.Desencriptar(mensajeEncriptado);
            
            vector<string> datagrama = Herramientas::Generales::split(mensajeDesencriptado, "|");
            cout << "Mensaje: "+ mensajeDesencriptado << endl;
            if (datagrama[0] == "CSM") {
                if (datagrama[1] == "A") {
                    cout << "Manda mensaje para todos los conects" << endl;
                    sendMessage(datagrama[2], "A");
                } else if (datagrama[1] == "E") {
                    vector<string> idConects = Herramientas::Generales::split(datagrama[2], ",");
                    for (const auto& id : idConects) {
                        sendMessage(datagrama[3], "E", stoi(id));
                        cout << "Ids: "+ id << endl;
                    }
                }
            }

            memset(buffer, 0, sizeof(buffer));
        }
    }
    catch(const std::exception& e) {
        Seguridad::cLog::ErrorSistema("blistener.cpp", "manejarClienteUnico", e.what());
    }

    close(clientSocket);
    clienteActivo = false;
    cout << "Cliente único desconectado" << endl;
}

/**
 * @brief Función que construye el servidor de escucha.
 * 
 * @param server_addr Parametros del servidor
 * @param socketDesc int
 */
void server(struct sockaddr_in* server_addr, int& socketDesc, int puerto)
{
    try{
        memset(server_addr, (char)0, sizeof(sockaddr_in));
        
        socketDesc = socket(AF_INET, SOCK_STREAM, 0);

        if (socketDesc < 0)
        {
            Seguridad::cLog::ErrorSistema("blistener.cpp","server", "Servidor listener no puede abrir el socket!");
            cout << "Hubo un error en el sistema, checa el log. "<< endl;             
            return;
        }
        server_addr->sin_family = AF_INET;
        server_addr->sin_addr.s_addr = INADDR_ANY;
        server_addr->sin_port = htons(PORT);
        int n;

        if ((n = bind(socketDesc, (struct sockaddr*)server_addr, sizeof(sockaddr_in))) < 0)
        {
            Seguridad::cLog::ErrorSistema("blistener.cpp","server", "Servidor listener no puede enlazar!");
            cout << "Hubo un error en el sistema, checa el log. "<< endl;              
            return;
        }

        if ((n = listen(socketDesc, SOMAXCONN) < 0))
        {
            Seguridad::cLog::ErrorSistema("blistener.cpp","server", "Servidor listener no puede escuchar!");
            cout << "Hubo un error en el sistema, checa el log. "<< endl;            
            return;
        }
        int iResult = Logs::log::setAccionPG("Se inicia el servidor BListener para escucha","correcto",1);
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","server", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
    }    
}

/**
 * @brief Inicializa y ejecuta el servidor de cliente único de caja
 * 
 * @param puerto Puerto para el servidor único
 */
void servidorCaja(int puerto) {
    try {
        int socketDesc;
        struct sockaddr_in server_addr;

        // Crear socket
        socketDesc = socket(AF_INET, SOCK_STREAM, 0);
        if (socketDesc < 0) {
            Seguridad::cLog::ErrorSistema("blistener.cpp", "servidorUnico", "Error al crear socket único");
            return;
        }

        // Configurar socket
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(puerto);

        // Permitir reutilización del puerto
        int opt = 1;
        setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Bind
        if (bind(socketDesc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            Seguridad::cLog::ErrorSistema("blistener.cpp", "servidorUnico", "Error en bind de servidor único");
            return;
        }

        // Listen
        if (listen(socketDesc, 1) < 0) {
            Seguridad::cLog::ErrorSistema("blistener.cpp", "servidorUnico", "Error en listen de servidor único");
            return;
        }

        cout << "Servidor de cliente Caja iniciado en puerto " << puerto << endl;

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        while (true) {
            int clientSocket = accept(socketDesc, (struct sockaddr*)&client_addr, &client_len);
            
            if (clientSocket < 0) {
                Seguridad::cLog::ErrorSistema("blistener.cpp", "servidorUnico", "Error al aceptar conexión");
                continue;
            }

            if (clienteActivo) {
                Seguridad::Aes3 objEnc;
                string mensaje = "Sesión activa. Intente más tarde.";
                string mensajeEncriptado = objEnc.Encriptar(mensaje);
                send(clientSocket, mensajeEncriptado.c_str(), mensajeEncriptado.length(), 0);
                close(clientSocket);
                cout << "Conexión rechazada: ya existe una sesión activa" << endl;
                continue;
            }

            clienteActivo = true;
            cout << "Nuevo cliente único conectado" << endl;

            thread clientThread(manejarClienteUnico, clientSocket);
            clientThread.detach();
        }
    }
    catch(const std::exception& e) {
        Seguridad::cLog::ErrorSistema("blistener.cpp", "servidorUnico", e.what());
    }
}

/**
 * @brief Función de escucha
 * 
 * @param client_addr 
 * @param new_sd 
 */
void func(struct sockaddr_in* client_addr, int& new_sd, const string& mensajeInicial)
{
    try
    {
        // Incrementar contador de hilos
        totalThreadsActivos++;
         // Registrar información del hilo
        {
            std::lock_guard<std::mutex> lock(threadsMutex);
            ThreadInfo threadInfo;
            threadInfo.id = std::this_thread::get_id();
            threadInfo.mac = obtenerMacDelMensaje(mensajeInicial);
            threadInfo.activo = true;
            mapaThreads[new_sd] = move(threadInfo);
        }
        cout << "Nuevo cliente conectado. Total hilos activos: " << totalThreadsActivos << endl;

        Seguridad::Aes3 objEnc;
        vector<string> datagrama = Herramientas::Generales::split(mensajeInicial, "|");
        
        evalua(datagrama, new_sd);
        
        // Continuamos con el ciclo normal de recepción
        char buffer[1024] = { 0 };
        int n = 0;

        while ((n = recv(new_sd, buffer, sizeof(buffer), 0)) > 0)
        {

            {
                std::lock_guard<std::mutex> lock(threadsMutex);
                if (!mapaThreads[new_sd].activo) {
                    break;
                }
            }

            string respuesta = objEnc.Desencriptar(string(buffer, n));
            cout << "Recibe: " + respuesta << endl;
            datagrama = Herramientas::Generales::split(respuesta,"|");

            evalua(datagrama, new_sd);
            
            memset(buffer, 0, sizeof(buffer));

        }
        cerrarHilo(new_sd);
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","func", e.what());
        cout << "Error en el hilo: " << e.what() << endl;
        cerrarHilo(new_sd);
    }    
}

// Función para cerrar un cliente específico por MAC
void cerrarClientePorMac(const string& mac) {
    std::lock_guard<std::mutex> lock(threadsMutex);
    for (const auto& pair : mapaThreads) {
        if (pair.second.mac == mac) {
            cerrarHilo(pair.first);
            break;
        }
    }
}

int main()
{
    try
    {
        Configuracion::bcSocket osocketserver;
        Configuracion::bcSocket osocketcentral;

        idsala = Configuracion::SalaInfo::GetSalaInfo();
        osocketserver = Configuracion::SocketsInfo::GetSocketLocalInfo();
        osocketcentral = Configuracion::SocketsInfo::GetSocketCentralInfo();

        //Iniciar servidor único en un thread separado
        thread servidorUnicoThread(servidorCaja, 20001);  // Puerto 20001 para el servidor único
        servidorUnicoThread.detach();

        //Creo el servidor TCP/IP
        int socketDesc;
        struct sockaddr_in server_addr;
        server(&server_addr, socketDesc, osocketserver.Puerto);
        cout << "Servidor de Bconect iniciado en puerto " << osocketserver.Puerto << endl;

        //Creo el thread pool 
        vector<std::thread> poolofthread;
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(sockaddr_in);
        int new_sd;

        //Ciclo para aceptar conexiones de BConnects
        while ((new_sd = accept(socketDesc, (sockaddr*)&client_addr, (socklen_t*)&length)) > 0)
        {
            cout << "Cliente intentando conectar..." << endl;

            char buffer[1024] = {0};        
            Seguridad::Aes3 objEnc;
            
            if (recv(new_sd, buffer, sizeof(buffer), 0) <= 0) {
                cout << "Error al recibir datos iniciales del cliente" << endl;
                close(new_sd);
                continue;
            }
            
            string mensajeEncriptado(buffer);
            string mensajeDesencriptado = objEnc.Desencriptar(mensajeEncriptado);
            vector<string> datagrama = Herramientas::Generales::split(mensajeDesencriptado, "|");
            
            if (datagrama.size() < 2 || datagrama[0] != "CQ") {
                string mensajeError = "ERROR|Formato de conexión inválido";
                string mensajeEncriptado = objEnc.Encriptar(mensajeError);
                send(new_sd, mensajeEncriptado.c_str(), mensajeEncriptado.length(), 0);
                close(new_sd);
                continue;
            }

            string mac = datagrama[1];
            cout << "Cliente autorizado con MAC: " << mac << endl;

            //Creamos un hilo por BConnect
            thread t(func, &client_addr, ref(new_sd), mensajeDesencriptado);
            //Lo pongo al final de la fila de hilos
            poolofthread.push_back(move(t));
            cout << "Threads open: " << poolofthread.size() << endl;
        }
        // Esperar a que todos los hilos terminen
        for(auto& t : poolofthread) {
            if(t.joinable()) {
                t.join();
            }
        }
        return 0;
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","main", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
        return -1;
    }
}
