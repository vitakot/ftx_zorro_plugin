/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

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
    readValue<double>(json, "cumulativeBuySize", m_cumulativeBuySize);
    readValue<double>(json, "cumulativeSellSize", m_cumulativeSellSize);
    readValue<double>(json, "estimatedLiquidationPrice", m_estimatedLiquidationPrice);
    readValue<double>(json, "recentAverageOpenPrice", m_recentAverageOpenPrice);
    readValue<double>(json, "recentBreakEvenPrice", m_recentBreakEvenPrice);
    readValue<double>(json, "recentPnl", m_recentPnl);
    readValue<double>(json, "collateralUsed", m_collateralUsed);
}

nlohmann::json FTXAccount::toJson() const {
    throw std::exception("Unimplemented: FTXAccount::toJson()");
}

void FTXAccount::fromJson(const nlohmann::json &json) {

    m_positions.clear();
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

nlohmann::json FTXOrder::toJson() const {
    throw std::exception("Unimplemented: FTXOrder::toJson()");
}

void FTXOrder::fromJson(const nlohmann::json &json) {

}

nlohmann::json FTXMarket::toJson() const {
    throw std::exception("Unimplemented: FTXMarket::toJson()");
}

void FTXMarket::fromJson(const nlohmann::json &json) {
    readValue<std::string>(json, "name", m_name);
    readValue<std::string>(json, "baseCurrency", m_baseCurrency);
    readValue<std::string>(json, "quoteCurrency", m_quoteCurrency);
    readValue<double>(json, "quoteVolume24h", m_quoteVolume24h);
    readValue<double>(json, "change1h", m_change1h);
    readValue<double>(json, "change24h", m_change24h);
    readValue<double>(json, "changeBod", m_changeBod);
    readValue<bool>(json, "highLeverageFeeExempt", m_highLeverageFeeExempt);
    readValue<double>(json, "minProvideSize", m_minProvideSize);
    readEnum<MarketType>(json, "type", m_type);
    readValue<std::string>(json, "underlying", m_underlying);
    readValue<bool>(json, "enabled", m_enabled);
    readValue<double>(json, "ask", m_ask);
    readValue<double>(json, "bid", m_bid);
    readValue<double>(json, "last", m_last);
    readValue<bool>(json, "postOnly", m_postOnly);
    readValue<double>(json, "price", m_price);
    readValue<double>(json, "priceIncrement", m_priceIncrement);
    readValue<double>(json, "sizeIncrement", m_sizeIncrement);
    readValue<bool>(json, "restricted", m_restricted);
    readValue<double>(json, "volumeUsd24h", m_volumeUsd24h);
}

nlohmann::json FTXMarkets::toJson() const {
    throw std::exception("Unimplemented: FTXMarkets::toJson()");
}

void FTXMarkets::fromJson(const nlohmann::json &json) {

    m_markets.clear();

    for (const auto &el: json) {
        FTXMarket market;
        market.fromJson(el);
        m_markets.push_back(market);
    }
}

nlohmann::json FTXCandle::toJson() const {
    throw std::exception("Unimplemented: FTXCandle::toJson()");
}

void FTXCandle::fromJson(const nlohmann::json &json) {
    readValue<double>(json, "open", m_open);
    readValue<double>(json, "high", m_high);
    readValue<double>(json, "low", m_low);
    readValue<double>(json, "close", m_close);
    readValue<double>(json, "volume", m_volume);
    readValue<std::string>(json, "startTime", m_startTime);
}

nlohmann::json FTXCandles::toJson() const {
    throw std::exception("Unimplemented: FTXCandles::toJson()");
}

void FTXCandles::fromJson(const nlohmann::json &json) {
    m_candles.clear();

    for (const auto &el: json) {
        FTXCandle candle;
        candle.fromJson(el);
        m_candles.push_back(candle);
    }
}