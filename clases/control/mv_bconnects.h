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
#include "../dbs/db_exec_sql.h"
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
     * @brief Clase para controlar las transacciones de validacion del BConnect
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
            static Objetos::bConnect Nuevo(int idBigConect,string macaddress, int status){
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *resultado;
                Objetos::bConnect bconect;
                int rows = 0;
                resultado = NULL;
                try{
                    resultado = oPostgres.ExecFunction("SELECT * FROM bcn.\"fnBigConnects\"('I', cast("+to_string(idBigConect)+" as smallint),'"+macaddress+"', cast("+to_string(status)+" as smallint))");
                    if (resultado != NULL) {
                        rows = PQntuples(resultado);
                        if(rows == 1)
                        {
                            bconect.IdBConnect = atoi(PQgetvalue(resultado,0,0));
                            bconect.MAC = PQgetvalue(resultado,0,1);
                            bconect.IdStatus = atoi(PQgetvalue(resultado,0,2));
                            bconect.status = PQgetvalue(resultado,0,3);
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
             * @brief Valida si la BConnect está dada de alta en la sala
             * 
             * @param macaddress 
             * @return Objetos::bConnect 
             */
            static Objetos::bConnect ValidacionLocal(string macaddress){
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *resultado;
                Objetos::bConnect bconect;
                int rows = 0;
                resultado = NULL;                
                try{
                    cout << "MAC : "+ macaddress << endl;
                    resultado = oPostgres.ExecFunction("SELECT * FROM bcn.\"fnBigConnects\"('CM', cast(0 as smallint), '"+macaddress+"')");
                    if (resultado != NULL) {
                        rows = PQntuples(resultado);
                        if(rows == 1)
                        {
                            bconect.IdBConnect = atoi(PQgetvalue(resultado,0,0));
                            bconect.MAC = PQgetvalue(resultado,0,1);
                            bconect.IdStatus = atoi(PQgetvalue(resultado,0,2));
                            bconect.status = PQgetvalue(resultado,0,3);
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
            static Objetos::bConnect CambioStatus(Objetos::bConnect Bc, int status){
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *resultado;
                Objetos::bConnect bconect;
                int rows = 0;          
                resultado = NULL;
                try{ 
                    cout<<"QUERY: SELECT * FROM bcn.\"fnBigConnects\"('AS', cast("+to_string(Bc.IdBConnect)+" as smallint), '"+Bc.MAC+"', cast("+to_string(status)+" as smallint))"<<endl;
                    resultado = oPostgres.ExecFunction("SELECT * FROM bcn.\"fnBigConnects\"('AS', cast("+to_string(Bc.IdBConnect)+" as smallint), '"+Bc.MAC+"', cast("+to_string(status)+" as smallint))");
                    if (resultado != NULL) {
                        rows = PQntuples(resultado);
                        if(rows == 1)
                        {
                            bconect.IdBConnect = atoi(PQgetvalue(resultado,0,0));
                            bconect.MAC = PQgetvalue(resultado,0,1);
                            bconect.IdStatus = atoi(PQgetvalue(resultado,0,2));
                            bconect.status = PQgetvalue(resultado,0,3);
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
             * @brief Valida si la BConnect está dada de alta en central y a que sala pertenece.
             * @param macaddress
             * @param Idsala
            */
            static pair<int, int> ValidacionCentral(vector<string> params){
                cout << "Entra en la validacionCentral" << endl;
                BaseDatos::DBConnectSQL dbConnect;
                int res = 0;
                int idBConect = 0;
                
                try{
                   auto [resSP, result, idBigConect] = dbConnect.ExecStoredProcedure("spbcn.ValidaBigConnects", params);
                    // Verificar el resultado de la ejecución
                    res = result;
                    idBConect = idBigConect;
                    if (resSP == SQL_SUCCESS || resSP == SQL_SUCCESS_WITH_INFO)
                    {
                        cout << "El SP se ejecuto de manera correcta" << endl;
                    }
                    else
                    {
                        cout << "Error al ejecutar el stored procedure." << endl;
                    }

                    // Cerrar la conexión con la base de datos
                    dbConnect.CloseConnection();
                }
                catch(const std::exception& e){
                    std::cout << "Error en la ValidacionCentral." << std::endl;
                    Seguridad::cLog::ErrorSistema("mv_bconnects.h","ValidacionCentral", e.what());
                    dbConnect.CloseConnection();
                }  

                return {res, idBConect};
            }

    };

}
#endif