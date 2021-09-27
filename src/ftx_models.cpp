/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "ftx_models.h"

nlohmann::json FTXPosition::toJson() const {
    return nlohmann::json();
}

bool FTXPosition::fromJson(const nlohmann::json &json) {
    return false;
}


