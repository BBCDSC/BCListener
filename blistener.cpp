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

//Clases locales
#include "clases/config/config_socket.h"
#include "clases/tools/tool_security.h"
#include "clases/tools/tool_log.h"
#include "clases/tools/tool_general.h"
#include "clases/config/config_cons.h"
#include "clases/config/config_postgres.h"
#include "clases/config/config_sala.h"
#include "clases/control/mv_bconnects.h"
#include "clases/dbs/db_exec_postgres.h"
//#include "clases/control/log_registros.h"


using namespace std;

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
        //int iResult = Logs::log::setAccionPG("Se inicia el servidor BListener para escucha","correcto",1);
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","server", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
    }    
}

string saveMac(const string& query)
{
    try{
        BaseDatos::DBConnectPostgres oPostgres;
        PGresult* result; 
        string errorMessage;

        result = oPostgres.ExecFunction(query);

        // Verificamos si obtuvimos un resultado
        if (!result) {
             cout << "La consulta no retornó resultados" << endl;
        }
        return string(PQgetvalue(result, 0, 0));
    }
    catch(const std::exception& e) {
        Seguridad::cLog::ErrorSistema("blistener.cpp","saveMac", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
        return "";
    }
}

string updateSaldo(const string& query){
        BaseDatos::DBConnectPostgres oPostgres;
        PGresult* result; 
        string errorMessage;

        result = oPostgres.ExecFunction(query);

        // Verificamos si obtuvimos un resultado
        if (result!=NULL) {
             return "1";
        } else {
            cout << "La consulta no retornó resultados" << endl;
             return "0";
        }
        
    }


tuple<string, string, string>selectBalance(const string& query)
{
    try{
        BaseDatos::DBConnectPostgres oPostgres;
        PGresult* result; 
        int filas;
        int columnas;

        result = oPostgres.ExecFunction(query);

        cout << "id: "+ string(PQgetvalue(result,0,0)) << endl;
        cout << "Tarjeta: "+ string(PQgetvalue(result,0,1)) << endl;
        cout << "tipomoneda: "+ string(PQgetvalue(result,0,3)) << endl;
        cout << "saldo: "+ string(PQgetvalue(result,0,4)) << endl;
        // Verificamos si obtuvimos un resultado
        if (!result) {
             cout << "La consulta no retornó resultados" << endl;
        }
        return make_tuple(string(PQgetvalue(result,0,0)), string(PQgetvalue(result,0,1)), string(PQgetvalue(result,0,4)));
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","selectBalance", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
        return make_tuple("", "", "");
    }
}

void registerLogAction(const string& query)
{
     try{
        BaseDatos::DBConnectPostgres oPostgres;
        PGresult* result; 

        oPostgres.ExecFunction(query);
        // Verificamos si obtuvimos un resultado
        if (!result) {
             cout << "La consulta no retornó resultados" << endl;
        }
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","registerLogAction", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
    }
}

void evalua(vector<string> datagrama, int& new_sd){
    auto shilo = this_thread::get_id();
    stringstream ss;
    ss << shilo;
    string idhilo = ss.str();
    Seguridad::Aes3 objEnc;
    
    if(datagrama[0] == "CQ") {
        string queryFunction = "SELECT * FROM bcn.\"fnBigConnects\"('B', NULL, '"+ datagrama[1]+"',  NULL)";
        string idBC = saveMac(queryFunction);
        cout << "Se registra la MAC: " + datagrama[1] + " con el thread: " + idhilo << endl;
        string Datagrama = "CR|"+idBC;
        cout << "Envia: "+Datagrama<< endl;
        auto duration = std::chrono::system_clock::now();
        
        Datagrama = objEnc.Encriptar(Datagrama);
        const char* sdata = Datagrama.c_str();
        string queryFunction = "SELECT * FROM sp.\"logsacciones\"('I', 100, 'Bconect se conecto con el lisener', 'Resultado Correcto', 4)";
        registerLogAction(queryFunction);
        send(new_sd,sdata, strlen(sdata),0); //preguntar que hace referencia el sero en el send
        //iResult = Logs::log::setAccionPG("Se registra la MAC: " + datagrama[1] + " con el thread: " + idhilo ,"correcto",1);
    } else if(datagrama[0] == "TQ") {
        //Se manda llamar la funcion sp.mvtarjetassaldos
        string queryFunc = "SELECT * FROM sp.\"mvtarjetassaldos\"('CS','"+datagrama[2]+"', NULL, NULL, NULL)";
        auto [idTarjeta, uIdTarjeta, saldo] = selectBalance(queryFunc);

        //string DatagramaEnvio = "TR|"+idTarjeta+"|"+uIdTarjeta+"|"+saldo;
        string DatagramaEnvio = "TR|"+datagrama[1]+"|"+uIdTarjeta+"|"+saldo;
        cout << "Envia: "+DatagramaEnvio<< endl;
        auto duration = std::chrono::system_clock::now();   
                    
        DatagramaEnvio = objEnc.Encriptar(DatagramaEnvio);
        string queryFunction = "SELECT * FROM sp.\"logsacciones\"('I', 1, 'Se consulta el sado una tarjeta de juego', 'Saldo en tarjeta: '"+saldo+", 4)";
        registerLogAction(queryFunction);

        const char* sdata = DatagramaEnvio.c_str();
        send(new_sd,sdata, strlen(sdata),0);
    } else if(datagrama[0] == "SQ") {
        //Actualizamos el saldo que nos regresa el conect
        string queryFunc = "SELECT sp.\"mvtarjetassaldos\"('A'::character varying,'"+datagrama[2]+
        "'::character varying, NULL::integer,"+ datagrama[3] +"::smallint, "+ datagrama[4] +"::numeric)";
        
        string response = updateSaldo(queryFunc);

        string DatagramaSaldo = "SR|" + response;
        cout << "Envia: "+DatagramaSaldo<< endl;
        auto duration = std::chrono::system_clock::now();   
                    
        DatagramaSaldo = objEnc.Encriptar(DatagramaSaldo);
        const char* sdata = DatagramaSaldo.c_str();
        send(new_sd,sdata, strlen(sdata),0);
    } else {
        //iResult = Logs::log::setErrorPG("Error al intetar registrar MAC con el thread",1 );
        string Datagrama = "No conicide con ningun DataGrama";
        auto duration = std::chrono::system_clock::now();               
                    
        Datagrama = objEnc.Encriptar(Datagrama);
        const char* sdata = Datagrama.c_str();
        send(new_sd,sdata, strlen(sdata),0);
        return;
    }
}

/**
 * @brief Función de escucha
 * 
 * @param client_addr 
 * @param new_sd 
 */
void func(struct sockaddr_in* client_addr, int& new_sd)
{
    try
    {
        char buffer[1024] = { 0 }; //buffer lo que manda el conect
        int n = 0;
        int iResult = 0;
        Seguridad::Aes3 objEnc;
        vector<string> datagrama; 
        //Recibo el primer datagrama
        recv(new_sd, buffer, sizeof(buffer),0);
        //Desencripto la cadena 
        string respuesta = objEnc.Desencriptar(buffer);
        cout<<"Recibe: "+ respuesta << endl;
        //Parseo la cadena
        datagrama = Herramientas::Generales::split(respuesta,"|");

        evalua(datagrama, new_sd);

        while ((n = recv(new_sd, buffer, sizeof(buffer), 0)))
        {
            cout << "Current Thread ID " << this_thread::get_id() << endl;

            string respuesta = objEnc.Desencriptar(string(buffer, n));
            cout<<"Recibe: "+ respuesta << endl;
            //Parseo la cadena
            datagrama = Herramientas::Generales::split(respuesta,"|");
            cout << respuesta << endl;

            evalua(datagrama, new_sd);
        }

        
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","func", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
    }
           
}


/*
void EscuchaPrincipal(int ipuertoescu)
{
    try
    {
        int iresult = 0;
        //Obtengo los parametros de configuración
        string idsala = "";
        Configuracion::bcSocket osocketserver;
        Configuracion::bcSocket osocketcentral;

        idsala = Configuracion::SalaInfo::GetSalaInfo();
        osocketserver = Configuracion::SocketsInfo::GetSocketLocalInfo();
        osocketcentral = Configuracion::SocketsInfo::GetSocketCentralInfo();

        int ipuertogeneral = osocketserver.Puerto;

        //"terminal.integrated.detectLocale": "off",
        //"terminal.integrated.localEchoEnabled": "off",
        // creating socket
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        // specifying the address
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(ipuertoescu); //htons(osocketserver.Puerto);
        serverAddress.sin_addr.s_addr = INADDR_ANY;//inet_addr(sIP.c_str());

        // binding socket.
        bind(serverSocket, (struct sockaddr*)&serverAddress,sizeof(serverAddress));

        int bSalir = 1;

        while (bSalir != 0)
        {
            // listening to the assigned socket escucho hasta 1000 conexiones
            iresult = listen(serverSocket, 1000);
            // accepting connection request
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            Seguridad::Aes3 objEnc;
            // recieving data
            char buffer[1024] = { 0 };
            recv(clientSocket, buffer, sizeof(buffer), 0);
            auto now = std::chrono::system_clock::now();
            cout << ipuertoescu << " | Message from client: " << buffer << endl;
            string respuesta = objEnc.Desencriptar(buffer);
            //auto duration = std::chrono::system_clock::now();
            //auto milliseconds = chrono::duration_cast<chrono::microseconds>(duration-now).count();                
            //cout << "Message from client: " << respuesta << "|" << milliseconds <<endl;
            cout << "Message from client: " << respuesta << "|" << respuesta.length() << endl;
            //int iResult = Logs::log::setAccionPG("Message from client: " + respuesta,"correcto",1);
            string Datagrama = "PP|d8:3a:dd:f7:bb:2b|172.16.1.101|1|20000";
            auto duration = std::chrono::system_clock::now();
            auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration-now).count();                
            cout << "Tiempo proceso: " << respuesta << "|" << milliseconds <<endl;            
            Datagrama = objEnc.Encriptar(Datagrama);
            const char* sdata = Datagrama.c_str();
            send(clientSocket,sdata, strlen(sdata),0);
        }

        // closing the socket.
        close(serverSocket);

    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","EscuchaPrincipal", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
    }
    
}
*/

int main()
{
    try
    {
        int iresult = 0;
        //Obtengo los parametros de configuración
        string idsala = "";
        Configuracion::bcSocket osocketserver;
        Configuracion::bcSocket osocketcentral;

        idsala = Configuracion::SalaInfo::GetSalaInfo();
        osocketserver = Configuracion::SocketsInfo::GetSocketLocalInfo();
        osocketcentral = Configuracion::SocketsInfo::GetSocketCentralInfo();
        //Creo el servidor TCP/IP
        int socketDesc;
        struct sockaddr_in server_addr;
        server(&server_addr, socketDesc, osocketserver.Puerto);
        //Creo el thread pool 
        vector<std::thread> poolofthread;
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(sockaddr_in);
        int new_sd;

        //Ciclo para aceptar conexiones de BConnects
        while ((new_sd = accept(socketDesc, (sockaddr*)&client_addr, (socklen_t*)&length)) > 0)
        {
            cout << "Client Accepted" << endl;
            //Creamos un hilo por BConnect
            thread t(func, &client_addr, ref(new_sd));
            //Lo pongo al final de la fila de hilos
            poolofthread.push_back(move(t));
            cout << "Threads open: " << poolofthread.size() << endl;
            
        }
        return 0;

/*
        std::list<thread> lsHilos;
        lsHilos.front();

        thread tprincipal(EscuchaPrincipal,19999);
        
        tprincipal.join();
        thread tprincipal2(EscuchaPrincipal,20000);
        tprincipal2.join();



        int iresult = 0;
        //Obtengo los parametros de configuración
        string idsala = "";
        Configuracion::bcSocket osocketserver;
        Configuracion::bcSocket osocketcentral;

        idsala = Configuracion::SalaInfo::GetSalaInfo();
        osocketserver = Configuracion::SocketsInfo::GetSocketLocalInfo();
        osocketcentral = Configuracion::SocketsInfo::GetSocketCentralInfo();

        int ipuertogeneral = osocketserver.Puerto;

        //"terminal.integrated.detectLocale": "off",
        //"terminal.integrated.localEchoEnabled": "off",
        // creating socket
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        // specifying the address
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(osocketserver.Puerto);
        serverAddress.sin_addr.s_addr = INADDR_ANY;//inet_addr(sIP.c_str());

        // binding socket.
        bind(serverSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));

        int bSalir = 1;

        while (bSalir != 0)
        {
            // listening to the assigned socket escucho hasta 1000 conexiones
            iresult = listen(serverSocket, 1000);
            // accepting connection request
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            Seguridad::Aes3 objEnc;
            // recieving data
            char buffer[1024] = { 0 };
            recv(clientSocket, buffer, sizeof(buffer), 0);
            auto now = std::chrono::system_clock::now();
            cout << "Message from client: " << buffer << endl;
            string respuesta = objEnc.Desencriptar(buffer);
            //auto duration = std::chrono::system_clock::now();
            //auto milliseconds = chrono::duration_cast<chrono::microseconds>(duration-now).count();                
            //cout << "Message from client: " << respuesta << "|" << milliseconds <<endl;
            cout << "Message from client: " << respuesta << "|" << respuesta.length() << endl;
            int iResult = Logs::log::setAccionPG("Message from client: " + respuesta,"correcto",1);
            string Datagrama = "PP|d8:3a:dd:f7:bb:2b|172.16.1.101|1|20000";
            auto duration = std::chrono::system_clock::now();
            auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration-now).count();                
            cout << "Tiempo proceso: " << respuesta << "|" << milliseconds <<endl;            
            Datagrama = objEnc.Encriptar(Datagrama);
            const char* sdata = Datagrama.c_str();
            send(clientSocket,sdata, strlen(sdata),0);
        }

        // closing the socket.
        close(serverSocket);*/
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("blistener.cpp","main", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
        return -1;
    }    

    return 0;
}
