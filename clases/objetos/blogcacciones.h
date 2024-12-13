#ifndef bcacciones_h
#define bcacciones_h
/**
 * @file bcacciones.h
 * @author JAGG
 * @brief Clase para crear objetos tipo Log Acciones
 * @version 0.1
 * @date 2024-12-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <string>
#include <iostream>
#include <ctime>

using namespace std;

namespace Objetos{
/**
 * @brief Clase para crear objetos tipo Log Acciones
 * 
 */
    class LogAcciones{
        public:
        /**
         * @brief int IdLogAccion
         * 
         */
        int IdLogAccion = 0;
        /**
         * @brief int IdCategoria
         * 
         */
        int IdCategoria = 0;
        /**
         * @brief string cAccion
         * 
         */
        string cAccion = "";
        /**
         * @brief string cResultado
         * 
         */
        string cResultado = "";
        /**
         * @brief time_t fTimeStamp
         * 
         */
        time_t fTimeStamp = time(NULL);
    };
}

#endif