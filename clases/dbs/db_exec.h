#ifndef dbexec_h
#define dbexec_h
/**
 * @file db_exec.h
 * @author JAGG
 * @brief Gestiona la conexión a la base de datos y ejecuta los store procedures
 * @version 0.1
 * @date 2024-11-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <iostream>
#include <mariadb/mysql.h>
#include <clases/json.hpp>
#include "clases/config_cons.h"
#include <string>
#include <fstream>
#include "clases/bcParamMariaDB.h"
#include "clases/config_mariadb.h"
#include "clases/tool_log.h"
#include "clases/tool_security.h"

/**
 * @brief Namespace para el manejo de bases de datos
 * 
 */
namespace BaseDatos
{
    /**
     * @brief Gestiona la conexión a la base de datos y ejecuta los store procedures
     * 
     */
    class DBConnect
    {

        public:
            static MYSQL* mysql_connection_setup(){
                try
                {
                    //Obtengo parametos
                    Configuracion::bcParamMariaDB objParam;
                    objParam = Configuracion::MariaDB::GetParametrosMariaDB();
                    if(objParam.Puerto > 0)
                    {
                        MYSQL *connection = mysql_init(NULL);
                        if(!mysql_real_connect(connection, objParam.Servidor.c_str(), objParam.Usuario.c_str(), objParam.Password.c_str(),objParam.BaseDatos.c_str(),objParam.Puerto,NULL,0))
                        {
                            std::cout << "Conexión error: " << mysql_error(connection) << std::endl;
                            Seguridad::cLog::ErrorSistema("db_mariadb.h","mysql_connection_setup", mysql_error(connection));
                            exit(1);
                        }
                        return connection;
                    }
                    else{
                        std::cout << "No hay parametros de conexión. "  << std::endl;
                        Seguridad::cLog::ErrorSistema("db_mariadb.h","mysql_connection_setup", "No hay parametros de conexión. ");
                        exit(-1);
                    }
                }
                catch(const std::exception& e){
                    std::cout << "Conexión error: " << e.what() << std::endl;
                    Seguridad::cLog::ErrorSistema("db_mariadb.h","mysql_connection_setup", e.what());
                    exit(-1);
                }    
            }

            /**
             * @brief 
             * 
             * @param connection 
             * @param sql_query 
             * @return MYSQL_RES* 
             */
            static MYSQL_RES* mysql_execute_query(MYSQL *connection,const char *sql_query){
                try
                {

                    if(mysql_query(connection,sql_query))
                    {
                        Seguridad::cLog::ErrorSistema("db_mariadb.h","mysql_execute_query", mysql_error(connection));
                        std::cout << "Conexión error: " << mysql_error(connection) << std::endl;
                        exit(1);
                    } 

                    return mysql_use_result(connection); 
                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("db_mariadb.h","mysql_execute_query", e.what());
                    std::cout << "Conexión error: " << e.what() << std::endl;
                    exit(-1);
                } 
            }
    };

}
#endif