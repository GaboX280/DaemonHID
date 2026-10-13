#include "HidManager.h"

void HidManager::begin() {
    Keyboard.begin();
    Keyboard.releaseAll();
    initialized = true;
}

bool HidManager::requestDemoText() {
    if (!initialized || pending) {
        return false;
    }

    // Give the user five seconds to focus a plain-text editor.
    sendAt = millis() + 5000;
    pending = true;
    return true;
}

void HidManager::update() {
    if (!pending || static_cast<int32_t>(millis() - sendAt) < 0) {
        return;
    }

    // Fixed test string only; never execute arbitrary received commands.
    Keyboard.print("DaemonHID HID test - authorized test");
    Keyboard.releaseAll();
    pending = false;
}

bool HidManager::isPending() const {
    return pending;
}
