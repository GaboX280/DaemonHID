
#include "HidManager.h"

void HidManager::begin() {
    delay(500);

    Keyboard.begin();
    Keyboard.releaseAll();

    initialized = true;
    SerialUSB.println("HID initialized successfully.");
}

bool HidManager::requestDemoText() {
    if (!initialized || pending) {
        return false;
    }

    currentDemo = DemoType::Text;
    sendAt = millis() + 5000;
    pending = true;
    return true;
}

bool HidManager::requestPingDemo() {
    if (!initialized || pending) {
        return false;
    }

    currentDemo = DemoType::Ping;
    sendAt = millis() + 5000;
    pending = true;
    return true;
}

void HidManager::update() {
    if (!pending ||
        static_cast<int32_t>(millis() - sendAt) < 0) {
        return;
    }

    if (currentDemo == DemoType::Text) {
        Keyboard.print("DaemonHID HID test - authorized test");
        Keyboard.releaseAll();
    } else if (currentDemo == DemoType::Ping) {
        // Open Windows Run dialog.
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');
        delay(100);
        Keyboard.releaseAll();

        // Open a standard, non-elevated CMD window.
        delay(800);
        Keyboard.print("cmd");
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();

        // Execute a fixed connectivity test.
        delay(1200);
        Keyboard.print("ping 8.8.8.8");
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();
    }

    currentDemo = DemoType::None;
    pending = false;
}

bool HidManager::isPending() const {
    return pending;
}

