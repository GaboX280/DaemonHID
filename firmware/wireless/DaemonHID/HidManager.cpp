
#include "HidManager.h"

void HidManager::begin() {
    // Espera a que la interfaz USB del UNO R4 WiFi esté lista.
    delay(500);

    Keyboard.begin();
    Keyboard.releaseAll();

    initialized = true;
    SerialUSB.println("HID inicializado correctamente.");
}

bool HidManager::requestDemoText() {
    if (!initialized || pending) {
        return false;
    }

    // Programa el envío del texto dentro de cinco segundos.
    currentDemo = DemoType::Text;
    sendAt = millis() + 5000;
    step = DemoStep::None;
    pending = true;

    return true;
}

bool HidManager::requestPingDemo() {
    if (!initialized || pending) {
        return false;
    }

    // Programa la demostración de conectividad dentro de cinco segundos.
    currentDemo = DemoType::Ping;
    sendAt = millis() + 5000;
    step = DemoStep::None;
    pending = true;

    return true;
}

void HidManager::update() {
    // Comprueba si hay una demostración pendiente y si llegó el momento
    // de ejecutar el siguiente paso.
    if (!pending || static_cast<int32_t>(millis() - sendAt) < 0) {
        return;
    }

    if (currentDemo == DemoType::Text) {
        // Envía únicamente el texto fijo de demostración.
        Keyboard.print("DaemonHID HID test - authorized test");
        Keyboard.releaseAll();

        currentDemo = DemoType::None;
        pending = false;
        step = DemoStep::None;
        return;
    }

    if (currentDemo != DemoType::Ping) {
        return;
    }

    // Ejecuta la demostración de ping mediante pasos no bloqueantes.
    switch (step) {
        case DemoStep::None:
            // Abre la ventana Ejecutar de Windows.
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press('r');

            step = DemoStep::ReleaseRun;
            sendAt = millis() + 100;
            break;

        case DemoStep::ReleaseRun:
            // Libera las teclas y espera a que aparezca la ventana.
            Keyboard.releaseAll();

            step = DemoStep::LaunchCmd;
            sendAt = millis() + 800;
            break;

        case DemoStep::LaunchCmd:
            // Escribe CMD y confirma la ejecución.
            Keyboard.print("cmd");
            Keyboard.press(KEY_RETURN);
            Keyboard.releaseAll();

            step = DemoStep::SendPing;
            sendAt = millis() + 1200;
            break;

        case DemoStep::SendPing:
            // Ejecuta una prueba de conectividad hacia 8.8.8.8.
            Keyboard.print("ping 8.8.8.8");
            Keyboard.press(KEY_RETURN);
            Keyboard.releaseAll();

            // Finaliza la demostración y libera el estado interno.
            currentDemo = DemoType::None;
            pending = false;
            step = DemoStep::None;
            break;
    }
}

bool HidManager::isPending() const {
    return pending;
}