#ifndef _BLEFINGERPRINT_
#define _BLEFINGERPRINT_

#include "GUI.h"
#include <ArduinoJson.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <NimBLEEddystoneTLM.h>
#include <NimBLEEddystoneURL.h>
#include <SoftFilters.h>

#define NO_RSSI -32768

class BleFingerprintCollection;

class BleFingerprint
{

public:
    BleFingerprint(BleFingerprintCollection *parent, BLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff);

    bool seen(BLEAdvertisedDevice *advertisedDevice);
    bool report(JsonDocument *doc, float maxDistance);

    String getId()
    {
        if (!pid.isEmpty()) return pid;
        if (macPublic) return getMac();
        if (!sid.isEmpty()) return sid;
        return getMac();
    }
    String getMac();
    int get1mRssi();

    float getDistance() { return output.value.position; }
    int getRssi() { return rssi; }
    int getNewestRssi() { return newest; }

    void setInitial(int rssi, float distance);

    NimBLEAddress getAddress() { return address; }
    long getAge() { return millis() - lastSeenMillis; };
    bool getIgnore() { return ignore; };

private:
    void fingerprint(BLEAdvertisedDevice *advertisedDevice);

    BleFingerprintCollection *_parent;
    bool hasValue = false, close = false, reported = false, macPublic = false, ignore = false;
    NimBLEAddress address;
    String pid, sid, name, url;
    int rssi = -100, calRssi = NO_RSSI, mdRssi = NO_RSSI, asRssi = NO_RSSI;
    int newest = -100;
    int recent = -100;
    int oldest = -100;
    float raw = 0, lastReported = 0, temp = 0;
    unsigned long firstSeenMillis, lastSeenMillis = 0, lastReportedMillis = 0;
    uint16_t volts = 0;

    Reading<Differential<float>> output;

    OneEuroFilter<float, long long> oneEuro;
    DifferentialFilter<float, long long> diffFilter;

    bool filter();
};
#endif
