
#ifndef DAEMONHID_APP_CONFIG_H
#define DAEMONHID_APP_CONFIG_H

#include <stdint.h>

namespace AppConfig {

    constexpr char FIRMWARE_VERSION[] = "0.1.0";

    constexpr uint16_t HTTP_PORT = 80;

    constexpr uint8_t WIFI_CHANNEL = 6;
    constexpr char AP_SSID[] = "DaemonHID";
    constexpr char AP_PASSWORD[] = "DmnHID_7x29!p";

}

#endif