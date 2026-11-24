#ifndef DAEMONHID_HTTP_SERVER_H
#define DAEMONHID_HTTP_SERVER_H

#include <Arduino.h>
#include <WiFiS3.h>

class StorageManager;
class HidManager;

/**
 * @brief Servidor HTTP que sirve el frontend desde la MicroSD y expone una API REST.
 */
class HttpServer {
public:
    HttpServer();
    bool begin(StorageManager* storage = nullptr, HidManager* hid = nullptr);
    void handleClient();

private:
    WiFiServer server;
    StorageManager* storageMgr = nullptr;
    HidManager* hidMgr = nullptr;

    void handleApiStatus(WiFiClient& client);
    void handleApiPayloads(WiFiClient& client);
    void handleApiFilesystem(WiFiClient& client);
    void handleApiExecute(WiFiClient& client, const String& uri);
    void handleApiAbort(WiFiClient& client);
    void handleStaticFile(WiFiClient& client, const String& uri, const String& method);
    void sendFallbackPage(WiFiClient& client);
    void sendJsonResponse(WiFiClient& client, const char* status, const String& jsonBody);
    void sendTextResponse(WiFiClient& client, const char* status, const char* contentType, const char* body);

    static int getFreeRam();
};

#endif
