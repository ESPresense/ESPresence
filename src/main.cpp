#include "main.h"

#include "MotionSensors.h"

bool sendTelemetry(int totalSeen, int totalFpSeen, int totalFpQueried, int totalFpReported, int count)
{
    if (!online)
    {
        if (sendOnline())
        {
            online = true;
            reconnectTries = 0;
        }
        else
        {
            log_e("Error sending status=online");
        }
    }

    if (discovery && !sentDiscovery)
    {
        if (sendDiscoveryConnectivity()
            && sendTeleSensorDiscovery("Uptime", EC_DIAGNOSTIC, "{{ value_json.uptime }}", "s")
            && sendTeleSensorDiscovery("Free Mem", EC_DIAGNOSTIC, "{{ value_json.freeHeap }}", "bytes")
            && (BleFingerprintCollection::countIds.isEmpty() ? sendDeleteDiscovery("sensor", "Count") : sendTeleSensorDiscovery("Count", "", "{{ value_json.count }}", ""))
            && sendButtonDiscovery("Restart", EC_DIAGNOSTIC)
            && sendSwitchDiscovery("Status LED", EC_CONFIG)
            && sendNumberDiscovery("Max Distance", EC_CONFIG)
            && sendNumberDiscovery("Absorption", EC_CONFIG)
            && sendSwitchDiscovery("Active Scan", EC_CONFIG)
            && sendSwitchDiscovery("Auto Update", EC_CONFIG)
            && sendSwitchDiscovery("Arduino OTA", EC_CONFIG)
            && sendSwitchDiscovery("Prerelease", EC_CONFIG)
            && sendDeleteDiscovery("switch", "OTA Update")
            && Motion::SendDiscovery(doc)
#ifdef MACCHINA_A0
            && sendTeleSensorDiscovery("Battery", "", "{{ value_json.batt }}", "%")
            && sendTeleBinarySensorDiscovery("Running", "", "{{ value_json.run }}", "running")
#endif
#ifdef SENSORS
            && sendDiscoveryHumidity()
            && sendDiscoveryTemperature()
            && sendDiscoveryLux()
            && BME280::SendDiscovery(doc)
            && TSL2561::SendDiscovery(doc)
#endif
        )
        {
            sentDiscovery = true;
        }
        else
        {
            log_e("Error sending discovery");
        }
    }

    auto now = millis();

    if (now - lastTeleMillis < 15000)
        return false;

    lastTeleMillis = now;

    doc.clear();
    doc["ip"] = localIp;
    doc["uptime"] = getUptimeSeconds();
#ifdef FIRMWARE
    doc["firm"] = String(FIRMWARE);
#endif
    doc["rssi"] = WiFi.RSSI();
#ifdef MACCHINA_A0
    auto mv = a0_read_batt_mv();
    doc["mV"] = mv;
    bool run = (mv > 13200);
    unsigned int soc = round(-13275.04 + 2.049731 * mv - (0.00007847975 * mv) * mv);
    doc["batt"] = run ? (unsigned int)100 : max((unsigned int)0, min((unsigned int)100, soc));
    doc["run"] = run ? "ON" : "OFF";
#endif
#ifdef VERSION
    doc["ver"] = String(VERSION);
#endif

    if (!BleFingerprintCollection::countIds.isEmpty())
        doc["count"] = count;
    if (totalSeen > 0)
        doc["adverts"] = totalSeen;
    if (totalFpSeen > 0)
        doc["seen"] = totalFpSeen;
    if (totalFpQueried > 0)
        doc["queried"] = totalFpQueried;
    if (totalFpReported > 0)
        doc["reported"] = totalFpReported;

    if (teleFails > 0)
        doc["teleFails"] = teleFails;
    if (reconnectTries > 0)
        doc["reconnectTries"] = reconnectTries;

    doc["freeHeap"] = ESP.getFreeHeap();
    doc["maxAllocHeap"] = ESP.getMaxAllocHeap();
    doc["memFrag"] = 100 - (ESP.getMaxAllocHeap() * 100.0 / ESP.getFreeHeap());
    doc["resetReason"] = resetReason(rtc_get_reset_reason(0));

    char teleMessageBuffer[512];
    serializeJson(doc, teleMessageBuffer);

    for (int i = 0; i < 10; i++)
    {
        if (!publishTele || mqttClient.publish(teleTopic.c_str(), 0, false, teleMessageBuffer))
            return true;
        delay(50);
    }

    teleFails++;
    log_e("Error after 10 tries sending telemetry (%d times since boot)", teleFails);
    return false;
}

