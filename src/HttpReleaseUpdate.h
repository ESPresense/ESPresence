#ifndef ___HTTP_RELEASE_UPDATE_H___
#define ___HTTP_RELEASE_UPDATE_H___

#include <Arduino.h>
#include <HTTPClient.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#define HTTP_UE_TOO_LESS_SPACE (-100)
#define HTTP_UE_SERVER_NOT_REPORT_SIZE (-101)
#define HTTP_UE_SERVER_FILE_NOT_FOUND (-102)
#define HTTP_UE_SERVER_FORBIDDEN (-103)
#define HTTP_UE_SERVER_WRONG_HTTP_CODE (-104)
#define HTTP_UE_SERVER_FAULTY_MD5 (-105)
#define HTTP_UE_BIN_VERIFY_HEADER_FAILED (-106)
#define HTTP_UE_BIN_FOR_WRONG_FLASH (-107)
#define HTTP_UE_NO_PARTITION (-108)

enum HttpUpdateResult {
    HTTP_UPDATE_FAILED,
    HTTP_UPDATE_NO_UPDATES,
    HTTP_UPDATE_OK
};

using HttpUpdateStartCB = std::function<void()>;
using HttpUpdateEndCB = std::function<void(bool)>;
using HttpUpdateErrorCB = std::function<void(int)>;
using HttpUpdateProgressCB = std::function<void(int, int)>;

class HttpReleaseUpdate {
   public:
    void setTimeout(int httpClientTimeout) {
        _httpClientTimeout = httpClientTimeout;
    }

    void rebootOnUpdate(bool reboot) {
        _rebootOnUpdate = reboot;
    }

    void setLedPin(int ledPin = -1, uint8_t ledOn = HIGH) {
        _ledPin = ledPin;
        _ledOn = ledOn;
    }

    HttpUpdateResult update(WiFiClient& client, const String& url);

    void onStart(HttpUpdateStartCB cbOnStart) { _cbStart = cbOnStart; }
    void onEnd(HttpUpdateEndCB cbOnEnd) { _cbEnd = cbOnEnd; }
    void onError(HttpUpdateErrorCB cbOnError) { _cbError = cbOnError; }
    void onProgress(HttpUpdateProgressCB cbOnProgress) { _cbProgress = cbOnProgress; }

    int getLastError(void);
    String getLastErrorString(void);

   protected:
    HttpUpdateResult handleUpdate(HTTPClient& http);
    bool runUpdate(Stream& in, uint32_t size);

    void _setLastError(int err) {
        _lastError = err;
        if (_cbError) {
            _cbError(err);
        }
    }
    int _lastError;
    bool _rebootOnUpdate = true;

   private:
    int _httpClientTimeout = 8000;

    // Callbacks
    HttpUpdateStartCB _cbStart;
    HttpUpdateEndCB _cbEnd;
    HttpUpdateErrorCB _cbError;
    HttpUpdateProgressCB _cbProgress;

    int _ledPin;
    uint8_t _ledOn;
};

#endif
