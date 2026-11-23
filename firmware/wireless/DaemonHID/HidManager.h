#ifndef DAEMONHID_HID_MANAGER_H
#define DAEMONHID_HID_MANAGER_H

#include <Arduino.h>
#include <Keyboard.h>
#include <SD.h>

class StorageManager;

/**
 * @brief Gestiona las demostraciones y ejecucion de scripts USB HID.
 *
 * Soporta demos integradas y ejecucion asincrona no bloqueante
 * de archivos de script desde la tarjeta MicroSD.
 */
class HidManager {
public:
    void begin(StorageManager* storage = nullptr);
    void setStorageManager(StorageManager* storage);

    bool requestDemoText();
    bool requestPingDemo();
    bool requestPayloadFile(const char* filename);
    void abort();
    void update();

    bool isPending() const;
    bool isExecuting() const;
    const char* getStateString() const;
    int getCountdownRemaining() const;
    const char* getActivePayloadName() const;

private:
    enum class DemoType {
        None,
        Text,
        Ping,
        FilePayload
    };

    enum class DemoStep {
        None,
        ReleaseRun,
        LaunchCmd,
        SendPing,
        ExecutingFile
    };

    bool initialized = false;
    bool pending = false;
    bool executingFile = false;

    uint32_t sendAt = 0;

    DemoType currentDemo = DemoType::None;
    DemoStep step = DemoStep::None;
    char targetPayloadFile[32] = "";

    StorageManager* storageMgr = nullptr;
    File activeFile;

    void processScriptLine(const String& line);
    void endScript();
};

#endif
