#ifndef DAEMONHID_HID_MANAGER_H
#define DAEMONHID_HID_MANAGER_H

#include <Arduino.h>
#include <Keyboard.h>

/**
 * @brief Encapsulates USB HID keyboard functionality for DaemonHID.
 *
 * The initial implementation exposes only a fixed demonstration string.
 * It does not accept arbitrary text or key sequences.
 */
class HidManager {
public:
    void begin();
    bool requestDemoText();
    void update();
    bool isPending() const;

private:
    bool initialized = false;
    bool pending = false;
    uint32_t sendAt = 0;
};

#endif
