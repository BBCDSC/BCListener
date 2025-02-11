#ifndef dbexecsql_h
#define dbexecsql_h

#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <unistd.h>
#include "../json.hpp"
#include "../config/config_cons.h"
#include "../config/config_sql.h"
#include "../objetos/bcParamPostgres.h"
#include "../tools/tool_log.h"
#include "../tools/tool_security.h"

using namespace std;

namespace BaseDatos
{
    class DBConnectSQL
    {
        Configuracion::bcParamPostgres oParamSQLServer;
        string strConn = "";
        SQLHENV henv = SQL_NULL_HENV;
        SQLHDBC hdbc = SQL_NULL_HDBC;
        SQLHSTMT hstmt = SQL_NULL_HSTMT;

        public:
            DBConnectSQL() {
                try
                {
                    oParamSQLServer = Configuracion::Sqlserver::GetParametrosCentral();
                    if (oParamSQLServer.BaseDatos != "")
                    {
                        strConn = "DRIVER={ODBC Driver 17 for SQL Server};"
                        "SERVER=" + oParamSQLServer.Servidor + ";"
                        "DATABASE=" + oParamSQLServer.BaseDatos + ";"
                        "UID=" + oParamSQLServer.Usuario + ";"
                        "PWD=" + oParamSQLServer.Password + ";"
                        "TrustServerCertificate=yes;"
                        "Encrypt=yes;";
                        cout << "strConn: "+ strConn << endl;
                    }
                    else
                    {
                        strConn = "";
                        std::cout << "No hay parámetros de conexión." << std::endl;
                        Seguridad::cLog::ErrorSistema("db_exec_sql.h", "DBConnectSQL", "No hay parámetros de conexión.");
                    }
                }
                catch (const std::exception &e)
                {
                    std::cout << "Error en el constructor." << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "DBConnectSQL", e.what());
                }
            }

