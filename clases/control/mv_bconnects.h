#ifndef mvconnects_h
#define mvconnects_h

/**
 * @file mv_bconnects.h
 * @author JAGG
 * @brief Clase para controlar las transacciones de velidacion del BConnect
 * @version 0.1
 * @date 2024-12-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <unistd.h>
#include <list>
#include <string>
#include "../objetos/bconnect.h"
#include "../json.hpp"
#include "../config/config_cons.h"
#include "../config/config_postgres.h"
#include "../objetos/bcParamPostgres.h"
#include "../dbs/db_exec_postgres.h"
#include "../tools/tool_log.h"
#include "../tools/tool_security.h"
using namespace std;

/**
 * @brief Namespace para controlar las transacciones
 * 
 */
namespace Transacciones
{
    /**
     * @brief Clase para controlar las transacciones de velidacion del BConnect
     * 
     */
    class MvBConnects
    {
        private:


        public:

      
            /**
             * @brief Registra una nueva BConnect
             * 
             * @param macaddress 
             * @return Objetos::bConnect 
             */
            static Objetos::bConnect Nuevo(string macaddress){
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *resultado;
                Objetos::bConnect bconect;
                int rows = 0;
                resultado = NULL;
                try{
                    resultado = oPostgres.ExecFunction("SELECT public.fncatbconnetcs('I'," + macaddress+ ",0)");
                    if (resultado != NULL) {
                        rows = PQntuples(resultado);
                        if(rows == 1)
                        {
                            bconect.IdBConnect = atoi(PQgetvalue(resultado,0,0));
                            bconect.iSocket = atoi(PQgetvalue(resultado,0,2));
                        }
                    }
                }
                catch(const std::exception& e){
                    std::cout << "Error en el constructor." << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_postgres.h","DBConnecPostgres", e.what());
                    bconect.IdBConnect = 0;
                    bconect.iSocket = 0;
                }  

                return bconect;
            }
            /**
             * @brief Valida si la BConnect está dada de alta sino la inserta con status pendiente de autorizar
             * 
             * @param macaddress 
             * @return Objetos::bConnect 
             */
            static Objetos::bConnect Validacion(string macaddress){
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *resultado;
                Objetos::bConnect bconect;
                int rows = 0;
                resultado = NULL;                
                try{
                    resultado = oPostgres.ExecFunction("SELECT public.fncatbconnetcs('B'," + macaddress+ ",0)");
                    if (resultado != NULL) {
                        rows = PQntuples(resultado);
                        if(rows == 1)
                        {
                            bconect.IdBConnect = atoi(PQgetvalue(resultado,0,0));
                            bconect.iSocket = atoi(PQgetvalue(resultado,0,2));
                        }
                    }
                }
                catch(const std::exception& e){
                    std::cout << "Error en el constructor." << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_postgres.h","DBConnecPostgres", e.what());
                    bconect.IdBConnect = 0;
                    bconect.iSocket = 0;
                }  

                return bconect;
            }

            /**
             * @brief Efectua el cambio de status de una BConnect
             * 
             * @param macaddress 
             * @param status 
             * @return Objetos::bConnect 
             */
            static Objetos::bConnect CambioStatus(string macaddress, int status){
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *resultado;
                Objetos::bConnect bconect;
                int rows = 0;          
                resultado = NULL;
                try{
                    resultado = oPostgres.ExecFunction("SELECT public.fncatbconnetcs('CS'," + macaddress+ "," + to_string(status) +")");
                    if (resultado != NULL) {
                        rows = PQntuples(resultado);
                        if(rows == 1)
                        {
                            bconect.IdBConnect = atoi(PQgetvalue(resultado,0,0));
                            bconect.iSocket = atoi(PQgetvalue(resultado,0,2));
                        }
                    }
                }
                catch(const std::exception& e){
                    std::cout << "Error en el constructor." << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_postgres.h","DBConnecPostgres", e.what());
                    bconect.IdBConnect = 0;
                    bconect.iSocket = 0;
                }  

                return bconect;
            }


    };

}
#endif