void connectToWifi()
{
    Serial.printf("Connecting to WiFi (%s)...\n", WiFi.macAddress().c_str());
    GUI::connected(false, false);

    WiFiSettings.onFailure = []()
    {
        GUI::status("WiFi Portal...");
    };
    WiFiSettings.onWaitLoop = []()
    {
        GUI::connecting();
        return 150;
    };
    WiFiSettings.onPortalWaitLoop = []()
    {
        if (getUptimeSeconds() > 600)
            ESP.restart();
    };

    GUI::connected(true, false);
#ifdef VERSION
    WiFiSettings.info("ESPresense Version: " + String(VERSION));
#endif
    room = WiFiSettings.string("room", ESPMAC, "Room");

    WiFiSettings.heading("MQTT <a href='https://espresense.com/configuration/settings#mqtt' target='_blank'>ℹ️</a>", false);
    mqttHost = WiFiSettings.string("mqtt_host", DEFAULT_MQTT_HOST, "Server");
    mqttPort = WiFiSettings.integer("mqtt_port", DEFAULT_MQTT_PORT, "Port");
    mqttUser = WiFiSettings.string("mqtt_user", DEFAULT_MQTT_USER, "Username");
    mqttPass = WiFiSettings.string("mqtt_pass", DEFAULT_MQTT_PASSWORD, "Password");
    discovery = WiFiSettings.checkbox("discovery", true, "Send to discovery topic");
    publishTele = WiFiSettings.checkbox("pub_tele", true, "Send to telemetry topic");
    publishRooms = WiFiSettings.checkbox("pub_rooms", true, "Send to rooms topic");
    publishDevices = WiFiSettings.checkbox("pub_devices", true, "Send to devices topic");

    WiFiSettings.heading("Updating <a href='https://espresense.com/configuration/settings#updating' target='_blank'>ℹ️</a>", false);
    autoUpdate = WiFiSettings.checkbox("auto_update", DEFAULT_AUTO_UPDATE, "Automatically update");
    prerelease = WiFiSettings.checkbox("prerelease", false, "Include pre-released versions in auto-update");
    arduinoOta = WiFiSettings.checkbox("arduino_ota", DEFAULT_ARDUINO_OTA, "Arduino OTA Update");

    WiFiSettings.heading("Scanning <a href='https://espresense.com/configuration/settings#scanning' target='_blank'>ℹ️</a>", false);
    activeScan = WiFiSettings.checkbox("active_scan", false, "Request scan results (usually not needed)");
    BleFingerprintCollection::knownMacs = WiFiSettings.string("known_macs", "", "Known BLE mac addresses (no colons, space seperated)");
    BleFingerprintCollection::query = WiFiSettings.string("query", DEFAULT_QUERY, "Query device ids for characteristics (eg. apple:1005:9-26)");

    WiFiSettings.heading("Counting <a href='https://espresense.com/configuration/settings#counting' target='_blank'>ℹ️</a>", false);
    BleFingerprintCollection::countIds = WiFiSettings.string("count_ids", "", "Include device ids (space seperated ids)");
    BleFingerprintCollection::countEnter = WiFiSettings.floating("count_enter", 0, 100, 2, "Start counting devices less than distance (in meters)");
    BleFingerprintCollection::countExit = WiFiSettings.floating("count_exit", 0, 100, 4, "Stop counting devices greater than distance (in meters)");
    BleFingerprintCollection::countMs = WiFiSettings.integer("count_ms", 0, 3000000, 30000, "Include devices with age less than (in ms)");

    WiFiSettings.heading("Filtering <a href='https://espresense.com/configuration/settings#filtering' target='_blank'>ℹ️</a>", false);
    if (BleFingerprintCollection::query == "1") BleFingerprintCollection::query = "apple:10"; // This is to keep query=true doing the same thing as older firmwares
    BleFingerprintCollection::include = WiFiSettings.string("include", DEFAULT_INCLUDE, "Include only sending these ids to mqtt (eg. apple:iphone10-6 apple:iphone13-2)");
    BleFingerprintCollection::exclude = WiFiSettings.string("exclude", DEFAULT_EXCLUDE, "Exclude sending these ids to mqtt (eg. exp:20 apple:iphone10-6)");
    BleFingerprintCollection::maxDistance = WiFiSettings.floating("max_dist", 0, 100, DEFAULT_MAX_DISTANCE, "Maximum distance to report (in meters)");
    BleFingerprintCollection::skipDistance = WiFiSettings.floating("skip_dist", 0, 10, DEFAULT_SKIP_DISTANCE, "Report early if beacon has moved more than this distance (in meters)");
    BleFingerprintCollection::skipMs = WiFiSettings.integer("skip_ms", 0, 3000000, DEFAULT_SKIP_MS, "Skip reporting if message age is less that this (in milliseconds)");

    WiFiSettings.heading("Calibration <a href='https://espresense.com/configuration/settings#calibration' target='_blank'>ℹ️</a>", false);
    BleFingerprintCollection::refRssi = WiFiSettings.integer("ref_rssi", -100, 100, DEFAULT_REF_RSSI, "Rssi expected from a 0dBm transmitter at 1 meter");
    BleFingerprintCollection::absorption = WiFiSettings.floating("absorption", -100, 100, DEFAULT_ABSORPTION, "Factor used to account for absorption, reflection, or diffraction");
    BleFingerprintCollection::forgetMs = WiFiSettings.integer("forget_ms", 0, 3000000, DEFAULT_FORGET_MS, "Forget beacon if not seen for (in milliseconds)");

    WiFiSettings.heading("Misc <a href='https://espresense.com/configuration/settings#misc' target='_blank'>ℹ️</a>", false);
    GUI::statusLed = WiFiSettings.checkbox("status_led", true, "Status LED");
    Motion::ConnectToWifi();
#ifdef SENSORS
    dht11Pin = WiFiSettings.integer("dht11_pin", 0, "DHT11 sensor pin (0 for disable)");
    dht22Pin = WiFiSettings.integer("dht22_pin", 0, "DHT22 sensor pin (0 for disable)");
    dhtTempOffset = WiFiSettings.floating("dhtTemp_offset", -40, 125, 0.0, "DHT temperature offset");

    WiFiSettings.heading("I2C Settings <a href='https://espresense.com/configuration/settings#i2c-settings' target='_blank'>ℹ️</a>", false);

    I2CDebug = WiFiSettings.checkbox("I2CDebug", false, "Debug I2C addreses. Look at the serial log to get the correct address");

    WiFiSettings.html("h4", "Bus 1:");
    I2C_Bus_1_SDA = WiFiSettings.integer("I2C_Bus_1_SDA", 0, 39, DEFAULT_I2C_BUS_1_SDA, "SDA pin (0 to disable)");
    I2C_Bus_1_SCL = WiFiSettings.integer("I2C_Bus_1_SCL", 0, 39, DEFAULT_I2C_BUS_1_SCL, "SCL pin (0 to disable)");

    WiFiSettings.html("h4", "Bus 2:");

    I2C_Bus_2_SDA = WiFiSettings.integer("I2C_Bus_2_SDA", 0, "SDA pin (0 to disable)");
    I2C_Bus_2_SCL = WiFiSettings.integer("I2C_Bus_2_SCL", 0, "SCL pin (0 to disable)");

    WiFiSettings.heading("I2C Sensors <a href='https://espresense.com/configuration/settings#i2c-sensors' target='_blank'>ℹ️</a>", false);

    WiFiSettings.html("h4", "BH1750 - Ambient Light Sensor:");
    BH1750_I2c_Bus = WiFiSettings.integer("BH1750_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    BH1750_I2c = WiFiSettings.string("BH1750_I2c", "", "I2C address (0x23 or 0x5C)");

    BME280::ConnectToWifi();

    TSL2561::ConnectToWifi();
#endif

    WiFiSettings.hostname = "espresense-" + kebabify(room);

    if (!WiFiSettings.connect(true, 60))
        ESP.restart();
#ifdef FIRMWARE
    Serial.println("Firmware:     " + String(FIRMWARE));
#endif
#ifdef VERSION
    Serial.println("Version:      " + String(VERSION));
#endif
    Serial.print("IP address:   ");
    Serial.println(WiFi.localIP());
    Serial.print("DNS address:  ");
    Serial.println(WiFi.dnsIP());
    Serial.print("Hostname:     ");
    Serial.println(WiFi.getHostname());
    Serial.print("Room:         ");
    Serial.println(room);
    Serial.printf("MQTT server:  %s:%d\n", mqttHost.c_str(), mqttPort);
    Serial.printf("Max Distance: %.2f\n", BleFingerprintCollection::maxDistance);
    Motion::SerialReport();
#ifdef SENSORS
    Serial.print("DHT11 Sensor: ");
    Serial.println(dht11Pin ? "enabled" : "disabled");
    Serial.print("DHT22 Sensor: ");
    Serial.println(dht22Pin ? "enabled" : "disabled");
    Serial.print("DHT Temp Offset: ");
    Serial.println(dhtTempOffset ? "enabled" : "disabled");
    Serial.print("BH1750_I2c Sensor: ");
    Serial.println(BH1750_I2c + " on bus " + BH1750_I2c_Bus);
    BME280::SerialReport();
    TSL2561::SerialReport();
#endif
    Serial.print("Query:        ");
    Serial.println(BleFingerprintCollection::query);
    Serial.print("Include:      ");
    Serial.println(BleFingerprintCollection::include);
    Serial.print("Exclude:      ");
    Serial.println(BleFingerprintCollection::exclude);
    Serial.print("Known Macs:   ");
    Serial.println(BleFingerprintCollection::knownMacs);
    Serial.print("Count Ids:    ");
    Serial.println(BleFingerprintCollection::countIds);

    localIp = WiFi.localIP().toString();
    id = slugify(room);
    roomsTopic = CHANNEL + "/rooms/" + id;
    statusTopic = roomsTopic + "/status";
    teleTopic = roomsTopic + "/telemetry";
    setTopic = roomsTopic + "/+/set";
}

void onMqttConnect(bool sessionPresent)
{
    xTimerStop(reconnectTimer, 0);
    mqttClient.subscribe("espresense/rooms/*/+/set", 1);
    mqttClient.subscribe(setTopic.c_str(), 1);
    GUI::connected(true, true);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    GUI::connected(true, false);
    log_e("Disconnected from MQTT; reason %d\n", reason);
    xTimerStart(reconnectTimer, 0);
    online = false;
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    char new_payload[len + 1];
    new_payload[len] = '\0';
    strncpy(new_payload, payload, len);
    Serial.printf("%s: %s\n", topic, new_payload);

    String top = String(topic);
    String pay = String(new_payload);

    auto setPos = top.lastIndexOf("/set");
    if (setPos <= 1) return;
    auto commandPos = top.lastIndexOf("/", setPos - 1);
    if (commandPos < 0) return;
    auto command = top.substring(commandPos + 1, setPos);

    if (command == "max_distance")
    {
        BleFingerprintCollection::maxDistance = pay.toFloat();
        spurt("/max_dist", pay);
        online = false;
    }
    else if (command == "absorption")
    {
        BleFingerprintCollection::absorption = pay.toFloat();
        spurt("/absorption", pay);
        online = false;
    }
    else if (command == "active_scan")
    {
        activeScan = pay == "ON";
        spurt("/active_scan", String(activeScan));
        online = false;
    }
    else if (command == "query")
    {
        BleFingerprintCollection::query = pay;
        spurt("/query", pay);
        online = false;
    }
    else if (command == "include")
    {
        BleFingerprintCollection::include = pay;
        spurt("/include", pay);
        online = false;
    }
    else if (command == "exclude")
    {
        BleFingerprintCollection::exclude = pay;
        spurt("/exclude", pay);
        online = false;
    }
    else if (command == "known_macs")
    {
        BleFingerprintCollection::knownMacs = pay;
        spurt("/known_macs", pay);
        online = false;
    }
    else if (command == "count_ids")
    {
        BleFingerprintCollection::countIds = pay;
        spurt("/count_ids", pay);
        online = false;
    }
    else if (command == "status_led")
    {
        GUI::statusLed = pay == "ON";
        spurt("/status_led", String(GUI::statusLed));
        online = false;
    }
    else if (command == "arduino_ota")
    {
        arduinoOta = pay == "ON";
        spurt("/arduino_ota", String(arduinoOta));
        online = false;
    }
    else if (command == "auto_update")
    {
        autoUpdate = pay == "ON";
        spurt("/auto_update", String(autoUpdate));
        online = false;
    }
    else if (command == "prerelease")
    {
        prerelease = pay == "ON";
        spurt("/prerelease", String(prerelease));
        online = false;
    }
    else if (command == "restart")
    {
        ESP.restart();
    }
    else if (command == "dump_memory")
    {
        heap_caps_dump_all();
    }
}

void reconnect(TimerHandle_t xTimer)
{
    Serial.printf("%u Reconnect timer\n", xPortGetCoreID());
    if (updateInProgress) return;
    if (WiFi.isConnected() && mqttClient.connected()) return;

    if (reconnectTries++ > 50)
    {
        log_e("Too many reconnect attempts; Restarting");
        ESP.restart();
    }

    if (!WiFi.isConnected())
    {
        Serial.printf("%u Reconnecting to WiFi...\n", xPortGetCoreID());
        if (!WiFiSettings.connect(true, 60))
            ESP.restart();
    }

    Serial.printf("%u Reconnecting to MQTT...\n", xPortGetCoreID());
    mqttClient.connect();
}

void connectToMqtt()
{
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(3000), pdTRUE, (void *)nullptr, reconnect);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setWill(statusTopic.c_str(), 0, true, offline.c_str());
    mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
    mqttClient.connect();
}

