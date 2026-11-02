
#include <WiFiS3.h>

#include "AppConfig.h"
#include "NetworkManager.h"
#include "HttpServer.h"
#include "HidManager.h"
#include "StorageManager.h"
#include "DisplayManager.h"
#include "JoystickManager.h"

// =====================================================
// CONFIGURACION DE MODULOS
// =====================================================

// Activar cuando el hardware correspondiente este conectado
// y se hayan confirmado sus conexiones fisicas.
constexpr bool ENABLE_DISPLAY = false;
constexpr bool ENABLE_STORAGE = false;
constexpr bool ENABLE_JOYSTICK = false;

// Pines provisionales: verificar antes de activar los modulos.
constexpr uint8_t TFT_CS_PIN = 9;
constexpr uint8_t TFT_DC_PIN = 8;
constexpr uint8_t TFT_RST_PIN = 7;

constexpr uint8_t SD_CS_PIN = 10;

constexpr uint8_t JOYSTICK_X_PIN = A0;
constexpr uint8_t JOYSTICK_Y_PIN = A1;
constexpr uint8_t JOYSTICK_BUTTON_PIN = 2;

// =====================================================
// INSTANCIAS DE LOS MODULOS
// =====================================================

NetworkManager networkManager;
HttpServer httpServer;
HidManager hidManager;

StorageManager storageManager;
DisplayManager displayManager(
    TFT_CS_PIN,
    TFT_DC_PIN,
    TFT_RST_PIN
);

JoystickManager joystickManager(
    JOYSTICK_X_PIN,
    JOYSTICK_Y_PIN,
    JOYSTICK_BUTTON_PIN
);

// Estado de inicializacion de los modulos.
bool httpServerReady = false;
bool displayReady = false;
bool storageReady = false;
bool joystickReady = false;

// =====================================================
// CONFIGURACION INICIAL
// =====================================================

void setup() {
    SerialUSB.begin(115200);

    SerialUSB.println();
    SerialUSB.println("================================");
    SerialUSB.println("       INICIANDO DAEMONHID");
    SerialUSB.println("================================");

    SerialUSB.print("Version del firmware: ");
    SerialUSB.println(AppConfig::FIRMWARE_VERSION);

    // -------------------------------------------------
    // USB HID
    // -------------------------------------------------

    SerialUSB.println();
    SerialUSB.println("[HID] Inicializando interfaz USB HID...");

    hidManager.begin();

    SerialUSB.println("[HID] Interfaz inicializada.");

    // -------------------------------------------------
    // PANTALLA TFT
    // -------------------------------------------------

    SerialUSB.println();
    SerialUSB.println("[DISPLAY] Inicializando pantalla TFT...");

    if (ENABLE_DISPLAY) {
        displayReady = displayManager.begin();

        if (displayReady) {
            displayManager.showMessage(
                "DaemonHID\nIniciando...",
                ST7735_WHITE
            );
            SerialUSB.println("[DISPLAY] Inicializacion completada.");
        } else {
            SerialUSB.println("[DISPLAY] No se pudo inicializar.");
        }
    } else {
        SerialUSB.println("[DISPLAY] Modulo desactivado.");
    }

    // -------------------------------------------------
    // ALMACENAMIENTO MICROSD
    // -------------------------------------------------

    SerialUSB.println();
    SerialUSB.println("[STORAGE] Inicializando microSD...");

    if (ENABLE_STORAGE) {
        storageReady = storageManager.begin(SD_CS_PIN);

        if (storageReady) {
            SerialUSB.println("[STORAGE] Tarjeta disponible.");
        } else {
            SerialUSB.println("[STORAGE] Error de inicializacion.");
        }
    } else {
        SerialUSB.println("[STORAGE] Modulo desactivado.");
    }

    // -------------------------------------------------
    // JOYSTICK
    // -------------------------------------------------

    SerialUSB.println();
    SerialUSB.println("[JOYSTICK] Inicializando joystick...");

    if (ENABLE_JOYSTICK) {
        joystickManager.begin();
        joystickReady = true;
        SerialUSB.println("[JOYSTICK] Modulo inicializado.");
    } else {
        SerialUSB.println("[JOYSTICK] Modulo desactivado.");
    }

    // -------------------------------------------------
    // DEMOSTRACIONES HID
    // -------------------------------------------------

    SerialUSB.println();
    SerialUSB.println("Demostraciones disponibles:");
    SerialUSB.println("H - Prueba de escritura de texto.");
    SerialUSB.println("P - Demostracion de conectividad mediante CMD.");
    SerialUSB.println("? - Mostrar ayuda.");

    SerialUSB.println();
    SerialUSB.println("Para H, selecciona un editor de texto antes");
    SerialUSB.println("de que finalice la cuenta regresiva de 5 segundos.");

    SerialUSB.println("Para P, utiliza un equipo de pruebas desbloqueado");
    SerialUSB.println("y guarda cualquier trabajo antes de ejecutarla.");

    // -------------------------------------------------
    // RED INALAMBRICA
    // -------------------------------------------------

    SerialUSB.println();
    SerialUSB.println("[NETWORK] Iniciando punto de acceso Wi-Fi...");

    if (!networkManager.beginAccessPoint()) {
        SerialUSB.println("[NETWORK] ERROR: No se pudo iniciar el AP.");
        SerialUSB.println("[HTTP] Servidor HTTP no disponible.");
        SerialUSB.println("================================");
        return;
    }

    SerialUSB.println("[NETWORK] Punto de acceso iniciado.");
    SerialUSB.print("[NETWORK] SSID: ");
    SerialUSB.println(AppConfig::AP_SSID);

    SerialUSB.print("[NETWORK] Direccion IP: ");
    SerialUSB.println(WiFi.localIP());

    // -------------------------------------------------
    // SERVIDOR HTTP
    // -------------------------------------------------

    SerialUSB.println();
    SerialUSB.println("[HTTP] Iniciando servidor web...");

    if (httpServer.begin()) {
        httpServerReady = true;
        SerialUSB.println("[HTTP] Servidor iniciado correctamente.");
    } else {
        SerialUSB.println("[HTTP] ERROR al iniciar el servidor.");
    }

    // -------------------------------------------------
    // FINALIZACION
    // -------------------------------------------------

    SerialUSB.println();
    SerialUSB.println("================================");
    SerialUSB.println("       DAEMONHID ESTA LISTO");
    SerialUSB.println("================================");
}

