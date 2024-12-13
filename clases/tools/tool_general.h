#ifndef cgeneral_h
#define cgeneral_h

#include <fstream>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

/**
 * @brief namespace Herramientas con funciones de uso general.
 * 
 */
namespace Herramientas{
    /**
     * @brief Clase de herramientas generales.
     * 
     */
    class Generales{
        
        public:

            /**
             * @brief Función que segmenta una cadena en función de un delimitador.
             * 
             * @param cadena Cadena a segmentar.
             * @param delimitador caracter demlimitador.
             * @return string* Arreglo de cadenas.
             */
            static vector<string> split(string cadena, string delimitador)
            {
                vector<string> areturn;
                // Find first occurrence of the delimiter
                auto pos = cadena.find(delimitador);

                // While there are still delimiters in the string
                while (pos != string::npos) 
                {

                    // Extracting the substring up to the delimiter
                    areturn.push_back(cadena.substr(0, pos));

                    // Erase the extracted part from the original string
                    cadena.erase(0, pos + delimitador.length());

                    // Find the next occurrence of the delimiter
                    pos = cadena.find(delimitador);
                }  
                //Insert the last part
                areturn.push_back(cadena.substr(0, pos));
                return areturn;           
            }

            /**
             * @brief Función valida que la IP ingresada sea valida
             * 
             * @return true 
             * @return false 
             */
            static int validaIP(string sIP)
            {
                int breturn = 1;
                try
                {
                    vector<string> aOctetos;
                    int iocteto;
                    aOctetos = Herramientas::Generales::split(sIP,".");
                    for(string octeto:aOctetos)
                    {
                        iocteto = stoi(octeto);
                        if(iocteto < 0 || iocteto > 255)
                        {
                            breturn = 0;
                            return breturn;
                        }
                    }
                    breturn = 1;
                }
                catch(const std::exception& e)
                {
                    //std::cerr << "error " << e.what() << '\n';
                    breturn = -1;
                }
                return breturn;
            }


    };
}
#endif