bool reportDevice(BleFingerprint *f)
{
    doc.clear();
    if (!f->report(&doc))
        return false;

    serializeJson(doc, buffer);
    String devicesTopic = CHANNEL + "/devices/" + f->getId() + "/" + id;

    bool p1 = false, p2 = false;
    for (int i = 0; i < 10; i++)
    {
        if (!mqttClient.connected())
            return false;

        if (!p1 && (!publishRooms || mqttClient.publish(roomsTopic.c_str(), 0, false, buffer)))
            p1 = true;

        if (!p2 && (!publishDevices || mqttClient.publish(devicesTopic.c_str(), 0, false, buffer)))
            p2 = true;

        if (p1 && p2)
            return true;
        delay(20);
    }
    teleFails++;
    return false;
}

int totalFpReported = 0;
int totalSeen = 0;
int totalFpSeen = 0;
int totalFpQueried = 0;

void reportTask(void *parameter)
{
    connectToMqtt();

    while (true)
    {
        while (updateInProgress || !mqttClient.connected())
            delay(1000);

        yield();
        auto copy = fingerprints.getCopy();

        int count = 0;
        for (auto i: copy)
            if (i->shouldCount())
                count++;

        yield();
        sendTelemetry(totalSeen, totalFpSeen, totalFpQueried, totalFpReported, count);
        yield();

        auto reported = 0;
        for (auto f : copy)
        {
            auto seen = f->getSeenCount();
            if (seen)
            {
                totalSeen += seen;
                totalFpSeen++;
            }
            if (reportDevice(f))
            {
                totalFpReported++;
                reported++;
            }
            yield();
        }
    }
}

