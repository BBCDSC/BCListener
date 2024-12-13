#include <iostream>
#include <unistd.h>
#include <list>
#include <string>
#include "clases/dbs/db_exec_postgres.h"
#include "clases/tools/tool_log.h"
#include "clases/tools/tool_security.h"
#include "clases/objetos/bclogerrores.h"
#include "clases/objetos/blogcacciones.h"

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
                    string sSQL = "SELECT public.\"fnlogAcciones\"('I'," + to_string(icategoria) +",'"+ accion.c_str()  + "','"+ resultado.c_str() + "')";
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
                    string sSQL ="SELECT public.\"fnlogAcciones\"('I'," + to_string(ierror) +",'"+ accion + "')";
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