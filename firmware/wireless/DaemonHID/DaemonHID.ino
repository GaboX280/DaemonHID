#include <WiFiS3.h>
#include "AppConfig.h"
#include "NetworkManager.h"
#include "HttpServer.h"
#include "HidManager.h"

NetworkManager networkManager;
HttpServer httpServer;
HidManager hidManager;

void setup() {
    // The UNO R4 WiFi USB HID stack uses SerialUSB for serial monitoring.
    SerialUSB.begin(115200);

    hidManager.begin();

    SerialUSB.println();
    SerialUSB.println("DaemonHID starting...");
    SerialUSB.print("Firmware version: ");
    SerialUSB.println(AppConfig::FIRMWARE_VERSION);
    SerialUSB.println("USB HID module initialized.");
    SerialUSB.println("For a safe test, focus a plain-text editor,");
    SerialUSB.println("then enter H in Serial Monitor and switch to the editor within 5 seconds.");

    SerialUSB.println("Starting Wi-Fi access point...");

    if (networkManager.beginAccessPoint()) {
        SerialUSB.println("Access point started successfully!");

        SerialUSB.print("Network name: ");
        SerialUSB.println(AppConfig::AP_SSID);

        SerialUSB.print("IP address: ");
        SerialUSB.println(WiFi.localIP());

        if (httpServer.begin()) {
            SerialUSB.println("HTTP server started successfully!");
        } else {
            SerialUSB.println("ERROR: Could not start HTTP server.");
        }
    } else {
        SerialUSB.println("ERROR: Could not start access point.");
    }
}

void loop() {
    httpServer.handleClient();
    hidManager.update();

    while (SerialUSB.available() > 0) {
        const char command = static_cast<char>(SerialUSB.read());

        if (command == 'h' || command == 'H') {
            if (hidManager.requestDemoText()) {
                SerialUSB.println("HID test scheduled. Switch to your text editor now.");
            } else {
                SerialUSB.println("HID test is unavailable or already pending.");
            }
        }
    }
}
