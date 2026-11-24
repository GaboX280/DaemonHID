
#ifndef DAEMONHID_STORAGE_MANAGER_H
#define DAEMONHID_STORAGE_MANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <WiFiS3.h>

/**
 * @brief Gestiona las operaciones de almacenamiento en microSD.
 *
 * Permite inicializar la tarjeta, listar scripts y transmitir
 * archivos estaticos directamente hacia clientes web sin saturar la RAM.
 */
class StorageManager {
public:
    /**
     * @brief Inicializa la tarjeta microSD.
     * @param chipSelect Pin utilizado para seleccionar la tarjeta.
     * @return true si la inicializacion fue exitosa.
     */
    bool begin(uint8_t chipSelect);

    /**
     * @brief Comprueba si la tarjeta esta disponible.
     */
    bool isReady() const;

    /**
     * @brief Crea o sobrescribe un archivo.
     */
    bool writeFile(const char* path, const char* content);

    /**
     * @brief Agrega contenido al final de un archivo.
     */
    bool appendFile(const char* path, const char* content);

    /**
     * @brief Lee el contenido de un archivo a un String (archivos pequenos).
     */
    bool readFile(const char* path, String& content);

    /**
     * @brief Comprueba si un archivo existe.
     */
    bool exists(const char* path) const;

    /**
     * @brief Elimina un archivo de la tarjeta.
     */
    bool deleteFile(const char* path);

    /**
     * @brief Transmite un archivo por bloques directamente a un cliente HTTP.
     * Evita la fragmentacion de memoria RAM.
     */
    bool streamFileToClient(const char* path, WiFiClient& client);

    /**
     * @brief Transmite un archivo previamente abierto por bloques.
     * @param file Archivo abierto en modo lectura.
     * @param client Cliente HTTP de destino.
     * @return true si se completo la transferencia mientras el cliente seguia conectado.
     */
    bool streamFileToClient(File& file, WiFiClient& client);

    /**
     * @brief Obtiene una lista en formato JSON de los archivos en un directorio.
     * @param dirPath Directorio a escanear (por defecto /payloads).
     * @return Cadena JSON con el arreglo de nombres de archivos.
     */
    String getPayloadsAsJson(const char* dirPath = "/payloads");

    /**
     * @brief Llena un arreglo estático con nombres de archivos de /payloads.
     * @param filenames Arreglo destino de cadenas de texto.
     * @param maxFiles Cantidad máxima de archivos a registrar.
     * @return Cantidad de archivos encontrados.
     */
    int getPayloadFileList(char filenames[][16], int maxFiles);

    /**
     * @brief Abre un archivo de la SD.
     */
    File openFile(const char* path, uint8_t mode = FILE_READ);

    /**
     * @brief Determina el tipo MIME segun la extension del archivo.
     */
    static const char* getMimeType(const char* path);

private:
    bool initialized = false;
};

#endif
