#ifndef bcsparampostgres_h
#define bcsparampostgres_h

#include <string>
#include <fstream>
#include <iostream>    

using namespace std;

namespace Configuracion
{    
    /**
     * @brief Clase para controlar los parametros de Postgres
     * 
     */
    class bcParamPostgres
    {
        public:
        /**
         * @brief Servidor de base de datos.
         * 
         */
            string Servidor;
            /**
             * @brief Nombre de la base de datos.
             * 
             */
            string BaseDatos;
            /**
             * @brief Usuario de conexión a la base de datos.
             * 
             */
            string Usuario;
            /**
             * @brief Contraseña de conexión.
             * 
             */
            string Password;
            /**
             * @brief Puerto el motor de la base d datos.
             * 
             */
            int Puerto;

            /**
             * @brief Constructor
             * 
             */
            bcParamPostgres()
            {
                Servidor = "";
                Puerto = 0;
            }

    };
}
#endif