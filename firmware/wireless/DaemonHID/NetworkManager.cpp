#include "NetworkManager.h"
#include "AppConfig.h"
#include <WiFiS3.h>
#include <string.h>

namespace {
    constexpr uint8_t AP_START_ATTEMPTS = 2;
    constexpr unsigned long AP_IP_WAIT_MS = 4000;
    constexpr unsigned long AP_IP_POLL_MS = 250;
    constexpr unsigned long WIFI_RESET_WAIT_MS = 1500;

    bool isAccessPointActive(uint8_t status) {
        return status == WL_AP_LISTENING || status == WL_AP_CONNECTED;
    }

    bool isValidIPAddress(const IPAddress& address) {
        return address[0] != 0 || address[1] != 0 ||
               address[2] != 0 || address[3] != 0;
    }

    IPAddress waitForSoftAPIP() {
        const unsigned long startedAt = millis();

        while (millis() - startedAt < AP_IP_WAIT_MS) {
            const IPAddress address = WiFi.softAPIP();

            if (isValidIPAddress(address)) {
                return address;
            }

            delay(AP_IP_POLL_MS);
        }

        return IPAddress(0, 0, 0, 0);
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

    void printAPConfiguration(const IPAddress& address) {
        SerialUSB.println("[NETWORK] Punto de acceso iniciado correctamente.");
        SerialUSB.print("[NETWORK] SSID: ");
        SerialUSB.println(AppConfig::AP_SSID);
        SerialUSB.print("[NETWORK] Canal: ");
        SerialUSB.println(AppConfig::WIFI_CHANNEL);
        SerialUSB.print("[NETWORK] IP del SoftAP: ");
        SerialUSB.println(address);
    }
}

bool NetworkManager::beginAccessPoint() {
    // La clave se mantiene en AppConfig.h; no imprimirla en el monitor serial.
    const size_t passwordLength = strlen(AppConfig::AP_PASSWORD);

    SerialUSB.println("[NETWORK] Comprobando modulo Wi-Fi...");
    const char* firmware = WiFi.firmwareVersion();
    SerialUSB.print("[NETWORK] Firmware ESP32-S3: ");
    SerialUSB.println(firmware);

    if (strcmp(firmware, "99.99.99") == 0) {
        SerialUSB.println("[NETWORK] ADVERTENCIA: no se pudo consultar la version del firmware Wi-Fi.");
    }

    uint8_t currentStatus = WiFi.status();
    printAccessPointStatus(currentStatus);

    // Un AP solo se considera listo si esta activo Y tiene una IP valida.
    if (isAccessPointActive(currentStatus)) {
        const IPAddress currentIP = waitForSoftAPIP();

        if (isValidIPAddress(currentIP)) {
            SerialUSB.println("[NETWORK] El punto de acceso ya estaba activo.");
            printAPConfiguration(currentIP);
            return true;
        }

        SerialUSB.println("[NETWORK] El AP figura activo, pero softAPIP() sigue devolviendo 0.0.0.0.");
        SerialUSB.println("[NETWORK] Se reiniciara el modulo Wi-Fi antes de reintentar.");
        WiFi.end();
        delay(WIFI_RESET_WAIT_MS);
    }

    if (passwordLength < 8 || passwordLength > 63) {
        SerialUSB.println("[NETWORK] ERROR: la clave WPA debe tener entre 8 y 63 caracteres.");
        return false;
    }

    for (uint8_t attempt = 1; attempt <= AP_START_ATTEMPTS; ++attempt) {
        SerialUSB.println();
        SerialUSB.print("[NETWORK] Intento de inicio AP ");
        SerialUSB.print(attempt);
        SerialUSB.print("/");
        SerialUSB.println(AP_START_ATTEMPTS);

        SerialUSB.print("[NETWORK] SSID configurado: ");
        SerialUSB.println(AppConfig::AP_SSID);
        SerialUSB.print("[NETWORK] Canal configurado: ");
        SerialUSB.println(AppConfig::WIFI_CHANNEL);

        const uint8_t startStatus = WiFi.beginAP(
            AppConfig::AP_SSID,
            AppConfig::AP_PASSWORD,
            AppConfig::WIFI_CHANNEL
        );

        if (startStatus != WL_AP_LISTENING) {
            SerialUSB.println("[NETWORK] beginAP() no devolvio WL_AP_LISTENING.");
            printAccessPointStatus(startStatus);
        } else {
            // Dar tiempo al firmware del modulo para configurar el SoftAP.
            delay(1000);

            currentStatus = WiFi.status();
            printAccessPointStatus(currentStatus);

            if (isAccessPointActive(currentStatus)) {
                const IPAddress apAddress = waitForSoftAPIP();

                if (isValidIPAddress(apAddress)) {
                    printAPConfiguration(apAddress);
                    return true;
                }

                SerialUSB.println("[NETWORK] El AP esta activo, pero no se pudo obtener una IP valida.");
            } else {
                SerialUSB.println("[NETWORK] El AP no permanecio activo despues de beginAP().");
            }
        }

        if (attempt < AP_START_ATTEMPTS) {
            SerialUSB.println("[NETWORK] Reiniciando modulo Wi-Fi para el siguiente intento...");
            WiFi.end();
            delay(WIFI_RESET_WAIT_MS);
        }
    }

    SerialUSB.println("[NETWORK] ERROR: no se pudo levantar un AP con direccion IP valida.");
    SerialUSB.print("[NETWORK] Estado final: ");
    SerialUSB.println(WiFi.status());
    SerialUSB.print("[NETWORK] IP final del SoftAP: ");
    SerialUSB.println(WiFi.softAPIP());

    return false;
}
