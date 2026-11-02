
#include "NetworkManager.h"
#include "AppConfig.h"
#include <WiFiS3.h>

bool NetworkManager::beginAccessPoint() {
    // Comprueba si el punto de acceso ya está activo.
    if (WiFi.status() == WL_AP_LISTENING) {
        SerialUSB.println("El punto de acceso ya esta activo.");
        return true;
    }

    // Inicia el punto de acceso con la configuracion establecida.
    const uint8_t status = WiFi.beginAP(
        AppConfig::AP_SSID,
        AppConfig::AP_PASSWORD,
        AppConfig::WIFI_CHANNEL
    );

    // Comprueba si el punto de acceso se inicio correctamente.
    if (status != WL_AP_LISTENING) {
        SerialUSB.print("Error al iniciar el punto de acceso. Estado: ");
        SerialUSB.println(status);
        return false;
    }

    SerialUSB.println("Punto de acceso iniciado correctamente.");
    SerialUSB.print("SSID: ");
    SerialUSB.println(AppConfig::AP_SSID);

    return true;
}
