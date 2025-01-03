#ifndef dbexecpost_h
#define dbexecpost_h
/**
 * @file db_exec_postgres.h
 * @author JAGG
 * @brief Clase que gestiona la conexión con postgres
 * @version 0.1
 * @date 2024-12-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <iostream>
#include "clases/postgresql/libpq-fe.h"
#include <unistd.h>
#include "../json.hpp"
#include "../config/config_cons.h"
#include "../config/config_postgres.h"
#include "../objetos/bcParamPostgres.h"
#include "../tools/tool_log.h"
#include "../tools/tool_security.h"
using namespace std;

namespace BaseDatos
{
/**
 * @brief Clase que gestiona la conexión con postgres
 * 
 */
    class DBConnectPostgres
    {

        Configuracion::bcParamPostgres oParamPostgres;
        string strConn = "";
        PGconn *cnn = NULL;
        PGresult *result = NULL;

        public:

            /**
             * @brief Constructor DBConnectPostgres , arma la cadena de conexión a postgress
             * 
             */
            DBConnectPostgres()
            {
                try
                {
                    oParamPostgres = Configuracion::Postgresql::GetParametrosPostgres();
                    if(oParamPostgres.BaseDatos != "")
                    {
                        strConn = "dbname=" + oParamPostgres.BaseDatos + " user= " + oParamPostgres.Usuario + " password= " + oParamPostgres.Password + " host=" + oParamPostgres.Servidor +  " port=" + to_string(oParamPostgres.Puerto);
                    }
                    else{
                        strConn = "";
                        std::cout << "No hay parametros de conexión. "  << std::endl;
                        Seguridad::cLog::ErrorSistema("db_exec_postgres.h","DBConnecPostgres", "No hay parametros de conexión. ");
                    }                    
                }
                catch(const std::exception& e){
                    std::cout << "Error en el constructor." << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_postgres.h","DBConnecPostgres", e.what());
                }   
            }

            /**
             * @brief Ejecuta funciones de Postgress
             * 
             * @param sQL Cadena SQL que select function.
             * @return PGresult* resultado de la ejecución de la función.
             */
            PGresult* ExecFunction(string sQL)
            {
                result = NULL;
                try
                {
                    const char *conninfo = strConn.c_str();
                    PGconn *conn = PQconnectdb(conninfo);
                    //string sQL = "SELECT * FROM cat.""Parametros"";";

                    if (PQstatus(conn) == CONNECTION_OK) {
                        //cout << "Estamos conectados a PostgreSQL!" << endl;
                        result = PQexec(conn, sQL.c_str());
                        //cout<< PQresultStatus(result) <<endl;
                        if(PQresultStatus(result) == PGRES_FATAL_ERROR)
                        {
                            cout<< PQerrorMessage(conn) <<endl;
                            std::cout << "Conexión error: " << PQerrorMessage(conn)  << std::endl;
                            Seguridad::cLog::ErrorSistema("db_exec_postgres.h","ExecFunction", PQerrorMessage(conn) );
                        }
                    } else {
                        cout << "Error de conexion" << PQerrorMessage(conn) <<endl;
                        Seguridad::cLog::ErrorSistema("db_exec_postgres.h","ExecFunction", PQerrorMessage(conn));
                        result = NULL;                    
                    }
                    PQfinish(cnn);
                }
                catch(const std::exception& e)
                {
                    std::cout << "Error ExecFunction." << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_postgres.h","ExecFunction", e.what());
                }
                return result;
            }
    };

}
#endif