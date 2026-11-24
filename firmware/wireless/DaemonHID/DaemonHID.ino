
#include <WiFiS3.h>
#include <SPI.h>

#include "AppConfig.h"
#include "HardwareConfig.h"
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
constexpr bool ENABLE_DISPLAY = true;
constexpr bool ENABLE_STORAGE = true;
constexpr bool ENABLE_JOYSTICK = true;

// Asignacion centralizada para Arduino UNO R4 WiFi.
constexpr uint8_t TFT_CS_PIN = HardwareConfig::TFT_CS_PIN;
constexpr uint8_t TFT_DC_PIN = HardwareConfig::TFT_DC_PIN;
constexpr uint8_t TFT_RST_PIN = HardwareConfig::TFT_RST_PIN;
constexpr uint8_t SD_CS_PIN = HardwareConfig::SD_CS_PIN;
constexpr uint8_t JOYSTICK_X_PIN = HardwareConfig::JOYSTICK_X_PIN;
constexpr uint8_t JOYSTICK_Y_PIN = HardwareConfig::JOYSTICK_Y_PIN;
constexpr uint8_t JOYSTICK_BUTTON_PIN = HardwareConfig::JOYSTICK_BUTTON_PIN;

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
bool hidReady = false;
bool spiBusReady = false;
bool httpServerReady = false;
bool networkReady = false;
bool displayReady = false;
bool storageReady = false;
bool joystickReady = false;

// =====================================================
// METODOS DE INICIALIZACION
// =====================================================

/**
 * @brief Prepara el bus SPI compartido por TFT y microSD.
 *
 * Ambos CS deben permanecer en HIGH antes de inicializar cualquier
 * dispositivo SPI para evitar que los dos modulos respondan a la vez.
 */
void initializeSharedSPIBus() {
    SerialUSB.println("[SPI] Preparando bus SPI compartido...");

    pinMode(TFT_CS_PIN, OUTPUT);
    digitalWrite(TFT_CS_PIN, HIGH);

    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    SPI.begin();
    spiBusReady = true;

    SerialUSB.println("[SPI] Bus listo (MOSI=D11, MISO=D12, SCK=D13).");
}

void initializeHIDModule() {
    SerialUSB.println();
    SerialUSB.println("[HID] Inicializando interfaz USB HID...");
    hidReady = hidManager.begin(&storageManager);

    SerialUSB.println(hidReady
        ? "[HID] Interfaz USB HID inicializada."
        : "[HID] No se pudo inicializar la interfaz.");
}

void initializeStorageModule() {
    SerialUSB.println();
    SerialUSB.println("[STORAGE] Inicializando microSD...");

    if (!ENABLE_STORAGE) {
        storageReady = false;
        SerialUSB.println("[STORAGE] Modulo desactivado.");
        return;
    }

    if (!spiBusReady) {
        SerialUSB.println("[STORAGE] Error: el bus SPI no esta listo.");
        return;
    }

    storageReady = storageManager.begin(SD_CS_PIN);
    SerialUSB.println(storageReady
        ? "[STORAGE] Tarjeta microSD disponible."
        : "[STORAGE] Error de inicializacion. Revise CS, alimentacion y formato.");
}

void initializeDisplayModule() {
    SerialUSB.println();
    SerialUSB.println("[DISPLAY] Inicializando TFT ST7735...");

    if (!ENABLE_DISPLAY) {
        displayReady = false;
        SerialUSB.println("[DISPLAY] Modulo desactivado.");
        return;
    }

    if (!spiBusReady) {
        SerialUSB.println("[DISPLAY] Error: el bus SPI no esta listo.");
        return;
    }

    displayReady = displayManager.begin(&storageManager, &hidManager);
    SerialUSB.println(displayReady
        ? "[DISPLAY] Secuencia de inicializacion TFT ejecutada."
        : "[DISPLAY] No se pudo configurar la TFT.");
}

void initializeJoystickModule() {
    SerialUSB.println();
    SerialUSB.println("[JOYSTICK] Inicializando joystick ELEGOO...");

    if (!ENABLE_JOYSTICK) {
        joystickReady = false;
        SerialUSB.println("[JOYSTICK] Modulo desactivado.");
        return;
    }

    joystickReady = joystickManager.begin();
    SerialUSB.println(joystickReady
        ? "[JOYSTICK] Pines y entradas configurados."
        : "[JOYSTICK] No se pudo configurar el modulo.");
}

