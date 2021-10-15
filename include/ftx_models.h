/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_FTX_MODELS_H
#define FTX_ZORRO_PLUGIN_FTX_MODELS_H

#include <nlohmann/json.hpp>
#include "i_json.h"
#include "enum.h"

BETTER_ENUM(Side, std::int32_t,
            sell,
            buy
)

BETTER_ENUM(MarketType, std::int32_t,
            spot,
            future
)

struct FTXResponse : public IJson {
    bool m_success = false;
    nlohmann::json m_result;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXPosition : public IJson {

    double m_cost = 0.0;
    double m_entryPrice = 0.0;
    std::string m_future;
    double m_initialMarginRequirement = 0.0;
    double m_longOrderSize = 0.0;
    double m_maintenanceMarginRequirement = 0.0;
    double m_netSize = 0.0;
    double m_openSize = 0.0;
    double m_realizedPnl = 0.0;
    double m_shortOrderSize = 0.0;
    Side m_side = Side::buy;
    double m_size = 0.0;
    double m_unrealizedPnl = 0.0;
    double m_cumulativeBuySize = 0.0;
    double m_cumulativeSellSize = 0.0;
    double m_estimatedLiquidationPrice = 0.0;
    double m_recentAverageOpenPrice = 0.0;
    double m_recentBreakEvenPrice = 0.0;
    double m_recentPnl = 0.0;
    double m_collateralUsed = 0.0;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXPositions : public IJson {
    std::vector<FTXPosition> m_positions;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXAccount : public IJson {

    bool m_backstopProvider = false;
    double m_collateral = 0.0;
    double m_freeCollateral = 0.0;
    double m_initialMarginRequirement = 0.0;
    double m_leverage = 0.0;
    bool m_liquidating = false;
    double m_maintenanceMarginRequirement = 0.0;
    double m_makerFee = 0.0;
    double m_marginFraction = 0.0;
    double m_openMarginFraction = 0.0;
    double m_takerFee = 0.0;
    double m_totalAccountValue = 0.0;
    double m_totalPositionSize = 0.0;
    std::string m_userName;
    std::vector<FTXPosition> m_positions;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXOrder : public IJson {

//    std::string m_createdAt;
//    std::string m_
//    "createdAt": "2019-03-05T09:56:55.728933+00:00",
//    "filledSize": 10,
//    "future": "XRP-PERP",
//    "id": 9596912,
//    "market": "XRP-PERP",
//    "price": 0.306525,
//    "avgFillPrice": 0.306526,
//    "remainingSize": 31421,
//    "side": "sell",
//    "size": 31431,
//    "status": "open",
//    "type": "limit",
//    "reduceOnly": false,
//    "ioc": false,
//    "postOnly": false,
//    "clientId": null

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXMarket : public IJson {

    std::string m_name;
    std::string m_baseCurrency;
    std::string m_quoteCurrency;
    double m_quoteVolume24h = 0.0;
    double m_change1h = 0.0;
    double m_change24h = 0.0;
    double m_changeBod = 0.0;
    bool m_highLeverageFeeExempt = false;
    double m_minProvideSize = 0.0;
    MarketType m_type = MarketType::spot;
    std::string m_underlying;
    bool m_enabled = false;
    double m_ask = 0.0;
    double m_bid = 0.0;
    double m_last = 0.0;
    bool m_postOnly = false;
    double m_price = 0.0;
    double m_priceIncrement = 0.0;
    double m_sizeIncrement = 0.0;
    bool m_restricted = false;
    double m_volumeUsd24h = 0.0;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXMarkets : public IJson {
    std::vector<FTXMarket> m_markets;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXCandle : public IJson {

    std::string m_startTime;
    double m_open = 0.0;
    double m_high = 0.0;
    double m_low = 0.0;
    double m_close = 0.0;
    double m_volume = 0.0;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXCandles : public IJson {
    std::vector<FTXCandle> m_candles;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

#endif //FTX_ZORRO_PLUGIN_FTX_MODELS_H
