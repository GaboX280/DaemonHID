#ifndef DAEMONHID_HID_MANAGER_H
#define DAEMONHID_HID_MANAGER_H

#include <Arduino.h>
#include <Keyboard.h>

/**
 * @brief Encapsulates USB HID keyboard functionality for DaemonHID.
 *
 * This first implementation intentionally exposes only a fixed, harmless
 * demonstration string. It does not accept arbitrary text or key sequences.
 */
class HidManager {
public:
    void begin();
    bool sendDemoText();

private:
    bool initialized = false;
};

#endif