void scanTask(void *parameter)
{
    NimBLEDevice::init("");
    for (esp_ble_power_type_t i = ESP_BLE_PWR_TYPE_CONN_HDL0; i <= ESP_BLE_PWR_TYPE_CONN_HDL8; i = esp_ble_power_type_t((int)i + 1))
        NimBLEDevice::setPower(ESP_PWR_LVL_P9, i);
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityRespKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
    NimBLEDevice::setMTU(255);

    auto pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    pBLEScan->setAdvertisedDeviceCallbacks(&fingerprints, true);
    pBLEScan->setActiveScan(activeScan);
    pBLEScan->setDuplicateFilter(false);
    pBLEScan->setMaxResults(0);
    if (!pBLEScan->start(0, nullptr, false))
        log_e("Error starting continuous ble scan");

    while (true)
    {
        for (auto f : *fingerprints.getNative())
            if (f->query())
                totalFpQueried++;

        if (!pBLEScan->isScanning())
        {
            if (!pBLEScan->start(0, nullptr, true))
                log_e("Error re-starting continuous ble scan");
            delay(3000); // If we stopped scanning, don't query for 3 seconds in order for us to catch any missed broadcasts
        }
        else
        {
            delay(100);
        }
    }
}

#ifdef SENSORS
/**
 * Task to reads temperature from DHT11 sensor
 * @param pvParameters
 *		pointer to task parameters
 */
