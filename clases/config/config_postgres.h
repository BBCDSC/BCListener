#ifndef postgrescfg_h
#define postgrescfg_h

/**
 * @file config_postgres.h
 * @author JAGG
 * @brief Gestiona los parámetros de conexióna la base de datos en Postgres
 * @version 0.1
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <clases/json.hpp>
#include "clases/config/config_cons.h"
#include "clases/objetos/bcParamPostgres.h"
#include <string>
#include <fstream>
#include <iostream>
#include "clases/tools/tool_log.h"
#include "clases/tools/tool_security.h"

using namespace std;

namespace Configuracion
{
    /**
     * @brief Clase PostgreSQL 
     * 
     */
    class Postgresql
    {
        private:

        public:

            /**
             * @brief Obtiene la cadena de conexión a PostgreSQL del archivo de configuración.
             * 
             * @return * Cadena de conexión 
             */
            static string GetConnectionString(){
                //creo el objeto json
                nlohmann::json jsonData; 
                Seguridad::Aes3 objDenc;            
                string sreturn;
                string _Server;
                string _DB;
                string _User;
                string _Pass;
                string _Puerto;

                try{
                    //Abro el archivo  para lectura
                    ifstream archivo(Constantes::getJsonPath());
                    //transfiero el stream aun objeto json
                    archivo >> jsonData;
                    //leo los parametros del objeto json
                    _Server = objDenc.Desencriptar( jsonData["postgres"]["servidor"]);
                    _DB = objDenc.Desencriptar(jsonData["postgres"]["db"]);
                    _User = objDenc.Desencriptar(jsonData["postgres"]["user"]);
                    _Pass = objDenc.Desencriptar(jsonData["postgres"]["pass"]);
                    _Puerto = objDenc.Desencriptar(jsonData["postgres"]["puerto"]);
                    //cierro el archivo
                    archivo.close();
                    //armo la cadena dec conexión
                    sreturn = "jdbc:postgres://" + _Server + ":" + _Puerto + "/" + _DB + "?user=" + _User + "&password=" + _Pass;            
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_postgres.h","GetConnectionString", e.what());
                    sreturn = "";
                }
                return sreturn;
            }

            /**
             * @brief Obtiene un objeto con los parametros de conexión a una base de datos
             * 
             * @return Configuracion::bcParamPostgres 
             */
            static Configuracion::bcParamPostgres GetParametrosPostgres(){
                //creo el objeto json
                nlohmann::json jsonData; 
                Seguridad::Aes3 objDenc;    
                Configuracion::bcParamPostgres objreturn;        

                try{
                    //Abro el archivo  para lectura
                    ifstream archivo(Constantes::getJsonPath());
                    //transfiero el stream aun objeto json
                    archivo >> jsonData;
                    //leo los parametros del objeto json
                    objreturn.Servidor = objDenc.Desencriptar( jsonData["postgres"]["servidor"]);
                    objreturn.BaseDatos = objDenc.Desencriptar(jsonData["postgres"]["db"]);
                    objreturn.Usuario = objDenc.Desencriptar(jsonData["postgres"]["user"]);
                    objreturn.Password = objDenc.Desencriptar(jsonData["postgres"]["pass"]);
                    string spuerto = objDenc.Desencriptar(jsonData["postgres"]["puerto"]);
                    objreturn.Puerto = stoi(spuerto); 
                    //cierro el archivo
                    archivo.close();
                    return objreturn;
       
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_postgres.h","GetConnectionString", e.what());
                    objreturn.Servidor = "";
                    objreturn.Puerto = 0; 
                }
                return objreturn;
            }

             /**
             * @brief Actualiza los parámetros de conexión para base de datos postgres.
             * 
             * @param _Server IP del servidor o localhost
             * @param _Puerto Socket / Puerto
             * @param _DB Base de datos
             * @param _User Usuario de la base de datos
             * @param _Pass Password de la base de datos
             * @return 1 Correcto
             * @return 0 Error
             */
            static int SetParams(string _Server, string _Puerto, string _DB, string _User, string _Pass){
                //creo el objeto json
                nlohmann::json jsonData;  
                Seguridad::Aes3 objEnc;          
                int breturn = 0;

                try{
                    //Abro el archivo de config para lectura
                    ifstream archivo(Constantes::getJsonPath());
                    //Transfiero el stream aun objeto json
                    archivo >> jsonData;
                    //Cierro el archivo
                    archivo.close();
                    //Modifico parámetros
                    
                    jsonData["postgres"]["servidor"] = objEnc.Encriptar(_Server);
                    jsonData["postgres"]["db"]= objEnc.Encriptar(_DB);
                    jsonData["postgres"]["user"] = objEnc.Encriptar(_User);
                    jsonData["postgres"]["pass"] = objEnc.Encriptar(_Pass);
                    jsonData["postgres"]["puerto"] = objEnc.Encriptar(_Puerto);
                    //Abro archivo para escritura
                    ofstream archivoactualizado(Constantes::getJsonPath());
                    //Mando el jason al stream
                    archivoactualizado << jsonData.dump(4); //El 4 es para la identación
                    //cierro el archivo
                    archivoactualizado.close();
                    breturn = 1;         
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_postgres.h","SetParams", e.what());
                    breturn = -1;
                }
                return breturn;
            }        
    };
}
#endif
