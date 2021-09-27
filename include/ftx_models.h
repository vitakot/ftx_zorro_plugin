/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_FTX_MODELS_H
#define FTX_ZORRO_PLUGIN_FTX_MODELS_H

#include "i_json.h"

enum class Side : std::int32_t {
    Sell = 0,
};

struct FTXPosition : public IJson {

    double m_cost = 0.0;
    double m_entryPrice = 0.0;
    std::string m_future;
    double m_initialMarginRequirement = 0.0;


//    cost": -31.7906,
//    "entryPrice": 138.22,
//    "future": "ETH-PERP",
//    "initialMarginRequirement": 0.1,
//    "longOrderSize": 1744.55,
//    "maintenanceMarginRequirement": 0.04,
//    "netSize": -0.23,
//    "openSize": 1744.32,
//    "realizedPnl": 3.39441714,
//    "shortOrderSize": 1732.09,
//    "side": "sell",
//    "size": 0.23,
//    "unrealizedPnl": 0

    [[nodiscard]] nlohmann::json toJson() const override;

    bool fromJson(const nlohmann::json &json) override;
};

struct Account : public IJson {

};

#endif //FTX_ZORRO_PLUGIN_FTX_MODELS_H
