
#include "DisplayManager.h"

DisplayManager::DisplayManager(uint8_t cs, uint8_t dc, uint8_t rst)
    : display(cs, dc, rst) {
}

bool DisplayManager::begin() {
    // Inicializa la pantalla de 1.44 pulgadas.
    display.initR(INITR_144GREENTAB);

    // Configura la orientacion de la pantalla.
    display.setRotation(0);

    // Limpia la pantalla al iniciar.
    display.fillScreen(ST7735_BLACK);

    // Configura las opciones iniciales del texto.
    display.setTextWrap(true);
    display.setTextSize(1);
    display.setTextColor(ST7735_WHITE);

    initialized = true;

    return true;
}

void DisplayManager::clear(uint16_t color) {
    if (!initialized) {
        return;
    }

    // Rellena toda la pantalla con el color seleccionado.
    display.fillScreen(color);
}

void DisplayManager::showMessage(
    const char* message,
    uint16_t color,
    uint8_t textSize
) {
    if (!initialized || message == nullptr) {
        return;
    }

    // Limpia el contenido anterior.
    display.fillScreen(ST7735_BLACK);

    // Configura la apariencia del mensaje.
    display.setCursor(0, 0);
    display.setTextColor(color);
    display.setTextSize(textSize);
    display.setTextWrap(true);

    // Escribe el mensaje en la pantalla.
    display.print(message);
}
