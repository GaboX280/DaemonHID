
#ifndef DAEMONHID_HID_MANAGER_H
#define DAEMONHID_HID_MANAGER_H

#include <Arduino.h>
#include <Keyboard.h>

class HidManager {
public:
    void begin();
    bool requestDemoText();
    bool requestPingDemo();
    void update();
    bool isPending() const;

private:
    enum class DemoType {
        None,
        Text,
        Ping
    };

    bool initialized = false;
    bool pending = false;
    uint32_t sendAt = 0;
    DemoType currentDemo = DemoType::None;
};

#endif
