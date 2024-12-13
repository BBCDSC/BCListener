#ifndef bcconnect_h
#define bcconnect_h

#include <string>
#include <iostream>
#include <ctime>

using namespace std;

/**
 * @brief Namespace para creación de objetos
 * 
 */
namespace Objetos
{
    /**
     * @brief Clase para crear objetos tipo BConnect
     * 
     */
    class bConnect{
        public:
            /**
             * @brief int IdBConnect
             * 
             */
            int IdBConnect = 0;
            /**
             * @brief string MAC
             * 
             */
            string MAC = "";
            /**
             * @brief int iSocket
             * 
             */
            int iSocket = 0;
            /**
             * @brief time_t fLastConnect
             * 
             */
            time_t fLastConnect = time(NULL);
            /**
             * @brief int IdStatus
             * 
             */
            int IdStatus = 0;

    };
}
#endif