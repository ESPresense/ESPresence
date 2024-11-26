#include "EnergyMonitor.h"
#include <AsyncWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

#define USE_ENERGY_SENSOR 1
#define USE_HLW8012 1
#define USE_CSE7766 1
#include "hlw8012.h"
#include "cse7766.h"

namespace EnergyMonitor {

const device_config deviceConfigs[] = {
    // DEVICE_NONE
    {
        "None",                 // name
        0,                      // sel_pin
        false,                  // sel_inverted
        0,                      // cf_pin
        0,                      // cf1_pin
        0,                      // voltage_divider
        0,                      // current_multiplier
        0,                      // power_multiplier
        false                   // use_cse7766
    },
    // DEVICE_SWITCHBOT_PLUG
    {
        "SwitchBot Plug Mini",  // name
        20,                     // sel_pin
        true,                   // sel_inverted
        18,                     // cf_pin
        19,                     // cf1_pin
        1467,                   // voltage_divider
        1,                      // current_multiplier
        1,                      // power_multiplier
        false                   // use_cse7766
    },
    // DEVICE_ATHOM_PLUG
    {
        "Athom Smart Plug",     // name
        0,                      // sel_pin
        false,                  // sel_inverted
        3,                      // cf_pin (RX)
        1,                      // cf1_pin (TX)
        0,                      // voltage_divider (internal)
        0,                      // current_multiplier (internal)
        0,                      // power_multiplier (internal)
        true                    // use_cse7766
    }
};

int deviceType = 0;
float power = 0;
float voltage = 0;
float current = 0;
float energy = 0;
float apparentPower = 0;
float reactivePower = 0;
float powerFactor = 0;

void ConnectToWifi() {
    AsyncWiFiSettings.heading("<a href='https://espresense.com/configuration/settings#energy' target='_blank'>Power Monitoring</a>", false);

    std::vector<String> deviceTypes;
    for(int i = 0; i < DEVICE_COUNT; i++) {
        deviceTypes.push_back(deviceConfigs[i].name);
    }

    deviceType = AsyncWiFiSettings.dropdown("energy_device", deviceTypes, 0, "Power Monitoring Device");
}

void Setup() {
    if (deviceType == DEVICE_NONE) return;

    const device_config& config = deviceConfigs[deviceType];

    if (!config.use_cse7766) {
        // HLW8012/BL0937 setup
        if (config.sel_pin > 0) {
            pinMode(config.sel_pin, OUTPUT);
            digitalWrite(config.sel_pin, config.sel_inverted);
        }
        if (config.cf_pin > 0) {
            pinMode(config.cf_pin, INPUT_PULLUP);
        }
        if (config.cf1_pin > 0) {
            pinMode(config.cf1_pin, INPUT_PULLUP);
        }
    } else {
        // CSE7766 setup
        Serial1.begin(4800, SERIAL_8E1, config.cf_pin, config.cf1_pin);
    }
}

void Loop() {
    if (deviceType == DEVICE_NONE) return;

    const device_config& config = deviceConfigs[deviceType];

    if (!config.use_cse7766) {
        // HLW8012/BL0937 readings
        // TODO: Implement readings using interrupts
    } else {
        // CSE7766 readings
        while (Serial1.available()) {
            // TODO: Implement CSE7766 protocol parsing
        }
    }
}

void SerialReport() {
    if (deviceType == DEVICE_NONE) return;
    Serial.printf("Power Monitor: %s\n", deviceConfigs[deviceType].name);
}

bool SendDiscovery() {
    if (deviceType == DEVICE_NONE) return true;

    bool success = sendSensorDiscovery("Power", EC_NONE, "power", "W") &&
                  sendSensorDiscovery("Voltage", EC_NONE, "voltage", "V") &&
                  sendSensorDiscovery("Current", EC_NONE, "current", "A") &&
                  sendSensorDiscovery("Energy", EC_NONE, "energy", "kWh");

    if (deviceConfigs[deviceType].use_cse7766) {
        success = success &&
                 sendSensorDiscovery("Apparent Power", EC_NONE, "apparent_power", "VA") &&
                 sendSensorDiscovery("Reactive Power", EC_NONE, "reactive_power", "VAR") &&
                 sendSensorDiscovery("Power Factor", EC_NONE, "power_factor", "%");
    }

    return success;
}

bool SendOnline() {
    if (deviceType == DEVICE_NONE) return true;

    bool success = pub((roomsTopic + "/power").c_str(), 0, true, String(power).c_str()) &&
                  pub((roomsTopic + "/voltage").c_str(), 0, true, String(voltage).c_str()) &&
                  pub((roomsTopic + "/current").c_str(), 0, true, String(current).c_str()) &&
                  pub((roomsTopic + "/energy").c_str(), 0, true, String(energy).c_str());

    if (deviceConfigs[deviceType].use_cse7766) {
        success = success &&
                 pub((roomsTopic + "/apparent_power").c_str(), 0, true, String(apparentPower).c_str()) &&
                 pub((roomsTopic + "/reactive_power").c_str(), 0, true, String(reactivePower).c_str()) &&
                 pub((roomsTopic + "/power_factor").c_str(), 0, true, String(powerFactor).c_str());
    }

    return success;
}

bool Command(String& command, String& pay) {
    return false;
}

float getPower() { return power; }
float getVoltage() { return voltage; }
float getCurrent() { return current; }
float getEnergy() { return energy; }
float getApparentPower() { return apparentPower; }
float getReactivePower() { return reactivePower; }
float getPowerFactor() { return powerFactor; }
bool isConnected() { return deviceType != DEVICE_NONE; }

}  // namespace EnergyMonitor
