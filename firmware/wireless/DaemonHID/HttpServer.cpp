#include "HttpServer.h"
#include "AppConfig.h"
#include "StorageManager.h"
#include "HidManager.h"

#if defined(ARDUINO_ARCH_RENESAS)
extern "C" char* sbrk(int incr);
int HttpServer::getFreeRam() {
    char top;
    return &top - reinterpret_cast<char*>(sbrk(0));
}
#else
int HttpServer::getFreeRam() {
    return 0;
}
#endif

namespace {
    constexpr unsigned long CLIENT_TIMEOUT_MS = 1000;

    const char FALLBACK_HTML[] =
        "<!DOCTYPE html>"
        "<html lang='es'>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width,initial-scale=1.0'>"
        "<title>DaemonHID - Alerta</title>"
        "<style>"
        "body{background:#0d1117;color:#c9d1d9;font-family:system-ui,sans-serif;padding:2rem;text-align:center;margin:0}"
        ".card{background:#161b22;border:1px solid #30363d;border-radius:12px;padding:2rem;max-width:440px;margin:2rem auto;box-shadow:0 8px 24px rgba(0,0,0,0.5)}"
        "h1{color:#58a6ff;margin-top:0;font-size:1.6rem}"
        ".badge{background:#f85149;color:#fff;padding:0.3rem 0.6rem;border-radius:20px;font-size:0.8rem;font-weight:bold;text-transform:uppercase}"
        "p{line-height:1.6;font-size:0.95rem;color:#8b949e}"
        "code{background:#21262d;padding:0.2rem 0.4rem;border-radius:4px;color:#79c0ff;font-size:0.9rem}"
        "</style>"
        "</head>"
        "<body>"
        "<div class='card'>"
        "<h1>DaemonHID</h1>"
        "<p><span class='badge'>MicroSD No Lista</span></p>"
        "<p>El punto de acceso y el servidor web estan activos, pero no se encontro la tarjeta MicroSD o la carpeta <code>/www/</code>.</p>"
        "<p>Para acceder al panel interactivo, inserte una MicroSD formateada en FAT32 con los archivos web en la carpeta <code>/www/</code>.</p>"
        "</div>"
        "</body>"
        "</html>";
}

HttpServer::HttpServer()
    : server(AppConfig::HTTP_PORT), storageMgr(nullptr), hidMgr(nullptr) {
}

bool HttpServer::begin(StorageManager* storage, HidManager* hid) {
    storageMgr = storage;
    hidMgr = hid;
    server.begin();
    return true;
}

void HttpServer::sendTextResponse(WiFiClient& client,
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

void HttpServer::sendJsonResponse(WiFiClient& client,
                                  const char* status,
                                  const String& jsonBody) {
    client.print("HTTP/1.1 ");
    client.println(status);
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Connection: close");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Cache-Control: no-store");
    client.println();
    client.print(jsonBody);
}

void HttpServer::sendFallbackPage(WiFiClient& client) {
    sendTextResponse(client, "200 OK", "text/html; charset=utf-8", FALLBACK_HTML);
}

void HttpServer::handleApiStatus(WiFiClient& client) {
    String json = "{";
    json += "\"system\":\"DaemonHID\",";
    json += "\"version\":\"";
    json += AppConfig::FIRMWARE_VERSION;
    json += "\",";
    json += "\"uptime_s\":";
    json += String(millis() / 1000);
    json += ",";
    json += "\"sd_ready\":";
    json += (storageMgr && storageMgr->isReady()) ? "true" : "false";
    json += ",";
    json += "\"free_ram\":";
    json += String(getFreeRam());
    json += ",";
    json += "\"hid_state\":\"";
    json += (hidMgr) ? hidMgr->getStateString() : "desactivado";
    json += "\",";
    json += "\"countdown\":";
    json += (hidMgr) ? String(hidMgr->getCountdownRemaining()) : "0";
    json += ",";
    json += "\"ip\":\"";
    json += WiFi.localIP().toString();
    json += "\"";
    json += "}";

    sendJsonResponse(client, "200 OK", json);
}

void HttpServer::handleApiPayloads(WiFiClient& client) {
    String json = "{\"payloads\":";
    if (storageMgr && storageMgr->isReady()) {
        json += storageMgr->getPayloadsAsJson("/payloads");
    } else {
        json += "[]";
    }
    json += "}";
    sendJsonResponse(client, "200 OK", json);
}

