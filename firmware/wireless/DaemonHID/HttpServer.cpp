
#include "HttpServer.h"
#include "AppConfig.h"

namespace {
    constexpr unsigned long CLIENT_TIMEOUT_MS = 1000;

    void sendResponse(WiFiClient& client,
                      const char* status,
                      const char* contentType,
                      const char* body) {
        client.print("HTTP/1.1 ");
        client.println(status);

        client.print("Content-Type: ");
        client.println(contentType);

        client.println("Connection: close");
        client.println("Cache-Control: no-store");
        client.println();

        client.print(body);
    }

    const char HTML_PAGE[] =
        "<!DOCTYPE html>"
        "<html lang='es'>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>DaemonHID</title>"
        "</head>"
        "<body>"
        "<h1>DaemonHID</h1>"
        "<p>Servidor web funcionando.</p>"
        "</body>"
        "</html>";
}

HttpServer::HttpServer()
    : server(AppConfig::HTTP_PORT) {
}

bool HttpServer::begin() {
    server.begin();

    // WiFiServer::begin() no devuelve un estado de error.
    // La inicialización se solicita aquí, pero no se verifica
    // de forma independiente el estado del servidor.
    return true;
}

void HttpServer::handleClient() {
    WiFiClient client = server.available();

    if (!client) {
        return;
    }

    // Esperar por una solicitud, pero nunca indefinidamente.
    const unsigned long startTime = millis();

    while (client.connected() &&
           !client.available() &&
           millis() - startTime < CLIENT_TIMEOUT_MS) {
        delay(1);
    }

    if (!client.available()) {
        client.stop();
        return;
    }

    client.setTimeout(CLIENT_TIMEOUT_MS);

    // Leer la primera línea de la solicitud HTTP.
    String requestLine = client.readStringUntil('\n');
    requestLine.trim();

    // Validar que sea una solicitud GET.
    if (!requestLine.startsWith("GET ") ||
        requestLine.indexOf(" HTTP/") == -1) {

        sendResponse(
            client,
            "400 Bad Request",
            "text/plain; charset=utf-8",
            "Solicitud HTTP no valida."
        );

        client.stop();
        return;
    }

    // Servir la página de estado.
    sendResponse(
        client,
        "200 OK",
        "text/html; charset=utf-8",
        HTML_PAGE
    );

    client.stop();
}
