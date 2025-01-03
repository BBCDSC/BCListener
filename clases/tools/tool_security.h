#ifndef toolsecurity_h
#define toolsecurity_h

/**
 * @file tool_security.h
 * @author LuisLule
 * @brief Clase que encripta y desencripta cadenas
 * @version 0.1
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "tool_log.h"

/**
 * @brief namespace Seguridad que agrupa las clases orientadas a la seguridad
 * 
 */
namespace Seguridad{

    /**
     * @brief Clase de encriptamiento Aes3
     * 
     */
    class Aes3
    {

        private:
            // Función para convertir bytes en hexadecimal
            std::string toHex(const std::vector<unsigned char> &encryptedMessage)
            {
                std::ostringstream oss;
                for (unsigned char c : encryptedMessage)
                {
                    oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
                }

                return oss.str();
            }

            // Función para codificar en Base64
            std::string toBase64(const std::vector<unsigned char> &data)
            {
                BIO *b64 = BIO_new(BIO_f_base64());
                BIO *bio = BIO_new(BIO_s_mem());
                b64 = BIO_push(b64, bio);

                BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Evita saltos de línea
                BIO_write(b64, data.data(), data.size());
                BIO_flush(b64);

                BUF_MEM *bufferPtr;
                BIO_get_mem_ptr(b64, &bufferPtr);

                std::string base64Data(bufferPtr->data, bufferPtr->length);
                BIO_free_all(b64);

                return base64Data;
            }

            // Función para decodificar desde Base64
            std::string fromBase64(const std::string &base64Data)
            {
                BIO *bio = BIO_new_mem_buf(base64Data.data(), base64Data.size());
                BIO *b64 = BIO_new(BIO_f_base64());
                bio = BIO_push(b64, bio);

                BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Evita saltos de línea

                char buffer[128];
                std::string decodedData;
                int length;
                while ((length = BIO_read(bio, buffer, sizeof(buffer))) > 0)
                {
                    decodedData.append(buffer, length);
                }
                BIO_free_all(bio);

                return decodedData;
            }
            // Función para agregar padding
            std::vector<unsigned char> applyPadding(const std::string &message)
            {
                size_t blockSize = AES_BLOCK_SIZE; // Tamaño del bloque AES (16 bytes)
                size_t paddingSize = blockSize - (message.size() % blockSize);
                std::vector<unsigned char> paddedMessage(message.begin(), message.end());
                paddedMessage.resize(message.size() + paddingSize, paddingSize);
                return paddedMessage;
            }

            // Función para eliminar padding después de desencriptar
            std::string removePadding(const std::vector<unsigned char> &paddedMessage)
            {
                size_t paddingSize = paddedMessage.back();
                return std::string(paddedMessage.begin(), paddedMessage.end() - paddingSize);
            }

        
            // Función para encriptar usando AES
            std::vector<unsigned char> encryptAES(const std::string &message, const unsigned char *key)
            {
                AES_KEY aesKey;
                AES_set_encrypt_key(key, 128, &aesKey);

                std::vector<unsigned char> paddedMessage = applyPadding(message);
                std::vector<unsigned char> encryptedMessage(paddedMessage.size());

                for (size_t i = 0; i < paddedMessage.size(); i += AES_BLOCK_SIZE)
                {
                    AES_encrypt(paddedMessage.data() + i, encryptedMessage.data() + i, &aesKey);
                }

                return encryptedMessage;
            }

            // Función para desencriptar usando AES
            std::string decryptAES(const std::vector<unsigned char> &encryptedMessage, const unsigned char *key)
            {
                AES_KEY aesKey;
                AES_set_decrypt_key(key, 128, &aesKey);

                std::vector<unsigned char> decryptedMessage(encryptedMessage.size());

                for (size_t i = 0; i < encryptedMessage.size(); i += AES_BLOCK_SIZE)
                {
                    AES_decrypt(encryptedMessage.data() + i, decryptedMessage.data() + i, &aesKey);
                }

                return removePadding(decryptedMessage);
            }

        public:

            /**
             * @brief Encripta una cadena ASCII.
             * 
             * @param cadena Cadena a encriptar
             * @return Cadena encriptada en HEX->B64
             */
            string Encriptar(string cadena)
            {
                string sreturn = "";
                try
                {
                    const unsigned char key[] = "BigConnectBBC#24"; // Clave de 128 bits (16 bytes)
                    // Encriptar el mensaje
                    std::vector<unsigned char> encryptedMessage = encryptAES(cadena, key);
                    sreturn = toBase64(encryptedMessage);
                  
                }
                catch(const std::exception& e)
                {
                    Seguridad::cLog::ErrorSistema("tool_security.h","Encriptar", e.what());
                    sreturn = "";
                }
                return sreturn.c_str();
            }

            /**
             * @brief Desencripta una cadena
             * 
             * @param cadenaB64 Cadena a desencriptar<
             * @return * Cadena en formato ASCII 
             */
            string Desencriptar(string cadenaB64)
            {
                string sreturn = "";
                try
                {
                    const unsigned char key[] = "BigConnectBBC#24"; // Clave de 128 bits (16 bytes)
                    // Encriptar el mensaje
                    std::string messageDecodeBase64 = fromBase64(cadenaB64);
                    std::vector<unsigned char> vectorMessage(messageDecodeBase64.begin(), messageDecodeBase64.end());
                    // std::cout << "Mensaje encriptado (Base64) ::: [" << messageDecodeBase64 << "]" << std::endl;
                    std::string decryptedMessage2 = decryptAES(vectorMessage, key);
                    sreturn = decryptedMessage2;
                    
                }
                catch(const std::exception& e)
                {
                    Seguridad::cLog::ErrorSistema("tool_security.h","Desencriptar", e.what());
                    sreturn = "";
                }
                return sreturn.c_str();                
            }

    };
}
#endif