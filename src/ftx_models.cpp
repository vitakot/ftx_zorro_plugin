/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <iostream>
#include "ftx_models.h"
#include "utils.h"

nlohmann::json FTXResponse::toJson() const {
    throw std::exception("Unimplemented: FTXResponse::toJson()");
}

void FTXResponse::fromJson(const nlohmann::json &json) {

    readValue<bool>(json, "success", m_success);
    m_result = json["result"];
}

nlohmann::json FTXPosition::toJson() const {
    throw std::exception("Unimplemented: FTXPosition::toJson()");
}

void FTXPosition::fromJson(const nlohmann::json &json) {

    readValue<double>(json, "cost", m_cost);
    readValue<double>(json, "entryPrice", m_entryPrice);
    readValue<std::string>(json, "future", m_future);
    readValue<double>(json, "initialMarginRequirement", m_initialMarginRequirement);
    readValue<double>(json, "longOrderSize", m_longOrderSize);
    readValue<double>(json, "maintenanceMarginRequirement", m_maintenanceMarginRequirement);
    readValue<double>(json, "netSize", m_netSize);
    readValue<double>(json, "openSize", m_openSize);
    readValue<double>(json, "realizedPnl", m_realizedPnl);
    readValue<double>(json, "shortOrderSize", m_shortOrderSize);
    readEnum<Side>(json, "side", m_side);
    readValue<double>(json, "size", m_size);
    readValue<double>(json, "unrealizedPnl", m_unrealizedPnl);
}

nlohmann::json Account::toJson() const {
    throw std::exception("Unimplemented: Account::toJson()");
}

void Account::fromJson(const nlohmann::json &json) {

    m_positions.clear();

    std::string prd = json.dump(4);

    readValue<bool>(json, "backstopProvider", m_backstopProvider);
    readValue<double>(json, "collateral", m_collateral);
    readValue<double>(json, "freeCollateral", m_freeCollateral);
    readValue<double>(json, "initialMarginRequirement", m_initialMarginRequirement);
    readValue<double>(json, "leverage", m_leverage);
    readValue<bool>(json, "liquidating", m_liquidating);
    readValue<double>(json, "maintenanceMarginRequirement", m_maintenanceMarginRequirement);
    readValue<double>(json, "makerFee", m_makerFee);
    readValue<double>(json, "marginFraction", m_marginFraction);
    readValue<double>(json, "openMarginFraction", m_openMarginFraction);
    readValue<double>(json, "takerFee", m_takerFee);
    readValue<double>(json, "totalAccountValue", m_totalAccountValue);
    readValue<double>(json, "totalPositionSize", m_totalPositionSize);
    readValue<std::string>(json, "username", m_userName);

    for (const auto &el: json["positions"]) {
        FTXPosition ftxPosition;
        ftxPosition.fromJson(el);
        m_positions.push_back(ftxPosition);
    }
}
