#include <iostream>
#include <unistd.h>
#include "clases/json.hpp"
#include "clases/config/config_cons.h"
#include "clases/config/config_postgres.h"
#include "clases/objetos/bcParamPostgres.h"
#include "clases/dbs/db_exec_postgres.h"
#include "clases/tools/tool_log.h"
#include "clases/tools/tool_security.h"
#include "clases/control/log_registros.h"

using namespace std;


int main()
{
    BaseDatos::DBConnectPostgres oPostgres;
    PGresult *resultado = NULL;
    resultado = oPostgres.ExecFunction("SELECT public.fncatbconnetcs('C1','',0)");
    int i = 0;
    int iResult =  Logs::log::setAccionPG("Message del client: test","correcto",1);
    if (resultado != NULL) {
        int tuplas = PQntuples(resultado);
        int campos = PQnfields(resultado);
        cout << "No. Filas:" << tuplas << endl;
        cout << "No. Campos:" << campos << endl;

        cout << "Los nombres de los campos son:" << endl;

        for (i=0; i<campos; i++) {
            cout << PQfname(resultado,i) << " | ";
        }

        cout << endl << "Contenido de la tabla" << endl;

        for (i=0; i<tuplas; i++) {
            for (int j=0; j<campos; j++) {
                cout << PQgetvalue(resultado,i,j) << " | ";
            }
            cout << endl;
        }
        PQclear(resultado);
    }
}