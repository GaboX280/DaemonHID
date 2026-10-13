#include "HidManager.h"

void HidManager::begin() {
    // Allow the UNO R4 WiFi USB interface to settle before HID use.
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
    if (!pending || static_cast<int32_t>(millis() - sendAt) < 0) {
        return;
    }

    if (currentDemo == DemoType::Text) {
        // Fixed demonstration text only.
        Keyboard.print("DaemonHID HID test - authorized test");
        Keyboard.releaseAll();
    } else if (currentDemo == DemoType::Ping) {
        // Fixed, non-elevated Windows connectivity demonstration.
        // Keep the test desktop focused and free of unsaved work.
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');
        delay(100);
        Keyboard.releaseAll();

        delay(800);
        Keyboard.print("cmd");
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();

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
