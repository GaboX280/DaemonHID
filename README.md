# DaemonHID

**Desarrollo de un dispositivo de interacción USB-HID con conectividad inalámbrica mediante Arduino UNO R4 WiFi**

DaemonHID is an educational hardware and firmware project developed with the **Arduino UNO R4 WiFi**. It explores the integration of USB Human Interface Device (USB-HID) emulation, local wireless connectivity, HTTP communication, and modular peripheral management.

## Project information
- **Institution:** Universidad Fidélitas
- **Course:** Electrónica Digital y Microprocesadores (SC-205)
- **Authors:** Aaron Josua Herrera Arrieta, Nathan Herrera Salzar, and Gabriel Villalobos Alpízar

## Overview
The device combines a USB connection to a host computer with a local Wi-Fi network. The Arduino creates an access point named `DaemonHID` and runs an HTTP server on port `80`, reachable at `http://192.168.4.1` by devices connected to that network. The current web page is informational: it identifies the device and indicates server availability; it is not yet a full remote-control interface.

The firmware is written in C++ using the Arduino framework and organized into modules for network connectivity, HTTP handling, USB-HID demonstrations, display, microSD storage, and joystick input. The project is intended for educational exploration and authorized testing in controlled environments.

> **Current implementation status:** The integrated sketch enables the TFT, microSD, and joystick modules in its configuration. Their startup routines are coordinated from `setup()`, but the physical wiring, voltage compatibility, and operation of each peripheral still require validation on the assembled device.

## Features
### Core functions
- **USB-HID text demonstration (`H`):** schedules a fixed test string to be typed after a five-second countdown.
- **USB-HID connectivity demonstration (`P`):** schedules the Windows + R shortcut, opens Command Prompt, and types `ping 8.8.8.8` after a five-second countdown. The command executes on the host computer, not on the Arduino.
- **Serial help (`?`):** displays the available commands in the Serial Monitor.
- **Wi-Fi access point:** creates a local wireless network named `DaemonHID`.
- **HTTP server:** serves a basic HTML status page at `http://192.168.4.1`.

### Peripheral modules
- **DisplayManager:** controls a 1.44-inch ST7735 TFT display intended to present device information, system states, and navigation elements.
- **StorageManager:** provides microSD initialization and file operations, including writing, appending, reading, checking existence, and deleting files.
- **JoystickManager:** reads the analog X/Y axes and integrated button to detect directional input and selections.

The display, storage, and joystick modules are controlled by the `ENABLE_DISPLAY`, `ENABLE_STORAGE`, and `ENABLE_JOYSTICK` constants in `DaemonHID.ino`. These options are currently enabled. A successful software initialization does not prove that a module is physically connected or electrically compatible.

## Hardware
### Main components
- Arduino UNO R4 WiFi
- USB-C data and power cable
- Host computer (Windows is required for the documented `P` demonstration)
- 1.44-inch ST7735 TFT LCD display
- MicroSD reader module and microSD card
- Analog joystick module
- Jumper wires and mounting/enclosure materials

The peripheral modules are represented in firmware, but wiring, voltage requirements, and physical integration must be checked before enabling them.

## Development environment and libraries
- Arduino IDE
- Arduino UNO R4 board package
- `WiFiS3` (provided by the board platform)
- `Keyboard` library compatible with the Arduino UNO R4 WiFi
- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- Arduino SD library

## Repository structure
The firmware sketch is located in `firmware/wireless/DaemonHID/`.

```text
DaemonHID/
├── README.md
└── firmware/
    └── wireless/
        └── DaemonHID/
            ├── DaemonHID.ino
            ├── AppConfig.h
            ├── HardwareConfig.h
            ├── NetworkManager.h / .cpp
            ├── HttpServer.h / .cpp
            ├── HidManager.h / .cpp
            ├── DisplayManager.h / .cpp
            ├── StorageManager.h / .cpp
            └── JoystickManager.h / .cpp
```

## Firmware architecture
| Module | Responsibility |
| --- | --- |
| `DaemonHID.ino` | Coordinates initialization, the main loop, serial commands, and module updates. |
| `AppConfig.h` | Centralizes firmware version and network configuration. |
| `HardwareConfig.h` | Centralizes the UNO R4 WiFi pin assignment for TFT, microSD, joystick, and SPI. |
| `NetworkManager` | Creates and configures the Wi-Fi access point. |
| `HttpServer` | Starts the HTTP server and handles incoming client requests. |
| `HidManager` | Schedules and updates the fixed USB-HID demonstrations. |
| `DisplayManager` | Initializes and controls the ST7735 TFT display. |
| `StorageManager` | Manages microSD initialization and file operations. |
| `JoystickManager` | Reads joystick axes and button input, including direction and press events. |

Peripheral activation is controlled in `DaemonHID.ino`. The three `ENABLE_*` options are currently `true`. Set an option to `false` when its module is not installed or is being isolated during troubleshooting. Confirm voltage compatibility and wiring before powering the assembled device.