void HttpServer::handleApiExecute(WiFiClient& client, const String& uri) {
    const int idx = uri.indexOf("file=");
    if (idx == -1) {
        sendJsonResponse(client, "400 Bad Request", "{\"success\":false,\"message\":\"Falta parametro 'file'\"}");
        return;
    }

    String filename = uri.substring(idx + 5);
    const int andIdx = filename.indexOf('&');
    if (andIdx != -1) {
        filename = filename.substring(0, andIdx);
    }

    if (!hidMgr) {
        sendJsonResponse(client, "503 Service Unavailable", "{\"success\":false,\"message\":\"Modulo HID no disponible\"}");
        return;
    }

    if (hidMgr->isPending()) {
        sendJsonResponse(client, "409 Conflict", "{\"success\":false,\"message\":\"Existe otra ejecucion en curso\"}");
        return;
    }

    const bool scheduled = hidMgr->requestPayloadFile(filename.c_str());
    if (scheduled) {
        String resp = "{\"success\":true,\"message\":\"Payload programado en 5 segundos\",\"file\":\"";
        resp += filename;
        resp += "\"}";
        sendJsonResponse(client, "200 OK", resp);
    } else {
        sendJsonResponse(client, "500 Internal Error", "{\"success\":false,\"message\":\"No se pudo programar el payload\"}");
    }
}

void HttpServer::handleApiAbort(WiFiClient& client) {
    if (hidMgr) {
        hidMgr->abort();
    }
    sendJsonResponse(client, "200 OK", "{\"success\":true,\"message\":\"Ejecucion cancelada exitosamente\"}");
}

void HttpServer::handleStaticFile(WiFiClient& client, const String& uri) {
    String path = uri;
    if (path == "/" || path == "") {
        path = "/index.html";
    }

    const String fullPath = "/www" + path;

    if (storageMgr && storageMgr->isReady() && storageMgr->exists(fullPath.c_str())) {
        client.println("HTTP/1.1 200 OK");
        client.print("Content-Type: ");
        client.println(StorageManager::getMimeType(fullPath.c_str()));
        client.println("Connection: close");
        client.println("Cache-Control: max-age=3600");
        client.println();

        storageMgr->streamFileToClient(fullPath.c_str(), client);
        return;
    }

    if (storageMgr && storageMgr->isReady()) {
        sendTextResponse(client, "404 Not Found", "text/plain; charset=utf-8", "Archivo no encontrado en la tarjeta MicroSD.");
        return;
    }

    // Si la MicroSD no está lista, servir la página de fallback
    sendFallbackPage(client);
}

void HttpServer::handleClient() {
    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    // Esperar datos con limite de tiempo
    const unsigned long startTime = millis();
    while (client.connected() && !client.available() && millis() - startTime < CLIENT_TIMEOUT_MS) {
        delay(1);
    }

    if (!client.available()) {
        client.stop();
        return;
    }

    client.setTimeout(CLIENT_TIMEOUT_MS);

    // Leer la primera linea de la solicitud HTTP
    String requestLine = client.readStringUntil('\n');
    requestLine.trim();

    // Drenar el resto de los encabezados HTTP para evitar resets de conexion
    while (client.available()) {
        String header = client.readStringUntil('\n');
        if (header == "\r" || header.length() == 0) {
            break;
        }
    }

    const int firstSpace = requestLine.indexOf(' ');
    const int secondSpace = requestLine.indexOf(' ', firstSpace + 1);

    if (firstSpace == -1 || secondSpace == -1) {
        sendTextResponse(client, "400 Bad Request", "text/plain; charset=utf-8", "Solicitud HTTP no valida.");
        client.stop();
        return;
    }

    const String method = requestLine.substring(0, firstSpace);
    const String uri = requestLine.substring(firstSpace + 1, secondSpace);

    // Enrutamiento de la API REST
    if (uri == "/api/status") {
        handleApiStatus(client);
    } else if (uri == "/api/payloads") {
        handleApiPayloads(client);
    } else if (uri.startsWith("/api/execute")) {
        handleApiExecute(client, uri);
    } else if (uri.startsWith("/api/abort")) {
        handleApiAbort(client);
    } else {
        handleStaticFile(client, uri);
    }

    client.stop();
}
