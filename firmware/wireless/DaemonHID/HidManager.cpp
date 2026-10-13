#include "HidManager.h"

void HidManager::begin() {
    Keyboard.begin();
    Keyboard.releaseAll();
    initialized = true;
}

bool HidManager::sendDemoText() {
    if (!initialized) {
        return false;
    }

    // Fixed test payload: type only into the currently focused text field.
    Keyboard.print("DaemonHID HID test - authorized test");
    Keyboard.releaseAll();

    return true;
}
