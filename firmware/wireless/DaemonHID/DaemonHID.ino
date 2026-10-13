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
    SerialUSB.println("Commands: H = fixed text test, P = CMD ping demo, ? = help.");
    SerialUSB.println("For H, focus a plain-text editor before the five-second countdown.");
    SerialUSB.println("For P, use your own unlocked test PC and save open work first.");

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
                SerialUSB.println("Text test scheduled. Switch to your editor within 5 seconds.");
            } else {
                SerialUSB.println("HID unavailable or another test is pending.");
            }
        } else if (command == 'p' || command == 'P') {
            if (hidManager.requestPingDemo()) {
                SerialUSB.println("Ping demo scheduled. Focus your test desktop now.");
            } else {
                SerialUSB.println("HID unavailable or another test is pending.");
            }
        } else if (command == '?') {
            SerialUSB.println("DaemonHID commands:");
            SerialUSB.println("H - Fixed text test");
            SerialUSB.println("P - Open CMD and run ping 8.8.8.8");
            SerialUSB.println("? - Show this help");
        }
    }
}
