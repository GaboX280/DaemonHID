#include "HidManager.h"
#include "StorageManager.h"

bool HidManager::begin(StorageManager* storage) {
    storageMgr = storage;
    if (initialized) return true;

    // Espera a que la interfaz USB del UNO R4 WiFi esté lista.
    delay(500);

    Keyboard.begin();
    Keyboard.releaseAll();

    initialized = true;
    SerialUSB.println("[HID] Interfaz USB HID inicializada.");
    return true;
}

void HidManager::setStorageManager(StorageManager* storage) {
    storageMgr = storage;
}

bool HidManager::requestDemoText() {
    if (!initialized || pending || executingFile) {
        return false;
    }

    currentDemo = DemoType::Text;
    sendAt = millis() + 5000;
    step = DemoStep::None;
    pending = true;

    SerialUSB.println("[HID] Demo de texto programada en 5 segundos.");
    return true;
}

bool HidManager::requestPingDemo() {
    if (!initialized || pending || executingFile) {
        return false;
    }

    currentDemo = DemoType::Ping;
    sendAt = millis() + 5000;
    step = DemoStep::None;
    pending = true;

    SerialUSB.println("[HID] Demo de ping programada en 5 segundos.");
    return true;
}

bool HidManager::requestPayloadFile(const char* filename) {
    if (!initialized || pending || executingFile || filename == nullptr || filename[0] == '\0') {
        return false;
    }

    if (strcmp(filename, "DEMO_TEXT") == 0) {
        return requestDemoText();
    }
    if (strcmp(filename, "DEMO_PING") == 0) {
        return requestPingDemo();
    }

    String fullPath = filename;
    if (!fullPath.startsWith("/")) {
        fullPath = "/payloads/" + fullPath;
    }

    if (!storageMgr || !storageMgr->isReady() || !storageMgr->exists(fullPath.c_str())) {
        SerialUSB.print("[HID] Error: Script no encontrado en MicroSD: ");
        SerialUSB.println(fullPath);
        return false;
    }

    strncpy(targetPayloadFile, filename, sizeof(targetPayloadFile) - 1);
    targetPayloadFile[sizeof(targetPayloadFile) - 1] = '\0';

    currentDemo = DemoType::FilePayload;
    sendAt = millis() + 5000;
    step = DemoStep::None;
    pending = true;
    executingFile = false;

    SerialUSB.print("[HID] Script programado para ejecucion en 5s: ");
    SerialUSB.println(targetPayloadFile);
    return true;
}

void HidManager::abort() {
    if (activeFile) {
        activeFile.close();
    }
    Keyboard.releaseAll();

    pending = false;
    executingFile = false;
    currentDemo = DemoType::None;
    step = DemoStep::None;
    targetPayloadFile[0] = '\0';

    SerialUSB.println("[HID] Demostracion / script abortado. Teclas liberadas.");
}

void HidManager::processScriptLine(const String& line) {
    if (line.startsWith("REM") || line.startsWith("#") || line.startsWith("//")) {
        // Linea de comentario, saltar de inmediato
        sendAt = millis() + 5;
        return;
    }

    if (line.startsWith("DELAY ") || line.startsWith("delay ")) {
        const int ms = line.substring(6).toInt();
        sendAt = millis() + (ms > 0 ? ms : 10);
        return;
    }

    if (line.startsWith("STRING ") || line.startsWith("string ")) {
        const String text = line.substring(7);
        Keyboard.print(text);
        sendAt = millis() + 25;
        return;
    }

    if (line.equalsIgnoreCase("ENTER")) {
        Keyboard.press(KEY_RETURN);
        delay(15);
        Keyboard.releaseAll();
        sendAt = millis() + 50;
        return;
    }

    if (line.startsWith("GUI ") || line.startsWith("WINDOWS ")) {
        const int pos = line.indexOf(' ');
        const char k = line.charAt(pos + 1);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press(k);
        delay(20);
        Keyboard.releaseAll();
        sendAt = millis() + 100;
        return;
    }

    if (line.startsWith("CTRL ") || line.startsWith("CONTROL ")) {
        const int pos = line.indexOf(' ');
        const char k = line.charAt(pos + 1);
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(k);
        delay(20);
        Keyboard.releaseAll();
        sendAt = millis() + 100;
        return;
    }

    if (line.startsWith("ALT ")) {
        const int pos = line.indexOf(' ');
        const char k = line.charAt(pos + 1);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(k);
        delay(20);
        Keyboard.releaseAll();
        sendAt = millis() + 100;
        return;
    }

    if (line.equalsIgnoreCase("TAB")) {
        Keyboard.press(KEY_TAB);
        delay(15);
        Keyboard.releaseAll();
        sendAt = millis() + 50;
        return;
    }

    if (line.equalsIgnoreCase("ESC") || line.equalsIgnoreCase("ESCAPE")) {
        Keyboard.press(KEY_ESC);
        delay(15);
        Keyboard.releaseAll();
        sendAt = millis() + 50;
        return;
    }

    if (line.equalsIgnoreCase("SPACE")) {
        Keyboard.print(" ");
        sendAt = millis() + 30;
        return;
    }

    if (line.equalsIgnoreCase("BACKSPACE")) {
        Keyboard.press(KEY_BACKSPACE);
        delay(15);
        Keyboard.releaseAll();
        sendAt = millis() + 50;
        return;
    }

    // Por defecto, escribir el texto plano de la linea
    Keyboard.print(line);
    sendAt = millis() + 30;
}

