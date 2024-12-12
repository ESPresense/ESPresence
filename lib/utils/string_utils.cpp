#include <regex>
#include <string_utils.h>
#include <SPIFFS.h>
#include <algorithm>
#include <cctype>

static constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string ltrim(const std::string &s, char toTrim)
{
    auto start = s.find_first_not_of(toTrim);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s, char toTrim)
{
    auto end = s.find_last_not_of(toTrim);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string lowertrim(const std::string &str, char toTrim)
{
    std::string lowerStr;
    std::transform(str.begin(), str.end(), std::back_inserter(lowerStr), [](unsigned char c) {
        return std::tolower(c);
    });
    return rtrim(ltrim(lowerStr, toTrim), toTrim);
}

std::regex whitespace_re("[\\s\\W-]+");
std::string slugify(const std::string &text)
{
    return lowertrim(std::regex_replace(text, whitespace_re, "_"), '_');
}

String slugify(const String &text)
{
    return String(slugify(std::string(text.c_str())).c_str());
}

std::string kebabify(const std::string &text)
{
    return lowertrim(std::regex_replace(text, whitespace_re, "-"), '-');
}

String kebabify(const String &text)
{
    return String(kebabify(std::string(text.c_str())).c_str());
}

template<typename T>
std::string hexStrImpl(const T *data, size_t len)
{
    std::string s(len * 2, ' ');
    for (size_t i = 0; i < len; ++i)
    {
        s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}

std::string hexStr(const uint8_t *data, int len) {
    return hexStrImpl(data, len);
}

std::string hexStr(const char *data, unsigned int len)
{
    return hexStrImpl(data, len);
}

std::string hexStr(const std::string &s)
{
    return hexStrImpl(reinterpret_cast<const uint8_t*>(s.data()), s.length());
}

std::string hexStrRev(const char *data, unsigned int len) {
    std::string s(len * 2, ' ');
    for (unsigned int i = 0; i < len; ++i) {
        s[2 * i] = hexmap[data[len - 1 - i] & 0x0F];
        s[2 * i + 1] = hexmap[(data[len - 1 - i] & 0xF0) >> 4];
    }
    return s;
}

std::string hexStrRev(const uint8_t *&s, unsigned int len) {
    return hexStrRev(reinterpret_cast<const char *>(s), len);
}

std::string hexStrRev(const std::string &s) {
    return hexStrRev(s.c_str(), s.length());
}

uint8_t hextob(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return 0;
}

bool hextostr(const String &hexStr, uint8_t* output, size_t len) {
    if (hexStr.length() != len * 2) return false;

    for (size_t i = 0; i < len; ++i)
        output[i] = (hextob(hexStr[2 * i]) << 4) | hextob(hexStr[2 * i + 1]);

    return true;
}


bool prefixExists(const String &prefixes, const String &s) {
    unsigned int start = 0;
    unsigned int space;

    while ((space = prefixes.indexOf(' ', start)) != -1) {
        if (space > start && s.startsWith(prefixes.substring(start, space))) return true;
        start = space + 1;
    }

    return start < prefixes.length() && s.startsWith(prefixes.substring(start));
}


bool spurt(const String &fn, const String &content) {
    File f = SPIFFS.open(fn, "w");
    if (!f) return false;
    auto w = f.print(content);
    f.close();
    return w == content.length();
}