## Getting started
1. Install the Arduino IDE and the Arduino UNO R4 board package.
2. Open `firmware/wireless/DaemonHID/DaemonHID.ino`. Keep all accompanying `.h` and `.cpp` files in the same sketch folder.
3. Select **Arduino UNO R4 WiFi** and the serial port associated with the board.
4. Compile and upload the sketch.
5. Open the Serial Monitor at **115200 baud**.
6. Wait for the startup messages. The firmware reports the access-point status and IP address.

### Serial commands
| Command | Action |
| --- | --- |
| `H` | Schedules the fixed text demonstration. Focus a plain-text editor before the five-second countdown ends. |
| `P` | Schedules the Windows ping demonstration. Use only your own authorized, unlocked test computer and save open work first. |
| `?` | Prints the command help. |

### Connecting to the local page
1. Connect the host computer or mobile device to the `DaemonHID` Wi-Fi access point.
2. Open `http://192.168.4.1` in a browser.

The access point is a local network; it does not provide Internet access by itself. The HTTP page is hosted directly by the Arduino.

## Peripheral pinout and startup sequence

The integrated firmware uses the following pin assignment for the Arduino UNO R4 WiFi:

| Module pin | UNO R4 WiFi | Notes |
| --- | --- | --- |
| TFT `SCL` | D13 | SPI clock (`SCK`) |
| TFT `SDA` | D11 | SPI data to display (`MOSI/COPI`), not I2C SDA |
| TFT `CS` | D9 | TFT chip select |
| TFT `DC` / `A0` | D8 | Data/command selection |
| TFT `RES` / `RST` | D7 | Display reset |
| TFT `GND` | GND | Common ground |
| TFT `VCC`, `BL` | Per module specification | Verify the actual breakout board before applying power |
| microSD `SCK` | D13 | Shared SPI clock |
| microSD `MOSI` / `DI` | D11 | Shared SPI data to module |
| microSD `MISO` / `DO` | D12 | Data from module |
| microSD `CS` / `SS` | D10 | Independent chip select |
| microSD `VCC`, `GND` | Per module specification, GND common | Do not assume every six-pin board accepts 5 V |
| Joystick `VRx` / `X` | A0 | Analog X axis |
| Joystick `VRy` / `Y` | A1 | Analog Y axis |
| Joystick `SW` / `KEY` | D2 | Active-low button using `INPUT_PULLUP` |
| Joystick `VCC`, `GND` | 5V, GND | Confirm the module pin labels and wiring |

The TFT and microSD share the hardware SPI bus (D11, D12, D13) and must use separate chip-select pins (D9 and D10). The startup routine sets both chip-select pins HIGH before calling `SPI.begin()`, then initializes microSD, TFT, joystick, USB-HID, Wi-Fi AP, and HTTP server. The serial monitor prints a status summary for each subsystem.

**Electrical caution:** the UNO R4 WiFi uses 5 V logic. Some TFT breakouts and microSD boards operate at 3.3 V and may not tolerate 5 V signals. Verify whether each exact module includes a regulator and logic-level conversion; add a suitable level shifter when required. Do not infer voltage compatibility from the number of pins alone.

## Build and validation status
The integrated firmware has been compiled successfully for the Arduino UNO R4 WiFi.

Latest recorded compilation:
- Flash usage: **88,400 bytes (33.7%)** of 262,144 bytes.
- Global memory usage: **11,096 bytes (33.9%)** of 32,768 bytes.
- Remaining RAM: **21,672 bytes**.

Previously reported physical-board validation for the core functions:
- [x] Core firmware compiled and uploaded.
- [x] Startup messages displayed in the Serial Monitor.
- [x] `H` types the fixed test text.
- [x] `P` opens Command Prompt and types `ping 8.8.8.8`.
- [x] The `DaemonHID` Wi-Fi access point is visible.
- [x] The local HTTP page loads at `http://192.168.4.1`.
- [ ] Physically validate the ST7735 TFT integration.
- [ ] Physically validate microSD initialization and file operations.
- [ ] Physically validate joystick readings, direction thresholds, and button behavior.
- [ ] Add repeatable regression tests and document the final wiring and library versions.

Compilation confirms that the sketch builds for the selected board; it does not, by itself, confirm that the peripheral modules work with the physical hardware.

## Roadmap
1. Complete physical integration and validation of the TFT, microSD, and joystick.
2. Improve HTTP request handling and error reporting.
3. Expand the HTTP interface only after defining and reviewing its control and security model.
4. Evaluate serving web assets from microSD after storage has been validated.
5. Add repeatable tests and document final wiring, pin assignments, and library versions.

## Security and responsible use
- Run USB-HID demonstrations only on computers you own or are explicitly authorized to test.
- Save open work and focus the intended application before triggering a demonstration.
- The HID actions are fixed proof-of-concept demonstrations; do not use the device for hidden input, credential collection, persistence, or unauthorized command execution.
- Keep the access point and HTTP server within a controlled test environment.
- Do not commit Wi-Fi passwords, tokens, or other secrets to the repository. Store sensitive configuration outside tracked source files and rotate any credential that has been exposed.

## License
No project license has been defined yet.