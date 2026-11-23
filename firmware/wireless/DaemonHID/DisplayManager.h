#ifndef DAEMONHID_DISPLAY_MANAGER_H
#define DAEMONHID_DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "JoystickManager.h"

class StorageManager;
class HidManager;

enum class ScreenState {
    MainMenu,
    PayloadList,
    SystemInfo,
    ArmedAlert
};

/**
 * @brief Gestiona la interfaz grafica en la pantalla TFT ST7735 de 1.44".
 *
 * Proporciona un sistema de menus interactivos con navegacion por joystick,
 * telemetria en tiempo real, selector de payloads de la MicroSD y alertas antiparpadeo.
 */
class DisplayManager {
public:
    DisplayManager(uint8_t cs, uint8_t dc, uint8_t rst);

    bool begin(StorageManager* storage = nullptr, HidManager* hid = nullptr);
    void setManagers(StorageManager* storage, HidManager* hid);

    void clear(uint16_t color = ST7735_BLACK);
    void showMessage(
        const char* message,
        uint16_t color = ST7735_WHITE,
        uint8_t textSize = 1
    );

    void handleNav(NavEvent event);
    void update();

private:
    Adafruit_ST7735 display;
    StorageManager* storageMgr = nullptr;
    HidManager* hidMgr = nullptr;

    bool initialized = false;

    ScreenState currentScreen = ScreenState::MainMenu;
    int selectedMenuIndex = 0;
    int selectedPayloadIndex = 0;

    static constexpr int MAX_PAYLOAD_FILES = 8;
    char payloadNames[MAX_PAYLOAD_FILES][16];
    int payloadCount = 0;

    int lastCountdown = -1;
    unsigned long lastUptimeSec = 0;
    bool needsFullRedraw = true;

    void drawHeader();
    void drawFooter();
    void drawMainMenu();
    void drawPayloadList();
    void drawSystemInfo();
    void drawArmedAlert();

    void loadPayloadFiles();
};

#endif
