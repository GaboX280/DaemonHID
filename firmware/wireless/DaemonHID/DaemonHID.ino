
#include <WiFiS3.h>
#include "AppConfig.h"
#include "NetworkManager.h"
#include "HttpServer.h"

NetworkManager networkManager;
HttpServer httpServer;

void setup() {
    Serial.begin(115200);

    Serial.println();
    Serial.println("DaemonHID starting...");
    Serial.print("Firmware version: ");
    Serial.println(AppConfig::FIRMWARE_VERSION);

    Serial.println("Starting Wi-Fi access point...");

    if (networkManager.beginAccessPoint()) {
        Serial.println("Access point started successfully!");

        Serial.print("Network name: ");
        Serial.println(AppConfig::AP_SSID);

        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        if (httpServer.begin()) {
            Serial.println("HTTP server started successfully!");
        } else {
            Serial.println("ERROR: Could not start HTTP server.");
        }
    } else {
        Serial.println("ERROR: Could not start access point.");
    }
}

void loop() {
    httpServer.handleClient();
}
