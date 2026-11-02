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

> **Current implementation status:** The integrated firmware has been compiled for the Arduino UNO R4 WiFi. Core functions—USB-HID demonstrations, the Wi-Fi access point, and the local HTTP page—have previously been validated on the physical board. The TFT, microSD, and joystick modules are implemented in the codebase but disabled by default; their physical integration and operation remain to be validated.

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

The display, storage, and joystick modules are disabled in the integrated sketch through the `ENABLE_DISPLAY`, `ENABLE_STORAGE`, and `ENABLE_JOYSTICK` configuration constants. Their physical operation has not yet been validated as part of the integrated device.

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
| `NetworkManager` | Creates and configures the Wi-Fi access point. |
| `HttpServer` | Starts the HTTP server and handles incoming client requests. |
| `HidManager` | Schedules and updates the fixed USB-HID demonstrations. |
| `DisplayManager` | Initializes and controls the ST7735 TFT display. |
| `StorageManager` | Manages microSD initialization and file operations. |
| `JoystickManager` | Reads joystick axes and button input, including direction and press events. |

Peripheral activation is controlled in `DaemonHID.ino`. The three `ENABLE_*` options are `false` by default. Keep them disabled until the corresponding hardware connections have been checked and the modules have been tested on the physical board.

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