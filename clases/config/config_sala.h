#ifndef confsala_h
#define confsala_h

/**
 * @file config_sala.h
 * @author JAGG
 * @brief Parametros confifuración para asociar al listener con la sala
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
    class SalaInfo
    {
        public:
            /**
             * @brief Obtiene el id de la sala donde está instalado el listener
             * 
             * @return Id de la sala 
             */
            static string GetSalaInfo(){
                //creo el objeto json
                nlohmann::json jsonData;            
                string sreturn = ""; 
                Seguridad::Aes3 objDenc; 
                try{
                    //Abro el archivo  para lectura
                    ifstream archivo(Constantes::getJsonPath());
                    //transfiero el stream aun objeto json
                    archivo >> jsonData;
                    //leo los parametros del objeto json
                    sreturn = objDenc.Desencriptar(jsonData["sala"]);
                    //cierro el archivo
                    archivo.close();       
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_sala.h","GetSalaInfo", e.what());
                    sreturn = "";
                }
                return sreturn;
            }  

            /**
             * @brief Asigna el valor del id de la sala donde se instala el listener
             * 
             * @param _Sala 
             * @return int 1 correcto, -1 error
             */
            static int SetParamSala(string _Sala){
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
                    jsonData["sala"] = objEnc.Encriptar(_Sala);
                    //Abro archivo para escritura
                    ofstream archivoactualizado(Constantes::getJsonPath());
                    //Mando el jason al stream
                    archivoactualizado << jsonData.dump(4); //El 4 es para la identación
                    //cierro el archivo
                    archivoactualizado.close();
                    breturn = 1;         
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("config_sala.h","SetParamSala", e.what());
                    breturn = -1;
                }
                return breturn;
            }    

    };


}
#endif