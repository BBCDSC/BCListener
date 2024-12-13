#ifndef bcerrores_h
#define bcerrores_h

#include <string>
#include <iostream>
#include <ctime>

using namespace std;

namespace Objetos{
    /**
     * @brief Clase para crear objetos tipo Log Errores
     * 
     */
    class LogErrores
    {
        public:
            /**
             * @brief int IdlogError
             * 
             */
            int IdlogError = 0;
            /**
             * @brief int IdError = 0;
             * 
             */
            int IdError = 0;
            /**
             * @brief string cAccion
             * 
             */
            string cAccion = "";
            /**
             * @brief time_t fTimeStamp
             * 
             */
            time_t fTimeStamp = time(NULL);
    };
}
#endif