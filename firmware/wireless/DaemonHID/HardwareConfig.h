#ifndef DAEMONHID_HARDWARE_CONFIG_H
#define DAEMONHID_HARDWARE_CONFIG_H

#include <Arduino.h>

/**
 * @brief Asignacion de pines para DaemonHID sobre Arduino UNO R4 WiFi.
 *
 * El bus SPI por hardware del UNO R4 WiFi utiliza:
 *   D11 = MOSI/COPI, D12 = MISO/CIPO, D13 = SCK.
 *
 * En la TFT ST7735 de 8 pines, SDA corresponde a MOSI y SCL a SCK
 * cuando el modulo esta configurado para SPI.
 */
namespace HardwareConfig {

    constexpr uint8_t TFT_CS_PIN = 9;
    constexpr uint8_t TFT_DC_PIN = 8;
    constexpr uint8_t TFT_RST_PIN = 7;

    constexpr uint8_t SD_CS_PIN = 10;

    constexpr uint8_t JOYSTICK_X_PIN = A0;
    constexpr uint8_t JOYSTICK_Y_PIN = A1;
    constexpr uint8_t JOYSTICK_BUTTON_PIN = 2;

    constexpr uint8_t SPI_MOSI_PIN = 11;
    constexpr uint8_t SPI_MISO_PIN = 12;
    constexpr uint8_t SPI_SCK_PIN = 13;
}

#endif
