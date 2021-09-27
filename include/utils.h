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

template<typename ValueType>
bool readValue(const nlohmann::json &json, const std::string &key, ValueType &value) {
    nlohmann::json::const_iterator it;

    it = json.find(key);

    if (it != json.end()) {
        value = it.value();
        return true;

    }
    return false;
}

inline TimePoint currentTime() {
    return Clock::now();
}

inline std::chrono::milliseconds getMsTimestamp(TimePoint time) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch());
}

std::string hmacString(const std::string &secret, std::string msg, std::size_t signedLen);

std::string stringToHex(const unsigned char *data, std::size_t len);

#endif //FTX_ZORRO_PLUGIN_UTILS_H
