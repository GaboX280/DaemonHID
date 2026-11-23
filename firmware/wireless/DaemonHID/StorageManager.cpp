
#include "StorageManager.h"

#include <SD.h>
#include <SPI.h>

bool StorageManager::begin(uint8_t chipSelect) {
    // Evita inicializar nuevamente una tarjeta ya disponible.
    if (initialized) {
        return true;
    }

    // Inicializa la comunicacion con la tarjeta microSD.
    initialized = SD.begin(chipSelect);

    if (!initialized) {
        SerialUSB.println("Error al inicializar la tarjeta microSD.");
        return false;
    }

    SerialUSB.println("Tarjeta microSD inicializada correctamente.");

    return true;
}

bool StorageManager::isReady() const {
    // Devuelve el estado de inicializacion del almacenamiento.
    return initialized;
}

bool StorageManager::writeFile(const char* path, const char* content) {
    if (!initialized || path == nullptr || content == nullptr) {
        return false;
    }

    // Elimina el archivo anterior para crear uno nuevo.
    if (SD.exists(path) && !SD.remove(path)) {
        return false;
    }

    // Abre el archivo para escritura.
    File file = SD.open(path, FILE_WRITE);

    if (!file) {
        return false;
    }

    // Comprueba que se haya escrito todo el contenido.
    const size_t expected = strlen(content);
    const size_t written = file.print(content);

    file.close();

    return written == expected;
}

bool StorageManager::appendFile(const char* path, const char* content) {
    if (!initialized || path == nullptr || content == nullptr) {
        return false;
    }

    // Abre el archivo para agregar contenido al final.
    File file = SD.open(path, FILE_WRITE);

    if (!file) {
        return false;
    }

    const size_t expected = strlen(content);
    const size_t written = file.print(content);

    file.close();

    return written == expected;
}

bool StorageManager::readFile(const char* path, String& content) {
    if (!initialized || path == nullptr) {
        return false;
    }

    // Comprueba que el archivo exista.
    if (!SD.exists(path)) {
        return false;
    }

    // Abre el archivo en modo lectura.
    File file = SD.open(path, FILE_READ);

    if (!file) {
        return false;
    }

    content = "";

    // Lee el archivo mientras existan datos disponibles.
    while (file.available()) {
        content += static_cast<char>(file.read());
    }

    file.close();

    return true;
}

bool StorageManager::exists(const char* path) const {
    if (!initialized || path == nullptr) {
        return false;
    }

    return SD.exists(path);
}

bool StorageManager::deleteFile(const char* path) {
    if (!initialized || path == nullptr) {
        return false;
    }

    // Evita intentar eliminar archivos inexistentes.
    if (!SD.exists(path)) {
        return false;
    }

    return SD.remove(path);
}

File StorageManager::openFile(const char* path, uint8_t mode) {
    if (!initialized || path == nullptr) {
        return File();
    }
    return SD.open(path, mode);
}

bool StorageManager::streamFileToClient(const char* path, WiFiClient& client) {
    if (!initialized || path == nullptr) {
        return false;
    }

    File file = SD.open(path, FILE_READ);
    if (!file) {
        return false;
    }

    // Transmite en bloques fijos para optimizar la SRAM
    uint8_t buffer[64];
    while (file.available() && client.connected()) {
        const int bytesRead = file.read(buffer, sizeof(buffer));
        if (bytesRead > 0) {
            client.write(buffer, bytesRead);
        }
    }

    file.close();
    return true;
}

String StorageManager::getPayloadsAsJson(const char* dirPath) {
    if (!initialized || dirPath == nullptr) {
        return "[]";
    }

    File dir = SD.open(dirPath);
    if (!dir || !dir.isDirectory()) {
        if (dir) dir.close();
        return "[]";
    }

    String json = "[";
    bool first = true;

    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }

        if (!entry.isDirectory()) {
            const char* name = entry.name();
            // Ignorar archivos del sistema y temporales
            if (name[0] != '.' && name[0] != '_') {
                if (!first) {
                    json += ",";
                }
                json += "\"";
                json += name;
                json += "\"";
                first = false;
            }
        }
        entry.close();
    }

    dir.close();
    json += "]";
    return json;
}

const char* StorageManager::getMimeType(const char* path) {
    if (path == nullptr) {
        return "text/plain; charset=utf-8";
    }

    String p = String(path);
    p.toLowerCase();

    if (p.endsWith(".html") || p.endsWith(".htm")) return "text/html; charset=utf-8";
    if (p.endsWith(".css")) return "text/css; charset=utf-8";
    if (p.endsWith(".js")) return "application/javascript; charset=utf-8";
    if (p.endsWith(".json")) return "application/json; charset=utf-8";
    if (p.endsWith(".txt")) return "text/plain; charset=utf-8";
    if (p.endsWith(".ico")) return "image/x-icon";
    if (p.endsWith(".png")) return "image/png";
    if (p.endsWith(".svg")) return "image/svg+xml";

    return "text/plain; charset=utf-8";
}
