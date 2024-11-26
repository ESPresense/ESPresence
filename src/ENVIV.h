#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace ENVIV
{
    void ConnectToWifi();
    void SerialReport();
    bool SendDiscovery();
    void Setup();
    void Loop();
}

#endif
