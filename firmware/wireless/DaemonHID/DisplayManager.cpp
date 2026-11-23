#include "DisplayManager.h"
#include "AppConfig.h"
#include "StorageManager.h"
#include "HidManager.h"
#include <WiFiS3.h>

#if defined(ARDUINO_ARCH_RENESAS)
extern "C" char* sbrk(int incr);
static int getFreeRamDisplay() {
    char top;
    return &top - reinterpret_cast<char*>(sbrk(0));
}
#else
static int getFreeRamDisplay() {
    return 0;
}
#endif

namespace {
    constexpr uint16_t COLOR_HEADER_BG = 0x0825; // Dark Slate Blue
    constexpr uint16_t COLOR_HIGHLIGHT_BG = 0xFFFF; // Blanco
    constexpr uint16_t COLOR_HIGHLIGHT_TXT = 0x0000; // Negro
    constexpr uint16_t COLOR_ALERT_BOX = 0x2000; // Dark Red
}

DisplayManager::DisplayManager(uint8_t cs, uint8_t dc, uint8_t rst)
    : display(cs, dc, rst) {
}

bool DisplayManager::begin(StorageManager* storage, HidManager* hid) {
    storageMgr = storage;
    hidMgr = hid;

    // Inicializa la pantalla TFT ST7735 de 1.44 pulgadas.
    display.initR(INITR_144GREENTAB);
    display.setRotation(0);
    display.fillScreen(ST7735_BLACK);
    display.setTextWrap(false);

    initialized = true;
    needsFullRedraw = true;

    drawHeader();
    drawFooter();
    drawMainMenu();

    return true;
}

void DisplayManager::setManagers(StorageManager* storage, HidManager* hid) {
    storageMgr = storage;
    hidMgr = hid;
}

void DisplayManager::clear(uint16_t color) {
    if (!initialized) return;
    display.fillScreen(color);
}

void DisplayManager::showMessage(
    const char* message,
    uint16_t color,
    uint8_t textSize
) {
    if (!initialized || message == nullptr) return;
    display.fillScreen(ST7735_BLACK);
    display.setCursor(4, 4);
    display.setTextColor(color);
    display.setTextSize(textSize);
    display.setTextWrap(true);
    display.print(message);
}

void DisplayManager::drawHeader() {
    // Barra superior
    display.fillRect(0, 0, 128, 14, COLOR_HEADER_BG);
    display.setTextColor(ST7735_WHITE, COLOR_HEADER_BG);
    display.setTextSize(1);
    display.setCursor(4, 3);
    display.print("DaemonHID");

    // Indicador AP / SD
    display.setCursor(96, 3);
    if (storageMgr && storageMgr->isReady()) {
        display.setTextColor(ST7735_GREEN, COLOR_HEADER_BG);
        display.print("[SD]");
    } else {
        display.setTextColor(ST7735_RED, COLOR_HEADER_BG);
        display.print("[--]");
    }

    display.drawFastHLine(0, 15, 128, ST7735_CYAN);
}

void DisplayManager::drawFooter() {
    display.drawFastHLine(0, 116, 128, 0x39E7);
    display.fillRect(0, 117, 128, 11, ST7735_BLACK);
    display.setTextSize(1);
    display.setTextColor(ST7735_YELLOW, ST7735_BLACK);
    display.setCursor(4, 119);

    if (currentScreen == ScreenState::ArmedAlert) {
        display.setTextColor(ST7735_RED, ST7735_BLACK);
        display.print("[SW] CANCELAR");
    } else if (currentScreen == ScreenState::SystemInfo) {
        display.print("[SW] Volver");
    } else {
        display.print("[v^] Mover  [SW] OK");
    }
}

void DisplayManager::drawMainMenu() {
    static const char* const menuItems[] = {
        "1. Scripts SD",
        "2. Test Texto (H)",
        "3. Ping CMD (P)",
        "4. Estado Red",
        "5. Info Sistema"
    };

    display.setTextSize(1);

    for (int i = 0; i < 5; ++i) {
        const int y = 20 + i * 18;

        if (i == selectedMenuIndex) {
            display.fillRect(2, y, 124, 15, COLOR_HIGHLIGHT_BG);
            display.setTextColor(COLOR_HIGHLIGHT_TXT, COLOR_HIGHLIGHT_BG);
            display.setCursor(6, y + 4);
            display.print("> ");
            display.print(menuItems[i]);
        } else {
            display.fillRect(2, y, 124, 15, ST7735_BLACK);
            display.setTextColor(ST7735_WHITE, ST7735_BLACK);
            display.setCursor(6, y + 4);
            display.print("  ");
            display.print(menuItems[i]);
        }
    }
}

void DisplayManager::loadPayloadFiles() {
    payloadCount = 0;
    if (storageMgr && storageMgr->isReady()) {
        payloadCount = storageMgr->getPayloadFileList(payloadNames, MAX_PAYLOAD_FILES);
    }
}

