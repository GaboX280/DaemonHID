
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
    buttonPressedEvent = false;
}

void JoystickManager::update() {
    // Lee los valores analogicos de ambos ejes.
    const int xValue = analogRead(pinX);
    const int yValue = analogRead(pinY);

    // Determina la direccion del movimiento.
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
