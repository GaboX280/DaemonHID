#include "NetworkManager.h"
#include "AppConfig.h"
#include <WiFiS3.h>

bool NetworkManager::beginAccessPoint() {
    const uint8_t status = WiFi.beginAP(
        AppConfig::AP_SSID,
        AppConfig::AP_PASSWORD,
        AppConfig::WIFI_CHANNEL
    );

    return status == WL_AP_LISTENING;
}
