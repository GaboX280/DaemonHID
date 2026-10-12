# DaemonHID

**DaemonHID** is an educational hardware and firmware project based on the **Arduino UNO R4 WiFi**. Its goal is to explore the interaction between physical hardware, local wireless connectivity, and USB input capabilities, with an emphasis on learning, security, and controlled testing.

> **Status:** Early development. Features described as future work have not yet been implemented or validated.

## Goals

- Study the capabilities and limitations of the Arduino UNO R4 WiFi.
- Develop a local web interface using the board's Wi-Fi access point.
- Investigate the board's compatibility with USB HID features and first validate a minimal proof of concept.
- Document wiring, design decisions, and reproducible results.
- Include physical controls and clear safeguards so input functions are activated intentionally.

## Hardware

- Arduino UNO R4 WiFi.
- Compatible USB data cable.
- MicroSD card module (planned; not yet available).
- Additional components from the starter kit, depending on each test.

HID functionality depends on the hardware, Arduino core, and compatible libraries. Support for a feature must not be assumed until it has been verified on the actual board.

## Repository Structure

The initial layout separates firmware, web assets, hardware references, and documentation:

```text
DaemonHID/
├── README.md
├── .gitignore
├── firmware/
│   └── wireless/
│       └── DaemonHID/
│           ├── DaemonHID.ino
│           ├── AppConfig.h
│           ├── NetworkManager.h
│           ├── NetworkManager.cpp
│           ├── HttpServer.h
│           ├── HttpServer.cpp
│           ├── StorageManager.h
│           ├── StorageManager.cpp
│           ├── DeviceController.h
│           └── DeviceController.cpp
├── web/
│   ├── index.html
│   ├── styles.css
│   └── app.js
├── hardware/
├── docs/
└── tests/
```

The firmware files are grouped in one Arduino IDE sketch folder. The `.ino` filename matches the sketch folder name. The `.h` and `.cpp` module files are kept next to the sketch initially to make them easy to open as IDE tabs. The `web/` directory contains the website assets that will later be copied to the microSD card.

Directories will be added as needed; Git does not track empty directories.

## Planned Module Responsibilities

- **`AppConfig.h`:** shared compile-time settings such as the HTTP port and web asset paths.
- **`NetworkManager`:** manages the local Wi-Fi access point using the board's supported networking library.
- **`HttpServer`:** handles browser requests and sends HTTP responses.
- **`StorageManager`:** initializes the microSD card and reads web assets. This module will be implemented when the card module is available.
- **`DeviceController`:** keeps future device-control logic separate from networking and HTTP handling.

## Development Plan

1. **Setup:** Document the project and keep the repository organized.
2. **Firmware skeleton:** Create the sketch and configuration file, then confirm the project structure.
3. **Wireless interface:** Implement and validate the local Wi-Fi access point.
4. **Web server:** Serve a basic page and a status endpoint before adding microSD support.
5. **MicroSD storage:** Read `index.html`, `styles.css`, and `app.js` from the card.
6. **USB validation:** Check which USB HID capabilities the board supports and run a local test that is visible and initiated by a physical action.
7. **Integration and documentation:** Validate security boundaries and record results, limitations, and reproducible testing steps.

## Security Principles

- Test only on your own devices or with explicit authorization.
- Do not perform hidden, persistent, or non-consensual input actions.
- Do not collect or transmit credentials or private data.
- Keep tests limited in scope, visible, and easy to stop.
- Do not expose the administration interface to external networks without a reviewed security design.

## Development Environment

Firmware will be developed using the Arduino IDE. The wireless functionality will use the `WiFiS3` library supplied by the Arduino UNO R4 board package. Library and board-core versions will be documented after the first local compile.

## Current Status

- [x] Git repository initialized.
- [x] README translated to English.
- [ ] Firmware skeleton created.
- [ ] Wi-Fi access point validated on the board.
- [ ] Local HTTP server validated.
- [ ] microSD integration validated.
- [ ] USB HID functionality validated on the board.
- [ ] Hardware tests and wiring diagrams added.

## License

The project license has not yet been defined.
