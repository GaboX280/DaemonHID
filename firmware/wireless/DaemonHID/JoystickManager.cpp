#include "JoystickManager.h"

JoystickManager::JoystickManager(
    uint8_t pinX,
    uint8_t pinY,
    uint8_t pinButton
) :
    pinX(pinX),
    pinY(pinY),
    pinButton(pinButton) {
}

void JoystickManager::begin() {
    // Configura los ejes analogicos como entradas.
    pinMode(pinX, INPUT);
    pinMode(pinY, INPUT);

    // Activa la resistencia pull-up interna del boton.
    pinMode(pinButton, INPUT_PULLUP);

    // Obtiene el estado inicial del boton.
    lastButtonReading = digitalRead(pinButton);
    stableButtonState = lastButtonReading;

    direction = JoystickDirection::Center;
    lastNavDirection = JoystickDirection::Center;
    buttonPressedEvent = false;
    navHoldStartTime = 0;
    lastRepeatTime = 0;
}

void JoystickManager::update() {
    // Lee los valores analogicos de ambos ejes.
    const int xValue = analogRead(pinX);
    const int yValue = analogRead(pinY);

    // Determina la direccion del movimiento con histeresis.
    if (xValue < AXIS_LOW_THRESHOLD) {
        direction = JoystickDirection::Left;
    } else if (xValue > AXIS_HIGH_THRESHOLD) {
        direction = JoystickDirection::Right;
    } else if (yValue < AXIS_LOW_THRESHOLD) {
        direction = JoystickDirection::Up;
    } else if (yValue > AXIS_HIGH_THRESHOLD) {
        direction = JoystickDirection::Down;
    } else {
        direction = JoystickDirection::Center;
    }

    // Lee el estado actual del boton.
    const bool reading = digitalRead(pinButton);

    // Reinicia el temporizador si cambia la lectura.
    if (reading != lastButtonReading) {
        lastDebounceTime = millis();
    }

    // Comprueba si el nuevo estado se mantuvo estable.
    if (millis() - lastDebounceTime >= DEBOUNCE_DELAY_MS) {
        if (reading != stableButtonState) {
            stableButtonState = reading;

            // Detecta solamente el momento de la pulsacion.
            if (stableButtonState == LOW) {
                buttonPressedEvent = true;
            }
        }
    }

    lastButtonReading = reading;
}

JoystickDirection JoystickManager::getDirection() const {
    return direction;
}

bool JoystickManager::wasButtonPressed() {
    // Devuelve y consume el evento de pulsacion.
    if (!buttonPressedEvent) {
        return false;
    }

    buttonPressedEvent = false;
    return true;
}

NavEvent JoystickManager::getNavEvent() {
    // El botón tiene máxima prioridad
    if (wasButtonPressed()) {
        return NavEvent::Click;
    }

    // Si la palanca está en el centro, reiniciar contadores de repetición
    if (direction == JoystickDirection::Center) {
        lastNavDirection = JoystickDirection::Center;
        navHoldStartTime = 0;
        return NavEvent::None;
    }

    const unsigned long now = millis();

    // Nueva pulsación de dirección (flanco ascendente)
    if (lastNavDirection == JoystickDirection::Center) {
        lastNavDirection = direction;
        navHoldStartTime = now;
        lastRepeatTime = now;

        switch (direction) {
            case JoystickDirection::Up: return NavEvent::Up;
            case JoystickDirection::Down: return NavEvent::Down;
            case JoystickDirection::Left: return NavEvent::Left;
            case JoystickDirection::Right: return NavEvent::Right;
            default: return NavEvent::None;
        }
    }

    // Mantenimiento de la misma dirección con auto-repetición controlada
    if (direction == lastNavDirection) {
        if (now - navHoldStartTime >= INITIAL_REPEAT_DELAY_MS) {
            if (now - lastRepeatTime >= REPEAT_RATE_MS) {
                lastRepeatTime = now;
                switch (direction) {
                    case JoystickDirection::Up: return NavEvent::Up;
                    case JoystickDirection::Down: return NavEvent::Down;
                    case JoystickDirection::Left: return NavEvent::Left;
                    case JoystickDirection::Right: return NavEvent::Right;
                    default: return NavEvent::None;
                }
            }
        }
    }

    return NavEvent::None;
}
