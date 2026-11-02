
#ifndef DAEMONHID_DISPLAY_MANAGER_H
#define DAEMONHID_DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

/**
 * @brief Gestiona la pantalla TFT ST7735.
 *
 * Permite inicializar la pantalla, borrar su contenido
 * y mostrar mensajes de texto.
 */
class DisplayManager {
public:
    /**
     * @brief Configura los pines de la pantalla.
     */
    DisplayManager(uint8_t cs, uint8_t dc, uint8_t rst);

    /**
     * @brief Inicializa la pantalla TFT.
     */
    bool begin();

    /**
     * @brief Limpia el contenido de la pantalla.
     */
    void clear(uint16_t color = ST7735_BLACK);

    /**
     * @brief Muestra un mensaje en la pantalla.
     */
    void showMessage(
        const char* message,
        uint16_t color = ST7735_WHITE,
        uint8_t textSize = 1
    );

private:
    Adafruit_ST7735 display;

    bool initialized = false;
};

#endif
