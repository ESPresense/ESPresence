#ifdef SENSORS
#include "ENVIV.h"
#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include "string_utils.h"

#include <M5UnitENV.h>

namespace ENVIV
{
    BMP280 bmp;
    SHT4X sht;
    //long BMP280_status;
    String ENVIVBMP280_I2c;
    int ENVIVBMP280_I2c_Bus;  
    unsigned long BMP280PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;
    bool initializedsht = false;
    
    void Setup() {
        //Serial.println("starting env IV setup");
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        //Serial.println("bus check complete");

        if (!bmp.begin(&Wire, BMP280_I2C_ADDR, 2, 1, 400000U)) {
            Serial.println("[ENVIV BMP280] Couldn't find a sensor, check your wiring and I2C address!");
            initialized = false;
            // Optionally, you can proceed without BMP280 or attempt re-initialization later
        } else {
            initialized = true;
        }

        /* Default settings from datasheet. */
        bmp.setSampling(BMP280::MODE_NORMAL,     /* Operating Mode. */
                        BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        BMP280::FILTER_X16,      /* Filtering. */
                        BMP280::STANDBY_MS_500); /* Standby time. */

        if (!sht.begin(&Wire, SHT40_I2C_ADDR_44, 2, 1, 400000U)) {
            Serial.println("[ENVIV SHT40]  Couldn't find SHT40, check your wiring and I2C address!");
            initializedsht = false;
            // Optionally, proceed without SHT40 or attempt re-initialization later
        } else {
            initializedsht = true;
        }
            initializedsht = true;
        }

        sht.setPrecision(SHT4X_HIGH_PRECISION);
        sht.setHeater(SHT4X_NO_HEATER);
    }

    void ConnectToWifi()
    {
        //Serial.println("env IV connect to wifi");
        ENVIVBMP280_I2c_Bus = AsyncWiFiSettings.integer("ENVIVBMP280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        ENVIVBMP280_I2c = AsyncWiFiSettings.string("ENVIVBMP280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        //if (BMP280m5_I2c.isEmpty()) return;
        Serial.print("ENVIV BMP280:       ");
        Serial.println(ENVIVBMP280_I2c + " on bus " + ENVIVBMP280_I2c_Bus);
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;
        if (!initializedsht) return;

        if (BMP280PreviousMillis == 0 || millis() - BMP280PreviousMillis >= sensorInterval) {

            bmp.takeForcedMeasurement();
            bmp.readTemperature();
            bmp.readPressure(); 

            sht.update();

            pub((roomsTopic + "/enviv_temperature").c_str(), 0, true, String(bmp.cTemp).c_str());
            pub((roomsTopic + "/enviv_pressure").c_str(), 0, true, String(bmp.pressure / 100).c_str());
            pub((roomsTopic + "/sht40_temperature").c_str(), 0, true, String(sht.cTemp).c_str());
            pub((roomsTopic + "/sht40_humidity").c_str(), 0, true, String(sht.humidity).c_str());
            
            BMP280PreviousMillis = millis();
        }
    }
    
    bool SendDiscovery()
    {
        //Serial.println("env IV send discovery");
        if (ENVIVBMP280_I2c.isEmpty()) return true;
        return sendSensorDiscovery("ENVIV Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("ENVIV Pressure", EC_NONE, "pressure", "hPa")
            && sendSensorDiscovery("SHT40 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("SHT40 Humidity", EC_NONE, "humidity", "%");
    }

}

#endif
