
#ifndef DAEMONHID_STORAGE_MANAGER_H
#define DAEMONHID_STORAGE_MANAGER_H

#include <Arduino.h>

/**
 * @brief Gestiona las operaciones de almacenamiento en microSD.
 *
 * Permite inicializar la tarjeta y realizar operaciones
 * basicas de lectura, escritura y administracion de archivos.
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
     * @param path Ruta del archivo.
     * @param content Contenido que se almacenara.
     * @return true si la escritura fue exitosa.
     */
    bool writeFile(const char* path, const char* content);

    /**
     * @brief Agrega contenido al final de un archivo.
     */
    bool appendFile(const char* path, const char* content);

    /**
     * @brief Lee el contenido de un archivo.
     * @param path Ruta del archivo.
     * @param content Variable donde se guardara el resultado.
     * @return true si la lectura fue exitosa.
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

private:
    bool initialized = false;
};

#endif
