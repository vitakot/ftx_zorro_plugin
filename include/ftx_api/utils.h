/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef UTILS_H
#define UTILS_H

#include <nlohmann/json.hpp>
#include <chrono>
#include <string>
#include <enum.h>

#define STRINGIZE_I(x) #x
#define STRINGIZE(x) STRINGIZE_I(x)

#define MAKE_FILELINE \
    __FILE__ "(" STRINGIZE(__LINE__) ")"

namespace ftx {

using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

BETTER_ENUM(LogSeverity, std::int32_t,
            Info,
            Warning,
            Critical,
            Error,
            Debug,
            Trace
)

using onLogMessage = std::function<void(LogSeverity severity, const std::string &errmsg)>;

constexpr char hexMap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

/**
 * Helper for reading a value from nlohmann::json object.
 * @tparam ValueType
 * @param json
 * @param key
 * @param value
 * @param canThrow Function will throw an exception instead of silently ignoring a missing attribute
 * @return true if succeeded and canThrow parameter is false
 */
template<typename ValueType>
bool readValue(const nlohmann::json &json, const std::string &key, ValueType &value, bool canThrow = false) {
    nlohmann::json::const_iterator it;

    it = json.find(key);

    if (canThrow) {
        if (!it.value().is_null()) {
            value = it.value();
        }
        return true;
    } else {
        if (it != json.end()) {
            if (!it.value().is_null()) {
                value = it.value();
            }
            return true;
        }
    }
    return false;
}

/**
 * Helper for reading a string value from nlohmann::json object and transform it to double.
 * @param json
 * @param key
 * @param defaultVal Will be used if value cannot be found or of it is not transformable into double.
 * @return
 */
double readStringAsDouble(const nlohmann::json &json, const std::string &key, double defaultVal = 0.0);

/**
 * Helper for reading a Better Enum value (http://github.com/aantron/better-enums) from nlohmann::json object.
 * @tparam ValueType
 * @param json
 * @param key
 * @param value
 * @param canThrow Function will throw an exception instead of silently ignoring a missing attribute
 * @return true if succeeded and canThrow parameter is false
 */
template<typename ValueType>
bool readEnum(const nlohmann::json &json, const std::string &key, ValueType &value, bool canThrow = false) {
    nlohmann::json::const_iterator it;

    it = json.find(key);

    if (canThrow) {
        if (!it.value().is_null()) {
            value = ValueType::_from_string_nocase(it->get<std::string>().c_str());
        }
        return true;
    } else {
        if (it != json.end()) {
            if (!it.value().is_null()) {
                value = ValueType::_from_string_nocase(it->get<std::string>().c_str());
            }
            return true;
        }
    }
    return false;
}

inline TimePoint currentTime() {
    return Clock::now();
}

inline std::chrono::milliseconds getMsTimestamp(TimePoint time) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch());
}

inline std::string stringToHex(const unsigned char *data, std::size_t len) {
    std::string s(len * 2, ' ');
    for (std::size_t i = 0; i < len; ++i) {
        s[2 * i] = hexMap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexMap[data[i] & 0x0F];
    }
    return s;
}

/**
 * Pack Date and Time components into MS COM time format (a double)
 * @param year
 * @param month
 * @param day
 * @param hour
 * @param min
 * @param sec
 * @param msec
 * @return time represented by double (aka Variant in MS COM)
 */
double systemTimeToVariantTimeMs(unsigned short year, unsigned short month, unsigned short day,
                                 unsigned short hour, unsigned short min, unsigned short sec,
                                 unsigned int msec);

/**
 * Copy string src to buffer dst of size dsize.  At most dsize-1
 * chars will be copied.  Always NUL terminates (unless dsize == 0).
 * Returns strlen(src); if retval >= dsize, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t dsize);

/**
 * Split string parts separated by a delimiter into a vector of sub-strings
 * @param s
 * @param delim
 * @return vector of sub-strings
 */
std::vector<std::string> splitString(const std::string &s, char delim);

/**
 * Read a string and represents it as a bool if possible
 * @param v e.g. "false", "true", "1", "0"
 * @return
 */
inline bool string2bool(std::string v) {
    std::transform(v.begin(), v.end(), v.begin(), ::tolower);
    return !v.empty() && (v == "true" || atoi(v.c_str()) != 0);
}

/**
 * Same as std::mktime but does not convert into local time, uses UTC instead
 * @param ptm
 * @return
 */
time_t mkgmtime(const struct tm *ptm);

/**
 * A helper for converting date-time strings into the Unix timestamp (seconds from epoch)
 * @param timeString e.g. "2022-01-28T21:45:00+00:00"
 * @param format e.g. "%Y-%m-%dT%H:%M:%S:%z"
 * @return seconds from epoch
 */
int64_t getTimeStampFromString(const std::string &timeString, const std::string &format);

}
#endif //UTILS_H
