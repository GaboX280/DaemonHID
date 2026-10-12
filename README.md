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
- Additional components from the starter kit, depending on each test.

HID functionality depends on the hardware, Arduino core, and compatible libraries. Support for a feature must not be assumed until it has been verified on the actual board.

## Repository Structure

```text
DaemonHID/
├── README.md
├── .gitignore
├── firmware/
│   ├── usb_hid/
│   └── wireless/
├── hardware/
├── docs/
└── tests/
```

Directories will be added as needed; Git does not track empty directories.

## Development Plan

1. **Setup:** Document the project and keep the repository organized.
2. **USB validation:** Check which USB HID capabilities the board supports and run a local test that is visible and initiated by a physical action.
3. **Controlled input:** Implement a limited, documented demonstration that requires explicit activation.
4. **Wireless interface:** Develop and test the local web interface independently.
5. **Integration:** Define how the features communicate and validate the security boundaries.
6. **Documentation:** Record setup details, results, limitations, and reproducible testing steps.

## Security Principles

- Test only on your own devices or with explicit authorization.
- Do not perform hidden, persistent, or non-consensual input actions.
- Do not collect or transmit credentials or private data.
- Keep tests limited in scope, visible, and easy to stop.
- Do not expose the administration interface to external networks without a reviewed security design.

## Development Environment

Firmware will be developed using the Arduino IDE. Before adding libraries or finalizing the architecture, compatibility with the Arduino UNO R4 WiFi will be verified and the versions in use will be documented.

## Current Status

- [x] Git repository initialized.
- [x] Initial README created.
- [ ] USB HID functionality validated on the board.
- [ ] First firmware test documented.
- [ ] Local web interface documented.
- [ ] Hardware tests and wiring diagrams added.

## License

The project license has not yet been defined.
