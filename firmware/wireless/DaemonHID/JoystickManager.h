
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
 * @brief Gestiona las entradas del joystick analogico.
 *
 * Detecta movimientos en los ejes X e Y y las pulsaciones
 * del boton mediante una lectura con antirrebote.
 */
class JoystickManager {
public:
    /**
     * @brief Configura los pines del joystick.
     */
    JoystickManager(uint8_t pinX, uint8_t pinY, uint8_t pinButton);

    /**
     * @brief Inicializa las entradas del joystick.
     */
    void begin();

    /**
     * @brief Actualiza las lecturas del joystick.
     *
     * Debe ejecutarse continuamente desde loop().
     */
    void update();

    /**
     * @brief Devuelve la direccion actual.
     */
    JoystickDirection getDirection() const;

    /**
     * @brief Comprueba si se produjo una nueva pulsacion.
     *
     * Consume el evento al consultarlo.
     */
    bool wasButtonPressed();

private:
    uint8_t pinX;
    uint8_t pinY;
    uint8_t pinButton;

    JoystickDirection direction = JoystickDirection::Center;

    bool lastButtonReading = HIGH;
    bool stableButtonState = HIGH;
    bool buttonPressedEvent = false;

    unsigned long lastDebounceTime = 0;

    static constexpr unsigned long DEBOUNCE_DELAY_MS = 40;

    static constexpr int AXIS_LOW_THRESHOLD = 350;
    static constexpr int AXIS_HIGH_THRESHOLD = 650;
};

#endif
