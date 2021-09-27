/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef I_JSON_H
#define I_JSON_H

#include "nlohmann/json_fwd.hpp"

struct IJson {

    virtual ~IJson() = default;

    /**
     * Serialize object to JSON object
     * @throws nlohmann::json::exception, std::exception
     * @return
     */
    [[nodiscard]] virtual nlohmann::json toJson() const = 0;

    /**
     * Deserialize object from JSON object
     * @param json
     * @throws nlohmann::json::exception, std::exception
     * @return
     */
    virtual void fromJson(const nlohmann::json &json) = 0;
};

#endif // I_JSON_H