void DisplayManager::drawPayloadList() {
    display.setTextSize(1);
    const int totalItems = payloadCount + 1; // El último siempre es [< Volver]

    if (payloadCount == 0) {
        display.fillRect(2, 20, 124, 90, ST7735_BLACK);
        display.setTextColor(ST7735_RED, ST7735_BLACK);
        display.setCursor(6, 26);
        display.print("Sin scripts en");
        display.setCursor(6, 38);
        display.print("/payloads/ (SD)");

        // Opcion [< Volver]
        display.fillRect(2, 60, 124, 15, COLOR_HIGHLIGHT_BG);
        display.setTextColor(COLOR_HIGHLIGHT_TXT, COLOR_HIGHLIGHT_BG);
        display.setCursor(6, 64);
        display.print("> [< Volver]");
        return;
    }

    // Listado con scroll si excede 5 elementos
    int startIdx = 0;
    if (selectedPayloadIndex >= 4) {
        startIdx = selectedPayloadIndex - 3;
    }
    if (startIdx + 4 > totalItems) {
        startIdx = max(0, totalItems - 4);
    }

    for (int row = 0; row < 4; ++row) {
        const int itemIdx = startIdx + row;
        const int y = 20 + row * 22;

        if (itemIdx < totalItems) {
            const bool isSelected = (itemIdx == selectedPayloadIndex);
            const uint16_t bg = isSelected ? COLOR_HIGHLIGHT_BG : ST7735_BLACK;
            const uint16_t fg = isSelected ? COLOR_HIGHLIGHT_TXT : ST7735_WHITE;

            display.fillRect(2, y, 124, 18, bg);
            display.setTextColor(fg, bg);
            display.setCursor(6, y + 5);

            if (isSelected) display.print("> ");
            else display.print("  ");

            if (itemIdx < payloadCount) {
                display.print(payloadNames[itemIdx]);
            } else {
                display.print("[< Volver]");
            }
        } else {
            display.fillRect(2, y, 124, 18, ST7735_BLACK);
        }
    }
}

void DisplayManager::drawSystemInfo() {
    display.fillRect(2, 18, 124, 96, ST7735_BLACK);
    display.setTextSize(1);
    display.setTextColor(ST7735_CYAN, ST7735_BLACK);

    display.setCursor(4, 22);
    display.print("SSID: ");
    display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    display.print(AppConfig::AP_SSID);

    display.setCursor(4, 38);
    display.setTextColor(ST7735_CYAN, ST7735_BLACK);
    display.print("IP: ");
    display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    display.print(WiFi.localIP().toString());

    display.setCursor(4, 54);
    display.setTextColor(ST7735_CYAN, ST7735_BLACK);
    display.print("MicroSD: ");
    if (storageMgr && storageMgr->isReady()) {
        display.setTextColor(ST7735_GREEN, ST7735_BLACK);
        display.print("ACTIVA");
    } else {
        display.setTextColor(ST7735_RED, ST7735_BLACK);
        display.print("NO LISTA");
    }

    display.setCursor(4, 70);
    display.setTextColor(ST7735_CYAN, ST7735_BLACK);
    display.print("RAM Libre: ");
    display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    display.print(getFreeRamDisplay() / 1024);
    display.print(" KB");

    display.setCursor(4, 86);
    display.setTextColor(ST7735_CYAN, ST7735_BLACK);
    display.print("Uptime: ");
    display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    display.print(millis() / 1000);
    display.print("s");
}

void DisplayManager::drawArmedAlert() {
    const int cd = (hidMgr) ? hidMgr->getCountdownRemaining() : 0;
    const char* state = (hidMgr) ? hidMgr->getStateString() : "";

    display.fillRect(8, 22, 112, 88, COLOR_ALERT_BOX);
    display.drawRect(8, 22, 112, 88, ST7735_RED);

    display.setTextSize(1);
    display.setTextColor(ST7735_YELLOW, COLOR_ALERT_BOX);
    display.setCursor(20, 28);

    if (strcmp(state, "escribiendo") == 0) {
        display.print("ESCRIBIENDO...");
    } else {
        display.print("! ARMADO (5s) !");
    }

    display.setTextSize(3);
    display.setTextColor(ST7735_WHITE, COLOR_ALERT_BOX);
    display.setCursor(52, 46);
    display.print(cd);

    display.setTextSize(1);
    display.setTextColor(ST7735_GREEN, COLOR_ALERT_BOX);
    display.setCursor(14, 84);

    const char* activeName = (hidMgr) ? hidMgr->getActivePayloadName() : "";
    if (activeName && activeName[0] != '\0') {
        display.print(activeName);
    } else {
        display.print("Demo Activa");
    }
}

