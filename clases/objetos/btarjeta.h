#ifndef bctarjeta_h
#define bctarjeta_h

#include <string>
#include <iostream>
#include <ctime>

using namespace std;

/**
 * @brief Namespace para creación de objetos
 * 
 */
namespace Objetos{
    
    /**
     * @brief Clase para crear objetos tipo Tarjeta
     * 
     */
    class bTarjeta{
        public:
            /**
             * @brief int IdTarjeta
             * 
             */
            int IdTarjeta = 0;
            /**
             * @brief string Ctarjeta
             * 
             */
            string Ctarjeta = "";
            /**
             * @brief int IdTipoMoneda
             * 
             */
            int IdTipoMoneda = 0;
            /**
             * @brief string CtipoMoneda
             * 
             */
            string CtipoMoneda = "";
            /**
             * @brief double Msaldo
             * 
             */
            double Msaldo = 0;

    };
}
#endif