void initializeNetworkModule() {
    SerialUSB.println();
    SerialUSB.println("[NETWORK] Iniciando punto de acceso Wi-Fi...");

    networkReady = networkManager.beginAccessPoint();
    if (!networkReady) {
        SerialUSB.println("[NETWORK] ERROR: No se pudo iniciar el AP.");
        return;
    }

    SerialUSB.println("[NETWORK] Punto de acceso iniciado.");
    SerialUSB.print("[NETWORK] SSID: ");
    SerialUSB.println(AppConfig::AP_SSID);
    SerialUSB.print("[NETWORK] Direccion IP: ");
    SerialUSB.println(WiFi.localIP());
}

void initializeHttpServerModule() {
    SerialUSB.println();
    SerialUSB.println("[HTTP] Iniciando servidor web...");

    if (!networkReady) {
        httpServerReady = false;
        SerialUSB.println("[HTTP] Omitido: el punto de acceso no esta disponible.");
        return;
    }

    httpServerReady = httpServer.begin(&storageManager, &hidManager);
    SerialUSB.println(httpServerReady
        ? "[HTTP] Servidor iniciado correctamente."
        : "[HTTP] ERROR al iniciar el servidor.");
}

void printInitializationSummary() {
    SerialUSB.println();
    SerialUSB.println("========== ESTADO DE DAEMONHID ==========");
    SerialUSB.print("[HID]      "); SerialUSB.println(hidReady ? "LISTO" : "NO DISPONIBLE");
    SerialUSB.print("[SPI]      "); SerialUSB.println(spiBusReady ? "LISTO" : "NO DISPONIBLE");
    SerialUSB.print("[MICROSD]  "); SerialUSB.println(storageReady ? "LISTA" : "NO DISPONIBLE");
    SerialUSB.print("[TFT]      "); SerialUSB.println(displayReady ? "CONFIGURADA" : "NO DISPONIBLE");
    SerialUSB.print("[JOYSTICK] "); SerialUSB.println(joystickReady ? "CONFIGURADO" : "NO DISPONIBLE");
    SerialUSB.print("[WIFI AP]  "); SerialUSB.println(networkReady ? "LISTO" : "NO DISPONIBLE");
    SerialUSB.print("[HTTP]     "); SerialUSB.println(httpServerReady ? "LISTO" : "NO DISPONIBLE");
    SerialUSB.println("=========================================");
}

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

    // Inicializar primero el bus y los perifericos que lo comparten.
    initializeSharedSPIBus();
    initializeStorageModule();
    initializeDisplayModule();
    initializeJoystickModule();

    // USB HID es independiente del bus SPI.
    initializeHIDModule();

    // La interfaz HTTP depende de que el punto de acceso este activo.
    initializeNetworkModule();
    initializeHttpServerModule();

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

    printInitializationSummary();

    SerialUSB.println();
    SerialUSB.println("       DAEMONHID INICIADO");
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

    // Actualiza el joystick y procesa sus eventos de navegacion.
    if (joystickReady) {
        joystickManager.update();

        const NavEvent navEvent = joystickManager.getNavEvent();

        if (displayReady) {
            displayManager.handleNav(navEvent);
        } else if (navEvent == NavEvent::Click) {
            // Si la pantalla no está activa pero el joystick sí, el botón sigue actuando de parada de emergencia
            if (hidManager.isPending() || hidManager.isExecuting()) {
                hidManager.abort();
                SerialUSB.println("[JOYSTICK] Accion cancelada por boton de emergencia.");
            }
        }
    }

    // Actualiza la pantalla TFT (animaciones, countdown, telemetria).
    if (displayReady) {
        displayManager.update();
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
            // PARADA DE EMERGENCIA (ABORTAR)
            // -----------------------------------------

            case 'x':
            case 'X':

                hidManager.abort();
                SerialUSB.println("[HID] Accion cancelada por comando serial.");
                break;

            // -----------------------------------------
            // AYUDA
            // -----------------------------------------

            case '?':

                SerialUSB.println();
                SerialUSB.println("========== AYUDA DAEMONHID ==========");
                SerialUSB.println("H - Ejecutar prueba de escritura de texto.");
                SerialUSB.println("P - Abrir CMD y ejecutar ping 8.8.8.8.");
                SerialUSB.println("X - Parada de emergencia (abortar HID).");
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