void DisplayManager::handleNav(NavEvent event) {
    if (event == NavEvent::None) return;

    // En pantalla de alerta/countdown, cualquier clic cancela inmediatamente
    if (currentScreen == ScreenState::ArmedAlert) {
        if (event == NavEvent::Click) {
            if (hidMgr) hidMgr->abort();
            currentScreen = ScreenState::MainMenu;
            needsFullRedraw = true;
        }
        return;
    }

    // Pantalla de Menu Principal
    if (currentScreen == ScreenState::MainMenu) {
        if (event == NavEvent::Up) {
            selectedMenuIndex = (selectedMenuIndex > 0) ? selectedMenuIndex - 1 : 4;
            drawMainMenu();
        } else if (event == NavEvent::Down) {
            selectedMenuIndex = (selectedMenuIndex < 4) ? selectedMenuIndex + 1 : 0;
            drawMainMenu();
        } else if (event == NavEvent::Click) {
            switch (selectedMenuIndex) {
                case 0: // Scripts SD
                    loadPayloadFiles();
                    selectedPayloadIndex = 0;
                    currentScreen = ScreenState::PayloadList;
                    needsFullRedraw = true;
                    break;

                case 1: // Test Texto
                    if (hidMgr) hidMgr->requestDemoText();
                    currentScreen = ScreenState::ArmedAlert;
                    needsFullRedraw = true;
                    break;

                case 2: // Ping CMD
                    if (hidMgr) hidMgr->requestPingDemo();
                    currentScreen = ScreenState::ArmedAlert;
                    needsFullRedraw = true;
                    break;

                case 3: // Estado Red
                case 4: // Info Sistema
                    currentScreen = ScreenState::SystemInfo;
                    needsFullRedraw = true;
                    break;
            }
        }
        return;
    }

    // Pantalla de Payloads de la MicroSD
    if (currentScreen == ScreenState::PayloadList) {
        const int totalItems = payloadCount + 1;

        if (event == NavEvent::Up) {
            selectedPayloadIndex = (selectedPayloadIndex > 0) ? selectedPayloadIndex - 1 : totalItems - 1;
            drawPayloadList();
        } else if (event == NavEvent::Down) {
            selectedPayloadIndex = (selectedPayloadIndex < totalItems - 1) ? selectedPayloadIndex + 1 : 0;
            drawPayloadList();
        } else if (event == NavEvent::Left) {
            currentScreen = ScreenState::MainMenu;
            needsFullRedraw = true;
        } else if (event == NavEvent::Click) {
            if (selectedPayloadIndex == payloadCount || payloadCount == 0) {
                currentScreen = ScreenState::MainMenu;
                needsFullRedraw = true;
            } else {
                if (hidMgr) {
                    hidMgr->requestPayloadFile(payloadNames[selectedPayloadIndex]);
                }
                currentScreen = ScreenState::ArmedAlert;
                needsFullRedraw = true;
            }
        }
        return;
    }

    // Pantalla de Informacion del Sistema
    if (currentScreen == ScreenState::SystemInfo) {
        if (event == NavEvent::Click || event == NavEvent::Left) {
            currentScreen = ScreenState::MainMenu;
            needsFullRedraw = true;
        }
        return;
    }
}

void DisplayManager::update() {
    if (!initialized) return;

    // Detectar si el HID fue armado desde la Web o Serial mientras estabamos en otro menu
    if (hidMgr) {
        if (hidMgr->isPending() || hidMgr->isExecuting()) {
            if (currentScreen != ScreenState::ArmedAlert) {
                currentScreen = ScreenState::ArmedAlert;
                needsFullRedraw = true;
            }
        } else {
            if (currentScreen == ScreenState::ArmedAlert) {
                // Finalizo o se aborto la ejecucion
                currentScreen = ScreenState::MainMenu;
                needsFullRedraw = true;
            }
        }
    }

    // Actualizaciones periodicas segun pantalla
    if (currentScreen == ScreenState::ArmedAlert) {
        const int cd = (hidMgr) ? hidMgr->getCountdownRemaining() : 0;
        if (cd != lastCountdown) {
            lastCountdown = cd;
            drawArmedAlert();
        }
    } else if (currentScreen == ScreenState::SystemInfo) {
        const unsigned long sec = millis() / 1000;
        if (sec != lastUptimeSec) {
            lastUptimeSec = sec;
            drawSystemInfo();
        }
    }

    if (needsFullRedraw) {
        needsFullRedraw = false;
        display.fillScreen(ST7735_BLACK);
        drawHeader();
        drawFooter();

        switch (currentScreen) {
            case ScreenState::MainMenu:
                drawMainMenu();
                break;
            case ScreenState::PayloadList:
                drawPayloadList();
                break;
            case ScreenState::SystemInfo:
                drawSystemInfo();
                break;
            case ScreenState::ArmedAlert:
                drawArmedAlert();
                break;
        }
    }
}
