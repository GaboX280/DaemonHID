
#include "HttpServer.h"
#include "AppConfig.h"

HttpServer::HttpServer()
    : server(AppConfig::HTTP_PORT) {
}

bool HttpServer::begin() {
    server.begin();
    return true;
}

void HttpServer::handleClient() {
    WiFiClient client = server.available();

    if (!client) {
        return;
    }

    while (client.connected() && !client.available()) {
        delay(1);
    }

    if (client.available()) {
        client.readStringUntil('\r');

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html; charset=utf-8");
        client.println("Connection: close");
        client.println();

        client.println("<!DOCTYPE html>");
        client.println("<html lang='es'>");
        client.println("<head>");
        client.println("<meta charset='UTF-8'>");
        client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
        client.println("<title>DaemonHID</title>");
        client.println("</head>");
        client.println("<body>");
        client.println("<h1>DaemonHID</h1>");
        client.println("<p>Servidor web funcionando.</p>");
        client.println("</body>");
        client.println("</html>");
    }

    client.stop();
}
