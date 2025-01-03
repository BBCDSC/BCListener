#ifndef config_cons_h
#define config_cons_h

/**
 * @file config_cons.h
 * @author JAGG
 * @brief Almacena las constantes para ser utilizadas en el sistema.
 * @version 0.1
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <string>
#include <iostream>
#include "../tools/tool_security.h"

using namespace std;
/**
 * @brief namespace Configuracion para agrupar las clases que configuran el sistema.
 * 
 */
namespace Configuracion
{
    class Constantes{

        private:

        public:
            /**
             * @brief Constante que tiene la ruta del archivo json de configuración.
             * 
             * @return "config/config.json" 
             */
            static string getJsonPath() { return "config/config.json";  }

            /**
             * @brief Constante que tiene la ruta donde se almacenarán los logs.
             * 
             * @return "logs/";  
             */
            static string getLogPath() { return "/data/codes/BConnect/logs/log"; }

 
    };
   
}
#endif