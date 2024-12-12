#pragma once
#include <Arduino.h>
#include <WString.h>
#include <string>

static inline uint32_t getChipId() {
    return static_cast<uint32_t>(ESP.getEfuseMac() >> 24);
}

static inline String getEspMac() {
    char buffer[7];
    snprintf(buffer, sizeof(buffer), "%06x", getChipId());
    return String(buffer);
}

// Wrapper function for Sprintf
static inline String Sprintf(const char* format, ...) {
    char* buffer;
    va_list args;
    va_start(args, format);
    int err = vasprintf(&buffer, format, args);
    va_end(args);
    if (err < 0) return String();
    String result(buffer);
    free(buffer);
    return result;
}

// Wrapper function for Stdprintf
static inline std::string Stdprintf(const char* format, ...) {
    char* buffer;
    va_list args;
    va_start(args, format);
    vasprintf(&buffer, format, args);
    va_end(args);
    std::string result(buffer);
    free(buffer);
    return result;
}

// Function declarations
std::string slugify(const std::string& text);
String slugify(const String& text);
std::string kebabify(const std::string& text);
String kebabify(const String& text);
std::string hexStr(const uint8_t *data, int len);
std::string hexStr(const char *data, int len);
std::string hexStr(const std::string& s);
std::string hexStrRev(const uint8_t *data, int len);
std::string hexStrRev(const char *data, int len);
std::string hexStrRev(const std::string &s);
bool hextostr(const String &hexStr, uint8_t* output, size_t len);
bool prefixExists(const String &prefixes, const String &s);
bool spurt(const String &fn, const String &content);
