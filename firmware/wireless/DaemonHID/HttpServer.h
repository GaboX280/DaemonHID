#ifndef DAEMONHID_HTTP_SERVER_H
#define DAEMONHID_HTTP_SERVER_H

#include <Arduino.h>
#include <WiFiS3.h>

class HttpServer {
public:
    HttpServer();
    bool begin();
    void handleClient();

private:
    WiFiServer server;
};

#endif