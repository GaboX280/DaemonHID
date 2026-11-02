# DaemonHID

**DaemonHID** is an educational hardware and firmware project built around the **Arduino UNO R4 WiFi**. It combines USB HID demonstrations, a standalone Wi-Fi access point, a lightweight HTTP status page, and a modular architecture designed for progressive integration of physical peripherals.

The project is developed in C++ using the Arduino framework. Its modules separate network, HTTP, HID, display, storage, and joystick responsibilities.

> **Current status:** The core firmware has been compiled for the Arduino UNO R4 WiFi. The main hardware functions (USB HID demonstrations, Wi-Fi access point, and local HTTP page) have previously been validated on the physical board. The TFT display, microSD reader, and joystick modules are present in the codebase but are disabled by default while their physical integration is pending.

## Features

### Core functions
- **USB HID text demonstration (H):** schedules a fixed test string to be typed after a five-second countdown.
- **USB HID ping demonstration (P):** schedules the Windows + R shortcut, opens Command Prompt, and types ping 8.8.8.8 after a five-second countdown. The command runs on the connected host computer, not on the Arduino.
- **Serial help (?):** displays the available commands in the Serial Monitor.
- **Wi-Fi access point:** creates a local wireless network named DaemonHID.
- **HTTP server:** serves a basic HTML status page at http://192.168.4.1 when a device is connected to the access point.

### Peripheral modules in the firmware
- **DisplayManager:** controls a 1.44-inch ST7735 TFT display.
- **StorageManager:** provides microSD initialization and basic file operations (write, append, read, check existence, and delete).
- **JoystickManager:** reads the analog X/Y axes and button, and exposes directional input.

The display, storage, and joystick modules are currently disabled in the integrated sketch through the ENABLE_DISPLAY, ENABLE_STORAGE, and ENABLE_JOYSTICK configuration constants. Their physical operation has not yet been validated as part of the integrated device.

The HTTP page is currently informational: it does not trigger HID actions or provide a full device-control interface. The firmware does not accept arbitrary text or commands from the web page or serial interface.

## Hardware
### Main board
- Arduino UNO R4 WiFi
- USB data cable
- Host computer (Windows is required for the documented P demonstration)

### Peripherals planned for integration
- 1.44-inch ST7735 TFT display
- MicroSD module and card
- Analog joystick module

The peripheral modules are represented in firmware, but their wiring and physical integration must be verified before enabling them.

## Development environment
- Arduino IDE
- Arduino UNO R4 board package
- WiFiS3 (included with the board platform)
- Board-compatible Keyboard library
- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- SD library available through the Arduino platform

## Repository structure
The firmware sketch is located in firmware/wireless/DaemonHID/.

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

## Firmware architecture
| Module | Responsibility |
| --- | --- |
| DaemonHID.ino | Coordinates initialization, the main loop, serial commands, and module updates. |
| AppConfig.h | Centralizes firmware version and network configuration. |
| NetworkManager | Creates and configures the Wi-Fi access point. |
| HttpServer | Starts the HTTP server and handles incoming client requests. |
| HidManager | Schedules and updates the fixed USB HID demonstrations. |
| DisplayManager | Initializes and controls the ST7735 TFT display. |
| StorageManager | Manages microSD initialization and file operations. |
| JoystickManager | Reads joystick axes and button input, including direction and press events. |

Peripheral activation is controlled in DaemonHID.ino. The three ENABLE_* options are false by default. Keep them disabled until the corresponding hardware connections have been checked and the modules have been tested on the physical board.

## Getting started
1. Install the Arduino IDE and the Arduino UNO R4 board package.
2. Open firmware/wireless/DaemonHID/DaemonHID.ino. Keep all accompanying header and source files in the same sketch folder.
3. Select **Arduino UNO R4 WiFi** and the serial port associated with the board.
4. Compile and upload the sketch.
5. Open the Serial Monitor at **115200 baud**.
6. Wait for the startup messages. The firmware reports the access-point status and IP address.

### Serial commands
| Command | Action |
| --- | --- |
| H | Schedules the fixed text demonstration. Focus a plain-text editor before the five-second countdown ends. |
| P | Schedules the Windows ping demonstration. Use only your own authorized, unlocked test computer and save open work first. |
| ? | Prints the command help. |

### Connecting to the local page
1. Connect the host computer or mobile device to the DaemonHID Wi-Fi access point.
2. Open http://192.168.4.1 in a browser.

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
- [x] H types the fixed test text.
- [x] P opens Command Prompt and types ping 8.8.8.8.
- [x] The DaemonHID Wi-Fi access point is visible.
- [x] The local HTTP page loads at http://192.168.4.1.
- [ ] Physically validate the ST7735 TFT integration.
- [ ] Physically validate microSD initialization and file operations.
- [ ] Physically validate joystick readings, direction thresholds, and button behavior.
- [ ] Add repeatable regression tests and document the final wiring and library versions.

Compilation confirms that the sketch builds for the selected board; it does not, by itself, confirm that the peripheral modules work with the physical hardware.

## Roadmap
1. Complete physical integration and validation of the TFT, microSD, and joystick.
2. Improve HTTP request handling and error reporting.
3. Separate the HTML, CSS, and JavaScript assets if the web interface is expanded.
4. Evaluate serving web assets from microSD after storage has been validated.
5. Add repeatable tests and document final wiring, pin assignments, and library versions.
6. Review the security model before introducing any remote control functionality.

## Security and responsible use
- Run USB HID demonstrations only on computers you own or are explicitly authorized to test.
- Save open work and focus the intended application before triggering a demonstration.
- The HID actions are fixed proof-of-concept demonstrations; do not use the device for hidden input, credential collection, persistence, or unauthorized command execution.
- Keep the access point and HTTP server within a controlled test environment.
- Do not commit Wi-Fi passwords, tokens, or other secrets to the repository. Store sensitive configuration outside tracked source files and rotate any credential that has been exposed.

## License
No project license has been defined yet.