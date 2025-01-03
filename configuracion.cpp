/**
 * @file configuracion.cpp
 * @author JAGG
 * @brief Programa para configuración del BListener
 * @version 0.1
 * @date 2024-12-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "clases/json.hpp"
#include "clases/config/config_socket.h"
#include "clases/tools/tool_security.h"
#include "clases/tools/tool_log.h"
#include "clases/tools/tool_general.h"
#include "clases/config/config_cons.h"
#include "clases/config/config_postgres.h"
#include "clases/config/config_sala.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

/**
 * @brief Función que configura el socket general con el sistema de caja
 * 
 * @return int 
 */
int setSocketListener(){
    int ireturn = 0;
    int itiempo = 0;
    int iintentos = 0;
    string cIP = "";
    string cPuerto = "";
    int bIP = 0;
    try
    {
        //Ingreso la IP del servidor y valido su estructura
        while(bIP < 2)
        {
            cout << "" << endl;
            cout << "TODOS LOS CAMPOS SON OBLIGATORIOS" << endl;
            cout << "Ingresa IP del servidor: ";
            cin >> cIP;
            int breturn = Herramientas::Generales::validaIP(cIP);
            if(breturn == 1)
            {
                cout << "IP valida! " << endl;;
                bIP = 3;
            }
            else{
                bIP = 0;
                cout << "Valor incorrecto!" << endl;
            }
        }
        //Pido el puerto genérico
        cout << "Ingresa el puerto genérico del servidor: ";
        cin >> cPuerto;
        cout << "" << endl;       
        Configuracion::bcSocket oSocket;
        int isave = Configuracion::SocketsInfo::SetParamsLocal(cIP,cPuerto);
        if(isave == 1)
        {
            cout << "" << endl;
            cout << "Los datos se guerdaron correctamente. "<< endl;         
        }
        else{
            cout << "" << endl;
            cout << "Hubo un error en el sistema, checa el log. "<< endl;   
        }
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("configuracion.cpp","setSocketListener", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
        ireturn -1;
    }
    return ireturn;
}

int setSocketCentral(){
    int ireturn = 0;
    int itiempo = 0;
    int iintentos = 0;
    string cIP = "";
    string cPuerto = "";
    int bIP = 0;
    try
    {
        //Ingreso la IP del servidor y valido su estructura
        while(bIP < 2)
        {
            cout << "" << endl;
            cout << "TODOS LOS CAMPOS SON OBLIGATORIOS" << endl;
            cout << "Ingresa IP del servidor central: ";
            cin >> cIP;
            int breturn = Herramientas::Generales::validaIP(cIP);
            if(breturn == 1)
            {
                cout << "IP valida! " << endl;;
                bIP = 3;
            }
            else{
                bIP = 0;
                cout << "Valor incorrecto!" << endl;
            }
        }
        //Pido el puerto genérico
        cout << "Ingresa el puerto del servidor central: ";
        cin >> cPuerto;
        cout << "" << endl;
        cout << "En caso de falló al intento de conexión con el servidor " << endl;
        cout << "se reitentará en la cantidad de segundos definidos. " << endl;
        cout << "Tiempo: ";
        cin >> itiempo; 
        //cout << "" << endl;
        cout << "Reintentos: ";
        cin >> iintentos; 
        cout << "" << endl;        
        Configuracion::bcSocket oSocket;
        int isave = Configuracion::SocketsInfo::SetParamsCentral(cIP,cPuerto,itiempo,iintentos);
        if(isave == 1)
        {
            cout << "" << endl;
            cout << "Los datos se guerdaron correctamente. "<< endl;         
        }
        else{
            cout << "" << endl;
            cout << "Hubo un error en el sistema, checa el log. "<< endl;   
        }
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("configuracion.cpp","setSocketCentral", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
        ireturn -1;
    }
    return ireturn;
}

int setSala()
{
    int ireturn = 0;
    string vsala = "";
    try
    {
        cout << "" << endl;
        cout << "INGRESA EL ID DE LA SALA DONDE ESTÁS INSTALANDO EL LISTENER" << endl;
        cout << "ID Sala: ";
        cin >> vsala;
        int isave = Configuracion::SalaInfo::SetParamSala(vsala);
        if(isave == 1)
        {
            cout << "" << endl;
            cout << "Los datos se guerdaron correctamente. "<< endl;         
        }
        else{
            cout << "" << endl;
            cout << "Hubo un error en el sistema, checa el log. "<< endl;   
        }              
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("configuracion.cpp","setSala", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
        ireturn -1;
    }
    return ireturn;    
}

int setDBLocal()
{
    int ireturn = 0;
    string vserv, vdb,vuser,vpass,vport;
    try
    {
        cout << "" << endl;
        cout << "TODOS LOS CAMPOS SON OBLIGATORIOS" << endl;
        cout << "Ingresa el servidor: ";
        cin >> vserv;
        cout << "Ingresa el puerto: ";
        cin >> vport;
        cout << "Ingresa el base de datos: ";
        cin >> vdb;
        cout << "Ingresa usuario: ";
        cin >> vuser;
        cout << "Ingresa password: ";
        cin >> vpass;
        int isave = Configuracion::Postgresql::SetParams(vserv,vport,vdb,vuser,vpass);
        if(isave == 1)
        {
            cout << "" << endl;
            cout << "Los datos se guerdaron correctamente. "<< endl;         
        }
        else{
            cout << "" << endl;
            cout << "Hubo un error en el sistema, checa el log. "<< endl;   
        }              
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("configuracion.cpp","setDBLocal", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
        ireturn -1;
    }
    return ireturn;    
}


int main()
{
    try
    {
        string entrada;
        int iOpcion;
        iOpcion = 0;
        int bSalir = 1;
        //Ciclo del menu hasta salir
        while(bSalir != 0)
        {
            cout << "BConnect v 0.1" << endl;
            cout << "Menú de configuración básica." << endl;
            cout << "" << endl;
            cout << "*****************************" << endl;
            cout << "1) Sala." << endl;
            cout << "2) Socket Listener." << endl;
            cout << "3) Base de datos local." << endl;
            cout << "4) Socket Servidor Central." << endl;
            cout << "5) Salir." << endl;
            cout << "*****************************" << endl;
            cout << "" << endl;
            cout << "Selecciona una opción: ";
            cin >> entrada;
            cout << "" << endl;
            cout << "Seleccionaste: " << entrada << endl;

            if(entrada != "1" && entrada != "2" && entrada != "3" && entrada != "4" && entrada != "5")
            {   
                cout << " " << entrada << " no es una opción valida!! " << endl;
                cout << "" << endl;
            }
            else
            {
                iOpcion = stoi(entrada);
                switch (iOpcion)
                {
                    case 1:
                        setSala();
                        break;
                    case 2:
                        setSocketListener();
                        break;
                    case 3:
                        setDBLocal();
                        break;
                    case 4:
                        setSocketCentral();
                        break;                        
                    case 5:
                        bSalir = 0;
                        exit(0);
                    break;
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        Seguridad::cLog::ErrorSistema("configuracion.cpp","main", e.what());
        cout << "Hubo un error en el sistema, checa el log. "<< endl;
    }

    return 0;

}