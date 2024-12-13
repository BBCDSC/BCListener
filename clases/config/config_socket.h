#ifndef dsocket_h
#define dsocket_h

/**
 * @file config_socket.h
 * @author JAGG
 * @brief Parametros confifuración del socket de comunicación
 * @version 0.1
 * @date 2024-11-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <clases/json.hpp>
#include "clases/config/config_cons.h"
#include "clases/objetos/bcSocket.h"
#include <string>
#include <fstream>
#include <iostream>
#include "clases/tools/tool_log.h"
#include "clases/tools/tool_security.h"



using namespace std;

namespace Configuracion
{
    /**
     * @brief Parametros confifuración del socket de comunicación
     * 
     */
    class SocketsInfo
    {
        public:
            /**
             * @brief Obtiene un objeto socket para establecer la comunicación local 
             * 
             * @return Socket 
             */
            static Configuracion::bcSocket GetSocketLocalInfo(){
                //creo el objeto json
                nlohmann::json jsonData;            
                Configuracion::bcSocket osocket;
                Seguridad::Aes3 objDenc; 
                try{
                    //Abro el archivo  para lectura
                    ifstream archivo(Constantes::getJsonPath());
                    //transfiero el stream aun objeto json
                    archivo >> jsonData;
                    //leo los parametros del objeto json
                    osocket.Server = objDenc.Desencriptar(jsonData["socketlocal"]["servidor"]);
                    string vpuerto = objDenc.Desencriptar(jsonData["socketlocal"]["puerto"]);
                    osocket.Puerto = stoi(vpuerto);
                    //cierro el archivo
                    archivo.close();       
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_socket.h","GetSocketLocalInfo", e.what());
                    osocket.Server = "";
                    osocket.Puerto = 0;
                    osocket.Tiempo = 1;
                    osocket.Intentos = 1;
                }
                return osocket;
            }  

            /**
             * @brief Obtiene un objeto socket para establecer la comunicación local 
             * 
             * @return Socket
             */
            static Configuracion::bcSocket GetSocketCentralInfo(){
                //creo el objeto json
                nlohmann::json jsonData;            
                Configuracion::bcSocket osocket;
                Seguridad::Aes3 objDenc; 
                try{
                    //Abro el archivo  para lectura
                    ifstream archivo(Constantes::getJsonPath());
                    //transfiero el stream aun objeto json
                    archivo >> jsonData;
                    //leo los parametros del objeto json
                    osocket.Server = objDenc.Desencriptar(jsonData["socketcentral"]["servidor"]);
                    string vpuerto = objDenc.Desencriptar(jsonData["socketcentral"]["puerto"]);
                    string vtiempo = objDenc.Desencriptar(jsonData["socketcentral"]["tiempo"]);
                    string vintentos = objDenc.Desencriptar(jsonData["socketcentral"]["intentos"]);
                    osocket.Tiempo = stoi(vtiempo);
                    osocket.Intentos = stoi(vintentos);
                    osocket.Puerto = stoi(vpuerto);
                    //cierro el archivo
                    archivo.close();       
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_socket.h","GetSocketCentralInfo", e.what());
                    osocket.Server = "";
                    osocket.Puerto = 0;
                    osocket.Tiempo = 1;
                    osocket.Intentos = 1;
                }
                return osocket;
            }  

            /**
             * @brief Configura el puerto de escucha.
             * 
             * @param _Server IP del servidor
             * @param _Puerto Puerto de escucha , por default 19999
             * @return int 
             */
            static int SetParamsLocal(string _Server, string _Puerto){
                //creo el objeto json
                nlohmann::json jsonData;            
                int breturn = 0;
                Seguridad::Aes3 objEnc; 
                try{
                    //Abro el archivo de config para lectura
                    ifstream archivo(Constantes::getJsonPath());
                    //Transfiero el stream aun objeto json
                    archivo >> jsonData;
                    //Cierro el archivo
                    archivo.close();
                    //Modifico parámetros
                    jsonData["socketlocal"]["servidor"] = objEnc.Encriptar(_Server);
                    jsonData["socketlocal"]["puerto"] = objEnc.Encriptar(_Puerto);
                    //Abro archivo para escritura
                    ofstream archivoactualizado(Constantes::getJsonPath());
                    //Mando el jason al stream
                    archivoactualizado << jsonData.dump(4); //El 4 es para la identación
                    //cierro el archivo
                    archivoactualizado.close();
                    breturn = 1;         
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_socket.h","SetParamsLocal", e.what());
                    breturn = -1;
                }
                return breturn;
            }    

            /**
             * @brief  Configura el puerto de escucha con el servidor cenral
             * 
             * @param _Server IP del servidor central
             * @param _Puerto Puerto por default 19998
             * @param _Tiempo tiempo de espera entre cada intento.
             * @param _Intentos Cantidad de intentos de conexión ante falla.
             * @return int 1: correcto, -1 error
             */
            static int SetParamsCentral(string _Server, string _Puerto, int _Tiempo, int _Intentos){
                //creo el objeto json
                nlohmann::json jsonData;            
                int breturn = 0;
                Seguridad::Aes3 objEnc; 
                try{
                    //Abro el archivo de config para lectura
                    ifstream archivo(Constantes::getJsonPath());
                    //Transfiero el stream aun objeto json
                    archivo >> jsonData;
                    //Cierro el archivo
                    archivo.close();
                    //Modifico parámetros
                    jsonData["socketcentral"]["servidor"] = objEnc.Encriptar(_Server);
                    jsonData["socketcentral"]["puerto"] = objEnc.Encriptar(_Puerto);
                    jsonData["socketcentral"]["tiempo"]= objEnc.Encriptar(to_string(_Tiempo));
                    jsonData["socketcentral"]["intentos"]= objEnc.Encriptar(to_string(_Intentos));
                    //Abro archivo para escritura
                    ofstream archivoactualizado(Constantes::getJsonPath());
                    //Mando el jason al stream
                    archivoactualizado << jsonData.dump(4); //El 4 es para la identación
                    //cierro el archivo
                    archivoactualizado.close();
                    breturn = 1;         
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_socket.h","SetParamsCentral", e.what());
                    breturn = -1;
                }
                return breturn;
            }                             

    };


}
#endif