void tempTask(void *pvParameters)
{
    Serial.println("tempTask loop started");
    while (1) // tempTask loop
    {
        if (dhtTasksEnabled && !gotNewTemperature)
        {
            // Read temperature only if old data was processed already
            // Reading temperature for humidity takes about 250 milliseconds!
            // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
            dhtSensorData = dhtSensor.getTempAndHumidity(); // Read values from sensor 1
            gotNewTemperature = true;
        }
        vTaskSuspend(NULL);
    }
}

/**
 * triggerGetTemp
 * Sets flag dhtUpdated to true for handling in loop()
 * called by Ticker tempTicker
 */
void triggerGetTemp()
{
    if (dhtTempTaskHandle != NULL)
    {
        xTaskResumeFromISR(dhtTempTaskHandle);
    }
}
#endif

void setup()
{
    GUI::setup();

#ifdef FAST_MONITOR
    Serial.begin(1500000);
#else
    Serial.begin(115200);
#endif
    Serial.setDebugOutput(true);

#ifdef VERBOSE
    esp_log_level_set("*", ESP_LOG_DEBUG);
#else
    esp_log_level_set("*", ESP_LOG_ERROR);
#endif

    spiffsInit();
    connectToWifi();
#if NTP
    setClock();
#endif
    Motion::Setup();
#if MACCHINA_A0
    pinMode(GPIO_NUM_35, INPUT);
#endif
#ifdef SENSORS
    if (dht11Pin) dhtSensor.setup(dht11Pin, DHTesp::DHT11);
    if (dht22Pin) dhtSensor.setup(dht22Pin, DHTesp::DHT22); //(AM2302)

    if (dht11Pin || dht22Pin)
    {
        // Start task to get temperature
        xTaskCreatePinnedToCore(
            tempTask,           /* Function to implement the task */
            "tempTask ",        /* Name of the task */
            4000,               /* Stack size in words */
            NULL,               /* Task input parameter */
            5,                  /* Priority of the task */
            &dhtTempTaskHandle, /* Task handle. */
            1);                 /* Core where the task should run */

        if (dhtTempTaskHandle == NULL)
        {
            Serial.println("[ERROR] Failed to start task for temperature update");
        }
        else
        {
            // Start update of environment data every 10 seconds
            tempTicker.attach(dhtUpdateTime, triggerGetTemp);
        }

        // Signal end of setup() to tasks
        dhtTasksEnabled = true;
    }

    if (I2C_Bus_1_SDA != 0 && I2C_Bus_1_SDA != 0) {
        Wire.begin(I2C_Bus_1_SDA, I2C_Bus_1_SCL);
        I2C_Bus_1_Enabled = true;
        Serial.println("\nInitialized I2C Bus 1");
    }

    if (I2C_Bus_2_SDA != 0 && I2C_Bus_2_SDA != 0) {
        Wire1.begin(I2C_Bus_2_SDA, I2C_Bus_2_SCL);
        I2C_Bus_2_Enabled = true;
        Serial.println("\nInitialized I2C Bus 2");
    }

    if (I2CDebug)
    {
        Serial.println("\nI2C Scanner");
    }


    if (I2C_Bus_1_Enabled || I2C_Bus_2_Enabled) {
        // BH1750_I2c
        // BH1750_updateFr
        if (BH1750_I2c == "0x23" || BH1750_I2c == "0x5C")
        {
            // Init BH1750 (witch default l2c adres)
            int rc; // Returncode
            unsigned long m; // milli for calibration
            bool state = false;

            // if (! BH1750.begin(BH1750_TO_GROUND))
            if (BH1750_I2c == "0x23" && BH1750_I2c_Bus == 1)
            {
                state = BH1750.begin(BH1750_TO_GROUND, &Wire);
            }
            else if (BH1750_I2c == "0x5C" && BH1750_I2c_Bus == 1)
            {
                state = BH1750.begin(BH1750_TO_VCC, &Wire);
            }
            else if (BH1750_I2c == "0x23" && BH1750_I2c_Bus == 2)
            {
                state = BH1750.begin(BH1750_TO_GROUND, &Wire1);
            }
            else if (BH1750_I2c == "0x5C" && BH1750_I2c_Bus == 2)
            {
                state = BH1750.begin(BH1750_TO_VCC, &Wire1);
            }

            if (!state)
            {
                Serial.println("Error on initialisation BH1750 GY-302");
            }
            else
            {
                sendDiscoveryLux();
                Serial.println("initialisation BH1750 GY-302 success");
                m = millis();
                rc = BH1750.calibrateTiming();
                m = millis() - m;
                Serial.print("Lux Sensor BH1750 GY-302 calibrated (Time: ");
                Serial.print(m);
                Serial.print(" ms)");
                if (rc != 0)
                {
                    Serial.print(" - Lux Sensor Error code ");
                    Serial.print(rc);
                }
                Serial.println();

                // start first measure and timecount
                lux_BH1750 = -1; // nothing to compare
                BH1750.start(BH1750_QUALITY_HIGH, 1);
                ms_BH1750 = millis();
            }
        }
    }
    
    //BME280::Setup();
    //TSL2561::Setup();
#endif
    xTaskCreatePinnedToCore(scanTask, "scanTask", 7168, nullptr, 2, &scanTaskHandle, CONFIG_BT_NIMBLE_PINNED_TO_CORE);
    xTaskCreatePinnedToCore(reportTask, "reportTask", 7168, nullptr, 1, &reportTaskHandle, 1);
    configureOTA();
}

