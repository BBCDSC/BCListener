#include <iostream>
#include <unistd.h>
#include <list>
#include <string>
#include "../dbs/db_exec_postgres.h"
#include "../tools/tool_log.h"
#include "../tools/tool_security.h"
#include "../objetos/bclogerrores.h"
#include "../objetos/blogcacciones.h"

using namespace std;

/**
 * @brief namespace para manejo de registros en el log.
 * 
 */
namespace Logs
{
    /**
     * @brief Clase de log en el sistema
     * 
     */
    class log
    {
        public:
            /**
             * @brief Registra una acción en el log de Acciones
             * 
             * @param accion Acción ejecutada
             * @param resultado Resultado obtenido
             * @param icategoria Categoría de registro
             * @return int Valor debe ser > 0 
             */
            static int setAccionPG(string accion, string resultado, int icategoria)
            {
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *pgresultado;
                int rows = 0;
                int ireturn = 0;            
                try
                {
                    string sSQL = "SELECT * FROM sp.\"logsacciones\"('I', 0, CAST("+to_string(icategoria)+" AS SMALLINT),'"+accion+"','"+resultado+"', CAST(4 AS SMALLINT))";
                    pgresultado = oPostgres.ExecFunction(sSQL);
                    if (pgresultado != NULL) {
                        rows = PQntuples(pgresultado);
                        if(rows == 1)
                        {
                            ireturn= atoi(PQgetvalue(pgresultado,0,0));
                        }
                    }
                }
                catch(const std::exception& e)
                {
                    Seguridad::cLog::ErrorSistema("log_resgistros.h","setAccion", e.what());
                    std::cout << "Error: " << e.what() << std::endl;
                    ireturn = -1;
                }
                return ireturn;
            };

            /**
             * @brief registro en el log de errores del sistema.
             * 
             * @param accion Acción ejecutada que refiere el error.
             * @param ierror Numero de error de acuerdo al catálogo de errores.
             * @return int Valor debe ser > 0 
             */
            static int setErrorPG(string accion, int ierror)
            {
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *pgresultado;
                int rows = 0;
                int ireturn = 0;               
                try
                {
                    string sSQL ="SELECT * FROM sp.\"logserrores\"('I', 0, CAST("+to_string(ierror)+" AS SMALLINT),'"+accion+"',CAST(4 AS SMALLINT))";
                    pgresultado = oPostgres.ExecFunction(sSQL);
                    if (pgresultado != NULL) {
                        rows = PQntuples(pgresultado);
                        if(rows == 1)
                        {
                            ireturn = atoi(PQgetvalue(pgresultado,0,0));
                        }
                    }
                }
                catch(const std::exception& e)
                {
                    Seguridad::cLog::ErrorSistema("log_resgistros.h","setError", e.what());
                    std::cout << "Conexión error: " << e.what() << std::endl;
                    ireturn = -1;
                }
                return ireturn;
            };
    };
        
}