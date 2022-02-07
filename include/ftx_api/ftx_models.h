/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_MODELS_H
#define FTX_MODELS_H

#include <nlohmann/json.hpp>
#include <ftx_api/i_json.h>
#include <enum.h>
#include <variant>

namespace ftx {

BETTER_ENUM(Side, std::int32_t,
            sell,
            buy
)

BETTER_ENUM(MarketType, std::int32_t,
            spot,
            future
)

/// New i a C++ keyword so it is capitalised
BETTER_ENUM(OrderStatus, std::int32_t,
            New,
            open,
            closed
)

BETTER_ENUM(OrderType, std::int32_t,
            market,
            limit,
            stop,
            trailingStop,
            takeProfit
)

BETTER_ENUM(Operation, std::int32_t,
            subscribe,
            unsubscribe,
            ping,
            login
)

BETTER_ENUM(Channel, std::int32_t,
            orderbook,
            trades,
            fills,
            orders,
            ftxpay,
            ticker,
            markets,
            pingpong
)

BETTER_ENUM(OperationResponse, std::int32_t,
            error,
            subscribed,
            unsubscribed,
            info,
            partial,
            update,
            pong
)

struct Response : public IJson {
    bool m_success = false;
    std::string m_error;
    nlohmann::json m_result;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Position : public IJson {

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

struct Positions : public IJson {
    std::vector<Position> m_positions;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Account : public IJson {

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
    std::vector<Position> m_positions;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Order : public IJson {

    std::string m_createdAt;
    double m_filledSize = 0.0;
    std::string m_future;
    std::int64_t m_id = -1;
    std::string m_market;
    double m_price = 0.0;
    double m_avgFillPrice = 0.0;
    double m_remainingSize = 0.0;
    Side m_side = Side::buy;
    double m_size = 0.0;
    OrderStatus m_status = OrderStatus::New;
    OrderType m_type = OrderType::limit;
    bool m_reduceOnly = false;
    bool m_ioc = false;
    bool m_postOnly = false;
    std::string m_clientId;
    double m_triggerPrice = 0.0;
    double m_trailValue = 0.0;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Market : public IJson {

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

struct Markets : public IJson {
    std::vector<Market> m_markets;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Candle : public IJson {

    std::string m_startTime;
    double m_open = 0.0;
    double m_high = 0.0;
    double m_low = 0.0;
    double m_close = 0.0;
    double m_volume = 0.0;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Candles : public IJson {
    std::vector<Candle> m_candles;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct ChannelSubscriptionRequest : public IJson {
    Operation m_op = Operation::subscribe;
    Channel m_channel = Channel::ticker;
    std::string m_market;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct ChannelSubscriptionResponse : public IJson {
    OperationResponse m_type = OperationResponse::error;
    Channel m_channel = Channel::ticker;
    std::string m_market;
    int m_code = 0;
    std::string m_msg;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct AuthenticationRequest : public IJson {
    Operation m_op = Operation::login;
    std::string m_key;
    std::string m_sign;
    std::string m_subAccount;
    std::int64_t m_time = 0;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct TickerData : public IJson {
    double m_bid = 0.0;
    double m_ask = 0.0;
    double m_bidSize = 0.0;
    double m_askSize = 0.0;
    double m_last = 0.0;
    std::int64_t m_time = 0;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct OrderBookData : public IJson {

    [[nodiscard]] nlohmann::json toJson() const override { return nlohmann::json{}; };

    void fromJson(const nlohmann::json &json) override {};
};

struct MarketsData : public IJson {

    [[nodiscard]] nlohmann::json toJson() const override { return nlohmann::json{}; };

    void fromJson(const nlohmann::json &json) override {};
};

struct TradesData : public IJson {

    [[nodiscard]] nlohmann::json toJson() const override { return nlohmann::json{}; };

    void fromJson(const nlohmann::json &json) override {};
};

struct FillData : public IJson {
    double m_fee = 0.0;
    double m_feeRate = 0.0;
    std::string m_future;
    std::int64_t m_id = -1;
    std::string m_liquidity;
    std::string m_market;
    std::int64_t m_orderId = -1;
    std::int64_t m_tradeId = -1;
    double m_price = 0.0;
    Side m_side = Side::buy;
    double m_size = 0;
    std::string m_time;
    std::string m_type;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct OrderData : public IJson {
    std::int64_t m_id = -1;
    std::string m_clientId;
    std::string m_market;
    OrderType m_type = OrderType::limit;
    Side m_side = Side::buy;
    double m_size = 0.0;
    double m_price = 0.0;
    bool m_reduceOnly;
    bool m_ioc;
    bool m_postOnly;
    OrderStatus m_status = OrderStatus::closed;
    double m_filledSize = 0.0;
    double m_remainingSize = 0.0;
    double m_avgFillPrice = 0.0;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FTXPayData : public IJson {
    std::string m_status;
    nlohmann::json m_app;
    nlohmann::json m_payment;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct NoData {
};

struct Event : public IJson {
    ChannelSubscriptionResponse m_subscriptionResponse;
    std::variant<NoData, TickerData, OrderBookData, MarketsData, TradesData, FillData, OrderData, FTXPayData> m_eventData;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Ping : public IJson {
    Operation m_op = Operation::ping;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Pong : public IJson {
    OperationResponse m_op = OperationResponse::pong;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

}
#endif //FTX_MODELS_H
