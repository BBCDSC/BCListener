#ifndef cnet_h
#define cnet_h

/**
 * @file tool_net.h
 * @author JAGG
 * @brief Clase para obtener IPv4 y Mac Address
 * @version 0.1
 * @date 2024-11-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <cstdio>
#include "clases/tools/tool_log.h" 

using namespace std;

namespace Configuracion
{
    /**
     * @brief Clase para obtener IPv4 y Mac Address
     * 
     */
    class Net
    {
        private:
            static string exec(const char* cmd, string card) 
            {
                string sresultado = "";
                try
                {
                    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
                    if (!pipe) return "-1";
                    char buffer[128];
                    std::string result = "";
                    while (!feof(pipe.get())) {
                        if (fgets(buffer, 128, pipe.get()) != NULL)
                            result += buffer;
                    }
                    // busco la interface
                    string sinterface = card;
                    size_t found = result.find(sinterface);
                    string sip;
                    if(found!= string::npos)
                    {
                        //segmento la cadena para llegar a la ip
                        sip = result.substr(found,100);
                        //Busco la palabra inet que va antes de la ipaddress
                        size_t found2 = sip.find("inet");
                        //Busco la palabra netm que va despues de la ipaddress
                        size_t found3 = sip.find("netm");
                        //Busco el final del segmento de la ip
                        found3 = found3 - (found2 + 5) - 1;
                        //Obtengo la IP
                        sresultado = sip.substr(found2 + 5,found3);
                    }
                    else{
                        sresultado = "NO_CONNECTED";
                    }

                }
                catch(const std::exception& e){
                    Seguridad::cLog::ErrorSistema("tool_net.h", "exec", e.what());
                    sresultado = "-1";
                }
                return sresultado;
            }        

        public:

        enum Interfaces
        {
            eth0,
            wlan0,
            end0,
        };

        /**
         * @brief Obtiene la Ip de la interface seleccionada
         * 
         * @return IPV4 Address  
         */
        static string getIPv4(Configuracion::Net::Interfaces interfaz)
        {
            string sreturn = "";
            try
            {
                switch (interfaz)
                {
                case Interfaces::eth0 :
                    sreturn = exec("ifconfig", "eth0");
                    break;
                case Interfaces::wlan0 :
                    sreturn = exec("ifconfig", "wlan0");
                    break;
                case Interfaces::end0 :
                    sreturn = exec("ifconfig", "end0");
                    break;                                 
                default:
                    sreturn = exec("ifconfig", "eth0");
                    break;
                }

                return sreturn;
                
            }
            catch(const std::exception& e)
            {
                Seguridad::cLog::ErrorSistema("tool_net.h", "getIPv4", e.what());
                sreturn = "NO_CONNECTED";
            }
            return sreturn;
        }

        /**
         * @brief Obtiene la MAC Address de la interfaz seleccionada.
         * 
         * @param interface 
         * @return string 
         */
        static string getMacAddress(Configuracion::Net::Interfaces  interfaz){
            string sreturn = "";
            string path = "";
            try
            {
                switch (interfaz)
                {
                case Interfaces::eth0 :
                    //Ethernet
                    path = "/sys/class/net/eth0/address";
                    break;
                case Interfaces::end0 :
                    //Ethernet
                    path = "/sys/class/net/end0/address";
                    break;                    
                case Interfaces::wlan0 :
                    //WiFi
                    path = "/sys/class/net/wlan0/address";
                    break;                        
                
                default:
                    path = "/sys/class/net/eth0/address";
                    break;
                }
                

                std::ifstream fopen(path);
                if(fopen.is_open())
                {
                    std::getline(fopen,sreturn);
                    fopen.close();
                }
                else
                {
                    sreturn = "NO_CONNECTED";
                }

            }
            catch(const std::exception& e)
            {
                Seguridad::cLog::ErrorSistema("config_cons.h","getMacAddress",e.what());
                sreturn = "NO_CONNECTED";
            }
            return sreturn;
        }

    };
}

#endif