#ifdef SENSORS
void dhtLoop()
{
    if (!dht11Pin && !dht22Pin) return;

    if (gotNewTemperature)
    {
        float humidity = dhtSensorData.humidity;
        float temperature = dhtSensorData.temperature + dhtTempOffset;
        Serial.println("Temp: " + String(temperature, 1) + "'C Humidity: " + String(humidity, 1) + "%");

        mqttClient.publish((roomsTopic + "/humidity").c_str(), 0, 1, String(humidity, 1).c_str());
        mqttClient.publish((roomsTopic + "/temperature").c_str(), 0, 1, String(temperature, 1).c_str());

        gotNewTemperature = false;
    }
}

//non blocking ambient sensor
void luxLoop()
{
    if (I2C_Bus_1_Enabled || I2C_Bus_2_Enabled) {

        if (BH1750_I2c == "0x23" || BH1750_I2c == "0x5C")
        {

            float lux;
            int lux_mqtt;

            if (BH1750.hasValue())
            {
                ms_BH1750 = millis() - ms_BH1750;
                if (!BH1750.saturated())
                {
                    lux = BH1750.getLux();
                    lux_mqtt = int(lux);

                    if (lux != lux_BH1750)
                    {
                        lux_BH1750 = lux;
                        // Serial.print("BH1750 (");
                        // Serial.print(ms_BH1750);
                        // Serial.print(" ms): ");
                        // Serial.print(lux);
                        // Serial.println(" lx");
                    }

                    //convert lx to integer to reduce mqtt traffic, send only if lx changed
                    if (lux_mqtt != lux_BH1750_MQTT)
                    {
                        lux_BH1750_MQTT = lux_mqtt;
                        Serial.print("BH1750 (");
                        Serial.print(ms_BH1750);
                        Serial.print(" ms): ");
                        Serial.print(lux_mqtt);
                        Serial.println(" lx");
                        mqttClient.publish((roomsTopic + "/lux").c_str(), 0, 1, String(lux_mqtt).c_str());
                    }
                }

                BH1750.adjustSettings(90);
                BH1750.start();
                ms_BH1750 = millis();
            }
        }
    }
}