void HidManager::endScript() {
    if (activeFile) {
        activeFile.close();
    }
    Keyboard.releaseAll();

    executingFile = false;
    currentDemo = DemoType::None;
    step = DemoStep::None;
    targetPayloadFile[0] = '\0';

    SerialUSB.println("[HID] Ejecucion de script finalizada exitosamente.");
}

void HidManager::update() {
    // Si no hay nada que hacer, salir
    if (!pending && !executingFile && currentDemo == DemoType::None) {
        return;
    }

    // Comprobar si todavia debemos esperar
    if (static_cast<int32_t>(millis() - sendAt) < 0) {
        return;
    }

    // Transicion: finalizar cuenta regresiva e iniciar accion
    if (pending) {
        pending = false;

        if (currentDemo == DemoType::Text) {
            Keyboard.print("DaemonHID HID test - authorized test");
            Keyboard.releaseAll();
            currentDemo = DemoType::None;
            step = DemoStep::None;
            return;
        }

        if (currentDemo == DemoType::Ping) {
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press('r');
            step = DemoStep::ReleaseRun;
            sendAt = millis() + 100;
            return;
        }

        if (currentDemo == DemoType::FilePayload) {
            String fullPath = targetPayloadFile;
            if (!fullPath.startsWith("/")) {
                fullPath = "/payloads/" + fullPath;
            }

            if (!storageMgr) {
                SerialUSB.println("[HID] Error: StorageManager no configurado.");
                abort();
                return;
            }

            activeFile = storageMgr->openFile(fullPath.c_str(), FILE_READ);
            if (!activeFile) {
                SerialUSB.println("[HID] Error: No se pudo abrir el archivo de script.");
                abort();
                return;
            }

            executingFile = true;
            step = DemoStep::ExecutingFile;
            sendAt = millis() + 20;
            SerialUSB.println("[HID] Iniciando interpretacion de lineas...");
            return;
        }
    }

    // Demostracion fija de ping por pasos
    if (currentDemo == DemoType::Ping) {
        switch (step) {
            case DemoStep::ReleaseRun:
                Keyboard.releaseAll();
                step = DemoStep::LaunchCmd;
                sendAt = millis() + 800;
                break;

            case DemoStep::LaunchCmd:
                Keyboard.print("cmd");
                Keyboard.press(KEY_RETURN);
                Keyboard.releaseAll();
                step = DemoStep::SendPing;
                sendAt = millis() + 1200;
                break;

            case DemoStep::SendPing:
                Keyboard.print("ping 8.8.8.8");
                Keyboard.press(KEY_RETURN);
                Keyboard.releaseAll();
                currentDemo = DemoType::None;
                step = DemoStep::None;
                break;

            default:
                break;
        }
        return;
    }

    // Interpretacion continua del archivo de script desde MicroSD
    if (executingFile) {
        if (!activeFile || !activeFile.available()) {
            endScript();
            return;
        }

        String line = activeFile.readStringUntil('\n');
        line.trim();

        if (line.length() == 0) {
            // Linea en blanco, continuar en el proximo ciclo
            sendAt = millis() + 10;
            return;
        }

        processScriptLine(line);
    }
}

bool HidManager::isPending() const {
    return pending;
}

bool HidManager::isExecuting() const {
    return executingFile || (step != DemoStep::None);
}

const char* HidManager::getStateString() const {
    if (!initialized) return "desactivado";
    if (pending) return "cuenta_regresiva";
    if (executingFile || step != DemoStep::None) return "escribiendo";
    return "inactivo";
}

int HidManager::getCountdownRemaining() const {
    if (!pending) return 0;
    const long diff = static_cast<long>(sendAt) - static_cast<long>(millis());
    if (diff <= 0) return 0;
    return static_cast<int>((diff + 999) / 1000);
}

const char* HidManager::getActivePayloadName() const {
    return targetPayloadFile;
}
