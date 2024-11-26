#pragma once
#include <Arduino.h>

namespace EnergyMonitor {
    enum DeviceType {
        DEVICE_NONE = 0,
        DEVICE_SWITCHBOT_PLUG = 1,
        DEVICE_ATHOM_PLUG = 2,
        // Add more devices as needed
        DEVICE_COUNT
    };

    struct device_config {
        const char* name;           // Device marketing name
        uint8_t sel_pin;           // SEL pin for HLW8012/BL0937
        bool sel_inverted;         // SEL pin inverted?
        uint8_t cf_pin;            // CF pin (power)
        uint8_t cf1_pin;           // CF1 pin (voltage/current)
        int voltage_divider;       // Voltage divider value
        int current_multiplier;    // Current multiplier
        int power_multiplier;      // Power multiplier
        bool use_cse7766;         // true = CSE7766, false = HLW8012/BL0937
    };

    extern const device_config deviceConfigs[];

    void ConnectToWifi();
    void Setup();
    void Loop();
    void SerialReport();
    bool SendDiscovery();
    bool SendOnline();
    bool Command(String& command, String& pay);

    // Sensor readings
    float getPower();
    float getVoltage();
    float getCurrent();
    float getEnergy();
    float getApparentPower();
    float getReactivePower();
    float getPowerFactor();

    bool isConnected();
}
