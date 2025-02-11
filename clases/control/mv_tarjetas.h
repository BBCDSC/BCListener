#ifndef mvtarjetas_h
#define mvtarjetas_h

/**
 * @file mv_bconnects.h
 * @author JAGG
 * @brief Clase para controlar las transacciones de velidacion del BConnect
 * @version 0.1
 * @date 2024-12-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <unistd.h>
#include <list>
#include <string>
#include "../json.hpp"
#include "../config/config_cons.h"
#include "../config/config_postgres.h"
#include "../dbs/db_exec_postgres.h"
#include "../objetos/btarjeta.h"
#include "../tools/tool_log.h"
#include "../tools/tool_security.h"
using namespace std;

/**
 * @brief Namespace para controlar las transacciones
 * 
 */
namespace Transacciones
{
    /**
     * @brief Clase para controlar las transacciones de la tarjeta
     * 
     */
    class MvTarjetas 
    {
        private:


        public:

      
            /**
             * @brief Consulta los datos de una tarjeta por su UID
             * 
             * @param uid
             * @return Objetos::bConnect 
             */
            static vector<Objetos::bTarjeta> ConsultaTarjetaByUid(string uid){
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *resultado = nullptr;
                vector<Objetos::bTarjeta> tarjetas;
                try{
                    resultado = oPostgres.ExecFunction("SELECT * FROM sp.\"mvtarjetassaldos\"('CST','"+uid+"')");
                    if (resultado != nullptr) {
                        int rows = PQntuples(resultado);
                        for (int i = 0; i < rows; i ++) {
                            Objetos::bTarjeta tarjeta;
                            tarjeta.IdTarjeta = atoi(PQgetvalue(resultado, i, 0));
                            tarjeta.Ctarjeta = PQgetvalue(resultado, i, 1);
                            tarjeta.IdTipoMoneda = atoi(PQgetvalue(resultado, i, 2));
                            tarjeta.CtipoMoneda = PQgetvalue(resultado, i, 3);
                            tarjeta.Msaldo = atof(PQgetvalue(resultado, i, 4));

                            tarjetas.push_back(tarjeta);
                        }
                        PQclear(resultado);
                    }
                }
                catch(const std::exception& e){
                    std::cout << "Error en la consulta de tarjeta." << std::endl;
                    Seguridad::cLog::ErrorSistema("Transacciones/mv_tarjetas.h","ConsultaTarjetaByUid", e.what());
                }  

                return tarjetas;
            }

            /**
             * @brief Actualiza el saldo de la tarjeta por el tipo de moneda y UID de la tarjeta
             * 
             * @param uid 
             * @param idTarjeta 
             * @param idTipoMoneda 
             * @param monto 
             * @return true 
             * @return false 
             */
            static bool ActualizaSaldoByIdTarjetaAndIdTipoMoneda(string uid,  int idTipoMoneda, double monto)
            {
                BaseDatos::DBConnectPostgres oPostgres;
                PGresult *resultado = nullptr;
                bool operacionExitosa = false;
                
                try{
                    resultado = oPostgres.ExecFunction(
                        "SELECT * FROM sp.\"mvtarjetassaldos\"("
                        "'ASTTM','"+uid+"', 0,"
                        "CAST("+to_string(idTipoMoneda)+" AS SMALLINT),"
                        +to_string(monto)+")"
                    );
                    if (resultado != nullptr) {
                        int rows = PQntuples(resultado);
                        if (rows > 0) {
                            operacionExitosa = true;
                        }
                    }
                }
                catch(const std::exception& e){
                    std::cout << "Error en la consulta de tarjeta." << std::endl;
                    Seguridad::cLog::ErrorSistema("Transacciones/mv_tarjetas.h","ActualizaSaldoByIdTarjetaAndIdTipoMoneda", e.what());
                    operacionExitosa = false;
                }
                return operacionExitosa;
            }
    };

}
#endif