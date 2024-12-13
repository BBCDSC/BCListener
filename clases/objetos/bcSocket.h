#ifndef bcsocket_h
#define bcsocket_h

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

namespace Configuracion
{
    /**
     * @brief Clase Socket para crear un objeto
     * 
     */
    class bcSocket
    {
         public:
            /**
             * @brief Servidor que va a tener el listener
             * 
             */
            string Server;
            /**
             * @brief Puero para abrir el socket
             * 
             */
            int Puerto;
            /**
             * @brief Tiempo de monitoreo en milisegundos
             * 
             */
            int Tiempo;
            /**
             * @brief Reintentos en caso de no poder establecer la comunicación
             * 
             */
            int Intentos;

            /**
             * @brief Constructor de un objeto Socket
             * 
             */
            bcSocket()
            {
                Server = "";
                Puerto = 0;
                Tiempo = 1;
                Intentos = 1;
            }
        };

    }
    #endif