void l2cScanner()
{
    if (I2C_Bus_1_Enabled || I2C_Bus_2_Enabled) {

        if (!I2CDebug) return;

        byte error, address;
        int nDevices;
        Serial.println("Scanning I2C device...");
        nDevices = 0;

        for (address = 1; address < 127; address++)
        {
            Wire.beginTransmission(address);
            error = Wire.endTransmission();
            if (error == 0)
            {
                Serial.print("I2C device found on bus 1 at address 0x");

                if (address < 16)
                {
                    Serial.print("0");
                }

                Serial.println(address, HEX);
                nDevices++;
            }
            else if (error == 4)
            {
                Serial.print("Unknow error on bus 1 at address 0x");
                if (address < 16)
                {
                    Serial.print("0");
                }
                Serial.println(address, HEX);
            }
        }

        for (address = 1; address < 127; address++)
        {
            Wire1.beginTransmission(address);
            error = Wire1.endTransmission();
            if (error == 0)
            {
                Serial.print("I2C device found on bus 2 at address 0x");

                if (address < 16)
                {
                    Serial.print("0");
                }

                Serial.println(address, HEX);
                nDevices++;
            }
            else if (error == 4)
            {
                Serial.print("Unknow error on bus 2 at address 0x");
                if (address < 16)
                {
                    Serial.print("0");
                }
                Serial.println(address, HEX);
            }
        }

        if (nDevices == 0)
        {
            Serial.println("No I2C devices found\n");
        }
        else
        {
            Serial.println("done\n");
            I2CDebug = false;
        }
        delay(5000);
    }
}
#endif

void loop()
{
    uint32_t freeHeap = ESP.getFreeHeap();
    if (arduinoOta && freeHeap > 4096)
        ArduinoOTA.handle();
    if (freeHeap < 10000) Serial.printf("Low memory: %u bytes free", freeHeap);
    firmwareUpdate();
    Motion::Loop(mqttClient);
#ifdef SENSORS
    dhtLoop();
    luxLoop();
    BME280::Loop(mqttClient);
    TSL2561::Loop(mqttClient);
    l2cScanner();
#endif
    WiFiSettings.httpLoop();
}
