#ifndef clogs_h
#define clogs_h
/**
 * @file tool_log.h
 * @author JAGG
 * @brief Clase destinada a registrar el log de la aplicación
 * @version 0.1
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "../config/config_cons.h"

using namespace std;

namespace Seguridad{
    /**
     * @brief Clase destinada a registrar el log de la aplicación.
     * 
     */
    class cLog{

        private:

        bool static ValidaArchivo(string const& nombre)
        {
            bool breturn = 0;
            struct stat buffer;
            return(stat(nombre.c_str(), &buffer) == 0);
        }


        public:
            /**
             * @brief Función para guardar en un log, los errores del sistema
             * 
             * @param archivo Archivo donde se generó el error
             * @param metodo Función donde se generó el error
             * @param error Descripción del error
             */
            void static ErrorSistema(string archivo, string metodo, string error){
                try
                {
                    //Formateo la fecha yyyymmdd
                    string sfecha = "";
                    auto t = time(nullptr);
                    auto fecha = *std::localtime(&t);
                    ostringstream oss;
                    oss << put_time(&fecha, "%Y%m%d");
                    string sfecha1 = oss.str();
                    string logfile = "";
                    logfile = "logs/log";
                    logfile = logfile + sfecha1 + ".txt";
                    bool breturn = ValidaArchivo(logfile);

                    //Armo registro
                    ostringstream osstm;
                    osstm << put_time(&fecha, "%Y/%m/%d %H:%M:%S");
                    string stimestamp = osstm.str();

                    string registro;
                    registro = stimestamp + "|"+ archivo + "|" + metodo + "|" + error ;

                    if(!breturn)
                    {
                        //No existe el archivo
                        ofstream fichero(logfile, ios::out);
                        fichero << registro << endl;
                        fichero.close();
                    }
                    else
                    {
                        //No existe el archivo
                        ofstream fichero(logfile,ios::app);
                        fichero << registro << endl;
                        fichero.close();
                    }

                }
                catch(const std::exception& e)
                {
                    cout << e.what();
                }

            }

    };
}
#endif