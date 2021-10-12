/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_UTILS_H
#define FTX_ZORRO_PLUGIN_UTILS_H

#include <nlohmann/json.hpp>
#include <chrono>
#include <string>

using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

constexpr char hexMap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

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

template<typename ValueType>
bool readEnum(const nlohmann::json &json, const std::string &key, ValueType &value, bool canThrow = false) {
    nlohmann::json::const_iterator it;

    it = json.find(key);

    if (canThrow) {
        if (!it.value().is_null()) {
            value = ValueType::_from_string(it->get<std::string>().c_str());
        }
        return true;
    } else {
        if (it != json.end()) {
            if (!it.value().is_null()) {
                value = ValueType::_from_string(it->get<std::string>().c_str());
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

#endif //FTX_ZORRO_PLUGIN_UTILS_H
