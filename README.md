# DaemonHID

**DaemonHID** is an educational hardware and firmware project built around the **Arduino UNO R4 WiFi**. It explores modular C++ firmware, a local Wi-Fi access point, a lightweight HTTP page, and explicitly triggered USB HID demonstrations for controlled testing on the owner's own computer.

> **Current status:** The first hardware milestone is working. The firmware has been compiled and uploaded to an Arduino UNO R4 WiFi. The startup messages, USB HID demonstrations (`H` and `P`), Wi-Fi access point, and local web page have been tested on the physical board. MicroSD-hosted web assets and OLED support are future work.

## Current Features

- **USB HID text demonstration (`H`):** after a five-second countdown, types a fixed test string into the focused application.
- **USB HID ping demonstration (`P`):** after a five-second countdown, sends the Windows shortcut to open Run, launches Command Prompt, and types `ping 8.8.8.8`.
- **Serial help (`?`):** prints the available commands in the Serial Monitor.
- **Wi-Fi access point:** creates a local wireless network named `DaemonHID`.
- **Local HTTP server:** serves a simple HTML status page at `http://192.168.4.1` when connected to the access point.
- **Modular firmware:** network management, HTTP handling, configuration, and HID demonstrations are separated into C++ modules.

These are fixed proof-of-concept demonstrations. The firmware does not accept arbitrary text or commands from the serial interface or web page. The web page currently does not trigger HID actions.

## Hardware and Development Environment

- Arduino UNO R4 WiFi
- USB data cable
- Computer running Windows for the tested `P` demonstration
- Arduino IDE
- Arduino UNO R4 board package, including `WiFiS3` and the board-compatible `Keyboard` library

Planned hardware additions:
- MicroSD module/card for external web assets
- OLED display for device and network status

The additional hardware above is **not yet integrated**.

## Repository Structure

The current sketch is located in `firmware/wireless/DaemonHID/`:

```text
DaemonHID/
├── README.md
└── firmware/
    └── wireless/
        └── DaemonHID/
            ├── DaemonHID.ino
            ├── AppConfig.h
            ├── NetworkManager.h
            ├── NetworkManager.cpp
            ├── HttpServer.h
            ├── HttpServer.cpp
            ├── HidManager.h
            └── HidManager.cpp
```

### Module responsibilities

| File | Responsibility |
| --- | --- |
| `DaemonHID.ino` | Initializes modules, prints serial help, and dispatches `H`, `P`, and `?` commands. |
| `AppConfig.h` | Stores firmware version, HTTP port, access-point name, channel, and local configuration. |
| `NetworkManager.h/.cpp` | Starts the Wi-Fi access point. |
| `HttpServer.h/.cpp` | Accepts local HTTP requests and currently serves an HTML page embedded in the C++ implementation. |
| `HidManager.h/.cpp` | Runs the fixed USB HID text and ping demonstrations after a countdown. |

## Getting Started

1. Install the Arduino IDE and the Arduino UNO R4 board package.
2. Open `firmware/wireless/DaemonHID/DaemonHID.ino`. Keep the sketch folder and the accompanying `.h` and `.cpp` files together.
3. Select **Arduino UNO R4 WiFi** and the port corresponding to the board.
4. Compile and upload the sketch.
5. Open the Serial Monitor at **115200 baud**.
6. Wait for the startup messages. The firmware reports the access-point name and IP address.

### Serial commands

| Command | Action |
| --- | --- |
| `H` | Schedules the fixed text demonstration after five seconds. Focus a plain-text editor before the countdown ends. |
| `P` | Schedules the fixed Windows ping demonstration after five seconds. Use only your own unlocked test PC and save open work first. |
| `?` | Prints the command help. |

For the web interface, connect your computer to the `DaemonHID` access point and open **http://192.168.4.1** in a browser. The local access point does not itself provide Internet access; this page is hosted by the board.

## Validation Status

The following results have been reported from tests on the physical Arduino UNO R4 WiFi:

- [x] Firmware compiled and uploaded with Arduino IDE.
- [x] Startup messages appear in the Serial Monitor.
- [x] `H` types the fixed test text.
- [x] `P` opens Command Prompt and runs `ping 8.8.8.8`.
- [x] The `DaemonHID` Wi-Fi network is visible.
- [x] The local page loads at `http://192.168.4.1`.
- [ ] Serve HTML, CSS, and JavaScript from a microSD card.
- [ ] Add an OLED status display.
- [ ] Add automated or repeatable regression tests for the integrated features.
- [ ] Add wiring diagrams and document the exact board package/library versions.

## Roadmap

1. Improve HTTP request handling, including a bounded client wait, and review error reporting.
2. Move the page into separate HTML, CSS, and JavaScript files.
3. Implement and validate microSD storage before serving external assets.
4. Add an OLED display for basic device and network status.
5. Expand test notes and document hardware/library versions.
6. Review the security model before adding any control actions to the web interface.

## Security and Safety

- Use HID demonstrations only on computers you own or are explicitly authorized to test.
- Save open work and focus the intended target application before triggering a demonstration.
- Keep actions fixed, visible, and initiated deliberately.
- Do not add hidden input, persistence, credential collection, or arbitrary command execution.
- Do not expose the local interface to other networks without a reviewed security design.
- Keep Wi-Fi credentials out of documentation and public commits. If an access-point password has been committed to a public repository, rotate it and move future secrets out of tracked source files.

## License

No project license has been defined yet.