        /**
         * @brief Ejecuta consultas en SQL Server
         * 
         * @param sQL Cadena SQL que se desea ejecutar.
         * @return SQLRETURN resultado de la ejecución de la consulta.
         */
        SQLRETURN ExecQuery(const string &sQL)
        {
            SQLRETURN ret = SQL_ERROR;
            try
            {
                // Allocate environment handle
                if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv) == SQL_SUCCESS)
                {
                    // Set the ODBC version environment attribute
                    if (SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0) == SQL_SUCCESS)
                    {
                        // Allocate connection handle
                        if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) == SQL_SUCCESS)
                        {
                            // Connect to SQL Server
                            ret = SQLDriverConnect(hdbc, NULL, (SQLCHAR *)strConn.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
                            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
                            {
                                // Allocate statement handle
                                if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) == SQL_SUCCESS)
                                {
                                    // Execute the SQL query
                                    ret = SQLExecDirect(hstmt, (SQLCHAR *)sQL.c_str(), SQL_NTS);
                                    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
                                    {
                                        // Process the result set if needed
                                    }
                                    else
                                    {
                                        std::cout << "Error al ejecutar la consulta." << std::endl;
                                        Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecQuery", "Error al ejecutar la consulta.");
                                    }
                                }
                                else
                                {
                                    std::cout << "Error al asignar el handle de la sentencia." << std::endl;
                                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecQuery", "Error al asignar el handle de la sentencia.");
                                }
                            }
                            else
                            {
                                std::cout << "Error de conexión a SQL Server." << std::endl;
                                Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecQuery", "Error de conexión a SQL Server.");
                            }
                        }
                        else
                        {
                            std::cout << "Error al asignar el handle de la conexión." << std::endl;
                            Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecQuery", "Error al asignar el handle de la conexión.");
                        }
                    }
                    else
                    {
                        std::cout << "Error al establecer el atributo de versión ODBC." << std::endl;
                        Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecQuery", "Error al establecer el atributo de versión ODBC.");
                    }
                }
                else
                {
                    std::cout << "Error al asignar el handle del entorno." << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecQuery", "Error al asignar el handle del entorno.");
                }
            }
            catch (const std::exception &e)
            {
                std::cout << "Error en ExecQuery." << std::endl;
                Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecQuery", e.what());
            }

            return ret;
        }


         /**
         * @brief Ejecuta un stored procedure en SQL Server
         * 
         * @param procedureName Nombre del stored procedure.
         * @param params Parámetros del stored procedure.
         * @return SQLRETURN resultado de la ejecución del stored procedure.
         */
        tuple<SQLRETURN, int, int> ExecStoredProcedure(const string &procedureName, const vector<string> &params)
        {
            SQLRETURN ret = SQL_ERROR;
            int result = 0;
            int idBConnect = 0;
            try
            {
                // Verificar que tengamos los parámetros necesarios
                if (params.size() < 2) {
                    std::cout << "Error: Número insuficiente de parámetros" << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecStoredProcedure", "Número insuficiente de parámetros");
                    return {ret, result, idBConnect};
                }

                // Validar y convertir IdSala
                int idSala;
                try {
                    idSala = std::stoi(params[1]);
                } catch (const std::invalid_argument& e) {
                    std::cout << "Error: IdSala no es un número válido" << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecStoredProcedure", "IdSala no es un número válido");
                    return {ret, result, idBConnect};
                } catch (const std::out_of_range& e) {
                    std::cout << "Error: IdSala fuera de rango" << std::endl;
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecStoredProcedure", "IdSala fuera de rango");
                    return {ret, result, idBConnect};
                }

                ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
                if (ret != SQL_SUCCESS) return {ret, result, idBConnect};
                
                ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
                if (ret != SQL_SUCCESS) return {ret, result, idBConnect};
                
                ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
                if (ret != SQL_SUCCESS) return {ret, result, idBConnect};

                ret = SQLDriverConnect(hdbc, NULL, (SQLCHAR*)strConn.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
                if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return {ret, result, idBConnect};

                ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
                if (ret != SQL_SUCCESS) return {ret, result, idBConnect};

                string sql = "EXEC " + procedureName + " @cMAC=?, @IdSala=?";
                cout << "Query: " + sql << endl;
                cout << "Parámetros: MAC=" << params[0] << ", IdSala=" << idSala << endl;

                ret = SQLPrepare(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
                if (ret != SQL_SUCCESS) {
                    SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
                    SQLINTEGER nativeError;
                    SQLSMALLINT length;
                    SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, &nativeError, message, sizeof(message), &length);
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecStoredProcedure", (char*)message);
                    return {ret, result, idBConnect};
                }

                // Bind MAC parameter
                SQLLEN cbMAC = SQL_NTS;
                ret = SQLBindParameter(hstmt, 1, 
                                    SQL_PARAM_INPUT, 
                                    SQL_C_CHAR, 
                                    SQL_VARCHAR, 
                                    params[0].length(), 
                                    0, 
                                    (SQLPOINTER)params[0].c_str(), 
                                    params[0].length(), 
                                    &cbMAC);
                if (ret != SQL_SUCCESS) {
                    SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
                    SQLINTEGER nativeError;
                    SQLSMALLINT length;
                    SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, &nativeError, message, sizeof(message), &length);
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecStoredProcedure", (char*)message);
                    return {ret, result, idBConnect};
                }

                // Bind IdSala parameter
                SQLLEN cbIdSala = 0;
                ret = SQLBindParameter(hstmt, 2, 
                                    SQL_PARAM_INPUT, 
                                    SQL_C_LONG, 
                                    SQL_INTEGER, 
                                    0, 
                                    0, 
                                    &idSala, 
                                    0, 
                                    &cbIdSala);
                if (ret != SQL_SUCCESS) {
                    SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
                    SQLINTEGER nativeError;
                    SQLSMALLINT length;
                    SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, &nativeError, message, sizeof(message), &length);
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecStoredProcedure", (char*)message);
                    return {ret, result, idBConnect};
                }

                ret = SQLExecute(hstmt);
                if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
                    if (SQLFetch(hstmt) == SQL_SUCCESS) {
                        SQLLEN indicator1, indicator2;
                        // Get IdBConnect from first column
                        SQLGetData(hstmt, 1, SQL_C_LONG, &idBConnect, sizeof(idBConnect), &indicator1);
                        // Get Result from second column
                        SQLGetData(hstmt, 2, SQL_C_LONG, &result, sizeof(result), &indicator2);
                    }
                } else {
                    SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
                    SQLINTEGER nativeError;
                    SQLSMALLINT length;
                    SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, &nativeError, message, sizeof(message), &length);
                    Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecStoredProcedure", (char*)message);
                }
            }
            catch (const std::exception &e)
            {
                std::cout << "Error en ExecStoredProcedure: " << e.what() << std::endl;
                Seguridad::cLog::ErrorSistema("db_exec_sql.h", "ExecStoredProcedure", e.what());
            }
            cout << "DAtos: "+to_string(idBConnect)+" "+to_string(result)<<endl;
            return {ret, result, idBConnect};
        }

        /**
         * @brief Cierra la conexión con SQL Server
         * 
         */
        void CloseConnection()
        {
            if (hstmt != SQL_NULL_HSTMT)
            {
                SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
            }
            if (hdbc != SQL_NULL_HDBC)
            {
                SQLDisconnect(hdbc);
                SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
            }
            if (henv != SQL_NULL_HENV)
            {
                SQLFreeHandle(SQL_HANDLE_ENV, henv);
            }
        }

    };
}
#endif