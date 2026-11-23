#ifndef DAEMONHID_JOYSTICK_MANAGER_H
#define DAEMONHID_JOYSTICK_MANAGER_H

#include <Arduino.h>

/**
 * @brief Direcciones disponibles para el joystick.
 */
enum class JoystickDirection {
    Center,
    Up,
    Down,
    Left,
    Right
};

/**
 * @brief Eventos discretos de navegación para menús.
 */
enum class NavEvent {
    None,
    Up,
    Down,
    Left,
    Right,
    Click
};

/**
 * @brief Gestiona las entradas del joystick analogico.
 *
 * Detecta movimientos con antirrebote y repetición para navegación en menús.
 */
class JoystickManager {
public:
    JoystickManager(uint8_t pinX, uint8_t pinY, uint8_t pinButton);
    void begin();
    void update();

    JoystickDirection getDirection() const;
    bool wasButtonPressed();
    NavEvent getNavEvent();

private:
    uint8_t pinX;
    uint8_t pinY;
    uint8_t pinButton;

    JoystickDirection direction = JoystickDirection::Center;
    JoystickDirection lastNavDirection = JoystickDirection::Center;

    bool lastButtonReading = HIGH;
    bool stableButtonState = HIGH;
    bool buttonPressedEvent = false;

    unsigned long lastDebounceTime = 0;
    unsigned long navHoldStartTime = 0;
    unsigned long lastRepeatTime = 0;

    static constexpr unsigned long DEBOUNCE_DELAY_MS = 40;
    static constexpr unsigned long INITIAL_REPEAT_DELAY_MS = 350;
    static constexpr unsigned long REPEAT_RATE_MS = 180;

    static constexpr int AXIS_LOW_THRESHOLD = 350;
    static constexpr int AXIS_HIGH_THRESHOLD = 650;
};

#endif
