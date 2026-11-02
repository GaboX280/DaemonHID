
#ifndef DAEMONHID_HID_MANAGER_H
#define DAEMONHID_HID_MANAGER_H

#include <Arduino.h>
#include <Keyboard.h>

/**
 * @brief Gestiona las demostraciones predefinidas mediante USB HID.
 *
 * Solo permite ejecutar demostraciones previamente definidas.
 * No acepta comandos ni texto arbitrario desde el monitor serial
 * o la red.
 */
class HidManager {
public:
    void begin();
    bool requestDemoText();
    bool requestPingDemo();
    void update();
    bool isPending() const;

private:
    // Identifica el tipo de demostración solicitada.
    enum class DemoType {
        None,
        Text,
        Ping
    };

    // Identifica el paso actual de una demostración.
    enum class DemoStep {
        None,
        ReleaseRun,
        LaunchCmd,
        SendPing
    };

    bool initialized = false;
    bool pending = false;

    // Momento en que debe ejecutarse el siguiente paso.
    uint32_t sendAt = 0;

    DemoType currentDemo = DemoType::None;
    DemoStep step = DemoStep::None;
};

#endif
