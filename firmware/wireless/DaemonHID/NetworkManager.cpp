#include "NetworkManager.h"
#include "AppConfig.h"
#include <WiFiS3.h>
#include <string.h>

namespace {
    bool isAccessPointActive(uint8_t status) {
        return status == WL_AP_LISTENING || status == WL_AP_CONNECTED;
    }

    void printAccessPointStatus(uint8_t status) {
        SerialUSB.print("[NETWORK] Estado Wi-Fi: ");
        SerialUSB.println(status);

        switch (status) {
            case WL_AP_LISTENING:
                SerialUSB.println("[NETWORK] El AP esta escuchando conexiones.");
                break;

            case WL_AP_CONNECTED:
                SerialUSB.println("[NETWORK] AP activo con al menos un cliente conectado.");
                break;

            case WL_AP_FAILED:
                SerialUSB.println("[NETWORK] El firmware Wi-Fi rechazo el inicio del AP.");
                break;

            default:
                SerialUSB.println("[NETWORK] Estado distinto al esperado para un punto de acceso.");
                break;
        }
    }
}

bool NetworkManager::beginAccessPoint() {
    // La clave se mantiene en AppConfig.h; no imprimirla en el monitor serial.
    const size_t passwordLength = strlen(AppConfig::AP_PASSWORD);

    SerialUSB.println("[NETWORK] Comprobando modulo Wi-Fi...");
    SerialUSB.print("[NETWORK] Firmware ESP32-S3: ");
    SerialUSB.println(WiFi.firmwareVersion());

    uint8_t currentStatus = WiFi.status();
    printAccessPointStatus(currentStatus);

    // WL_AP_CONNECTED tambien significa que el SoftAP ya esta activo.
    if (isAccessPointActive(currentStatus)) {
        SerialUSB.println("[NETWORK] El punto de acceso ya esta activo.");
        SerialUSB.print("[NETWORK] SSID: ");
        SerialUSB.println(AppConfig::AP_SSID);
        return true;
    }

    if (passwordLength < 8 || passwordLength > 63) {
        SerialUSB.println("[NETWORK] ERROR: la clave WPA debe tener entre 8 y 63 caracteres.");
        return false;
    }

    SerialUSB.println("[NETWORK] Iniciando punto de acceso...");
    SerialUSB.print("[NETWORK] SSID configurado: ");
    SerialUSB.println(AppConfig::AP_SSID);
    SerialUSB.print("[NETWORK] Canal configurado: ");
    SerialUSB.println(AppConfig::WIFI_CHANNEL);

    uint8_t startStatus = WiFi.beginAP(
        AppConfig::AP_SSID,
        AppConfig::AP_PASSWORD,
        AppConfig::WIFI_CHANNEL
    );

    if (startStatus != WL_AP_LISTENING) {
        SerialUSB.println("[NETWORK] Primer intento fallido.");
        printAccessPointStatus(startStatus);

        // Recuperar el modulo por si quedo en un estado Wi-Fi anterior.
        SerialUSB.println("[NETWORK] Reiniciando modulo Wi-Fi para reintentar...");
        WiFi.end();
        delay(700);

        startStatus = WiFi.beginAP(
            AppConfig::AP_SSID,
            AppConfig::AP_PASSWORD,
            AppConfig::WIFI_CHANNEL
        );
    }

    if (startStatus != WL_AP_LISTENING) {
        SerialUSB.println("[NETWORK] ERROR: no se pudo iniciar el punto de acceso.");
        printAccessPointStatus(startStatus);
        printAccessPointStatus(WiFi.status());
        return false;
    }

    // Dar tiempo al firmware Wi-Fi para anunciar el SSID y preparar el SoftAP.
    delay(1200);

    currentStatus = WiFi.status();
    if (!isAccessPointActive(currentStatus)) {
        SerialUSB.println("[NETWORK] El inicio devolvio exito, pero el AP no permanecio activo.");
        printAccessPointStatus(currentStatus);
        return false;
    }

    SerialUSB.println("[NETWORK] Punto de acceso iniciado correctamente.");
    SerialUSB.print("[NETWORK] SSID: ");
    SerialUSB.println(AppConfig::AP_SSID);
    SerialUSB.print("[NETWORK] Direccion IP: ");
    SerialUSB.println(WiFi.softAPIP());

    return true;
}
