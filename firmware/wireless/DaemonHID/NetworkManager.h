
#ifndef DAEMONHID_NETWORK_MANAGER_H
#define DAEMONHID_NETWORK_MANAGER_H

#include <Arduino.h>

/**
 * @brief Gestiona la conexion Wi-Fi del dispositivo.
 *
 * Se encarga de iniciar y comprobar el punto de acceso
 * utilizado por DaemonHID.
 */
class NetworkManager {
public:
    /**
     * @brief Inicia el punto de acceso Wi-Fi.
     * @return true si el punto de acceso se inicia correctamente.
     * @return false si ocurre un error durante la inicializacion.
     */
    bool beginAccessPoint();
};

#endif