// =====================================================
// CICLO PRINCIPAL
// =====================================================

void loop() {

    // Atiende las solicitudes del servidor HTTP.
    if (httpServerReady) {
        httpServer.handleClient();
    }

    // Actualiza las demostraciones USB HID.
    hidManager.update();

    // Actualiza el joystick y procesa sus eventos.
    if (joystickReady) {
        joystickManager.update();

        if (joystickManager.wasButtonPressed()) {
            SerialUSB.println("[JOYSTICK] Boton presionado.");
        }

        const JoystickDirection direction =
            joystickManager.getDirection();

        // La lectura de direccion queda disponible
        // para integrar el menu de navegacion.
        (void)direction;
    }

    // Procesa los comandos recibidos por USB serial.
    while (SerialUSB.available() > 0) {

        const char comando =
            static_cast<char>(SerialUSB.read());

        // Ignora saltos de linea.
        if (comando == '\n' || comando == '\r') {
            continue;
        }

        switch (comando) {

            // -----------------------------------------
            // PRUEBA DE TEXTO HID
            // -----------------------------------------

            case 'h':
            case 'H':

                if (hidManager.requestDemoText()) {
                    SerialUSB.println(
                        "[HID] Prueba de texto programada."
                    );

                    SerialUSB.println(
                        "Selecciona tu editor durante los proximos 5 segundos."
                    );
                } else {
                    SerialUSB.println(
                        "[HID] No disponible o existe otra demostracion pendiente."
                    );
                }

                break;

            // -----------------------------------------
            // DEMOSTRACION DE PING
            // -----------------------------------------

            case 'p':
            case 'P':

                if (hidManager.requestPingDemo()) {
                    SerialUSB.println(
                        "[HID] Demostracion de ping programada."
                    );

                    SerialUSB.println(
                        "Prepara tu equipo de pruebas."
                    );
                } else {
                    SerialUSB.println(
                        "[HID] No disponible o existe otra demostracion pendiente."
                    );
                }

                break;

            // -----------------------------------------
            // AYUDA
            // -----------------------------------------

            case '?':

                SerialUSB.println();
                SerialUSB.println("========== AYUDA DAEMONHID ==========");
                SerialUSB.println("H - Ejecutar prueba de escritura de texto.");
                SerialUSB.println("P - Abrir CMD y ejecutar ping 8.8.8.8.");
                SerialUSB.println("? - Mostrar esta ayuda.");
                SerialUSB.println("=====================================");

                break;

            // -----------------------------------------
            // COMANDO NO RECONOCIDO
            // -----------------------------------------

            default:

                SerialUSB.println(
                    "[ERROR] Comando no reconocido. Escribe ? para ver la ayuda."
                );

                break;
        }
    }
}
