/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ftx_api/ftx_models.h>
#include <ftx_api//utils.h>

namespace ftx {

nlohmann::json Response::toJson() const {
    throw std::exception("Unimplemented: Response::toJson()");
}

void Response::fromJson(const nlohmann::json &json) {

    readValue<bool>(json, "success", m_success);

    if (m_success) {
        m_result = json["result"];
    } else {
        readValue<std::string>(json, "error", m_error);
    }
}

nlohmann::json Position::toJson() const {
    throw std::exception("Unimplemented: Position::toJson()");
}

void Position::fromJson(const nlohmann::json &json) {

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

nlohmann::json Positions::toJson() const {
    throw std::exception("Unimplemented: Positions::toJson()");
}

void Positions::fromJson(const nlohmann::json &json) {
    m_positions.clear();

    for (const auto &el: json) {
        Position market;
        market.fromJson(el);
        m_positions.push_back(market);
    }
}

nlohmann::json Account::toJson() const {
    throw std::exception("Unimplemented: Account::toJson()");
}

void Account::fromJson(const nlohmann::json &json) {

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
        Position ftxPosition;
        ftxPosition.fromJson(el);
        m_positions.push_back(ftxPosition);
    }
}

nlohmann::json Order::toJson() const {
    nlohmann::json json;
    json["market"] = m_market;
    json["side"] = m_side._to_string();
    json["price"] = m_price;
    json["type"] = m_type._to_string();
    json["size"] = m_size;
    json["reduceOnly"] = m_reduceOnly;
    json["ioc"] = m_ioc;
    json["postOnly"] = m_postOnly;
    json["clientId"] = m_clientId;

    if (m_type == +OrderType::stop || m_type == +OrderType::takeProfit) {
        json["triggerPrice"] = m_triggerPrice;
    } else if (m_type == +OrderType::trailingStop) {
        json["trailValue"] = m_trailValue;
    }

    return json;
}

void Order::fromJson(const nlohmann::json &json) {
    readValue<std::string>(json, "createdAt", m_createdAt);
    readValue<double>(json, "filledSize", m_filledSize);
    readValue<std::string>(json, "future", m_future);
    readValue<std::int64_t>(json, "id", m_id);
    readValue<std::string>(json, "market", m_market);
    readValue<double>(json, "price", m_price);
    readValue<double>(json, "avgFillPrice", m_avgFillPrice);
    readValue<double>(json, "remainingSize", m_remainingSize);
    readEnum<Side>(json, "side", m_side);
    readValue<double>(json, "size", m_size);
    readEnum<OrderStatus>(json, "status", m_status);
    readEnum<OrderType>(json, "type", m_type);
    readValue<bool>(json, "reduceOnly", m_reduceOnly);
    readValue<bool>(json, "ioc", m_ioc);
    readValue<bool>(json, "postOnly", m_postOnly);
    readValue<std::string>(json, "clientId", m_clientId);
    readValue<double>(json, "trailValue", m_trailValue);
    readValue<double>(json, "triggerPrice", m_triggerPrice);
}

nlohmann::json Market::toJson() const {
    throw std::exception("Unimplemented: Market::toJson()");
}

void Market::fromJson(const nlohmann::json &json) {
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

nlohmann::json Markets::toJson() const {
    throw std::exception("Unimplemented: Markets::toJson()");
}

void Markets::fromJson(const nlohmann::json &json) {

    m_markets.clear();

    for (const auto &el: json) {
        Market market;
        market.fromJson(el);
        m_markets.push_back(market);
    }
}

nlohmann::json Candle::toJson() const {
    throw std::exception("Unimplemented: Candle::toJson()");
}

void Candle::fromJson(const nlohmann::json &json) {
    readValue<double>(json, "open", m_open);
    readValue<double>(json, "high", m_high);
    readValue<double>(json, "low", m_low);
    readValue<double>(json, "close", m_close);
    readValue<double>(json, "volume", m_volume);
    readValue<std::string>(json, "startTime", m_startTime);
}

nlohmann::json Candles::toJson() const {
    throw std::exception("Unimplemented: Candles::toJson()");
}

void Candles::fromJson(const nlohmann::json &json) {
    m_candles.clear();

    for (const auto &el: json) {
        Candle candle;
        candle.fromJson(el);
        m_candles.push_back(candle);
    }
}

nlohmann::json ChannelSubscriptionRequest::toJson() const {
    nlohmann::json json;
    json["op"] = m_op._to_string();
    json["channel"] = m_channel._to_string();

    if (!m_market.empty()) {
        json["market"] = m_market;
    }
    return json;
}

void ChannelSubscriptionRequest::fromJson(const nlohmann::json &json) {
    throw std::exception("Unimplemented: ChannelSubscriptionRequest::fromJson()");
}

nlohmann::json ChannelSubscriptionResponse::toJson() const {
    nlohmann::json json;
    json["type"] = m_type._to_string();
    json["channel"] = m_channel._to_string();
    json["market"] = m_market;
    json["code"] = m_code;
    json["msg"] = m_msg;
    return json;
}

void ChannelSubscriptionResponse::fromJson(const nlohmann::json &json) {
    readEnum<OperationResponse>(json, "type", m_type);
    readEnum<Channel>(json, "channel", m_channel);
    readValue<std::string>(json, "market", m_market);
    readValue<int>(json, "code", m_code);
    readValue<std::string>(json, "msg", m_msg);
}

nlohmann::json AuthenticationRequest::toJson() const {
    nlohmann::json json;
    nlohmann::json jsonArg;
    jsonArg["key"] = m_key;
    jsonArg["subaccount"] = m_subAccount;
    jsonArg["sign"] = m_sign;
    jsonArg["time"] = m_time;
    json["op"] = m_op._to_string();
    json["args"] = jsonArg;
    return json;
}

void AuthenticationRequest::fromJson(const nlohmann::json &json) {
    throw std::exception("Unimplemented: AuthenticationRequest::fromJson()");
}

nlohmann::json TickerData::toJson() const {
    nlohmann::json json;
    json["bid"] = m_bid;
    json["ask"] = m_ask;
    json["bidSize"] = m_bidSize;
    json["askSize"] = m_askSize;
    json["last"] = m_last;
    json["time"] = m_time;
    return json;
}

void TickerData::fromJson(const nlohmann::json &json) {
    readValue<double>(json, "bid", m_bid);
    readValue<double>(json, "ask", m_ask);
    readValue<double>(json, "bidSize", m_bidSize);
    readValue<double>(json, "askSize", m_askSize);
    readValue<double>(json, "last", m_last);
    readValue<std::int64_t>(json, "time", m_time);
}

nlohmann::json FillData::toJson() const {
    nlohmann::json json;
    json["fee"] = m_fee;
    json["feeRate"] = m_feeRate;
    json["future"] = m_future;
    json["id"] = m_id;
    json["liquidity"] = m_liquidity;
    json["market"] = m_market;
    json["orderId"] = m_orderId;
    json["tradeId"] = m_tradeId;
    json["price"] = m_price;
    json["side"] = m_side._to_string();
    json["size"] = m_size;
    json["time"] = m_time;
    json["type"] = m_type;
    return json;
}

void FillData::fromJson(const nlohmann::json &json) {
    readValue<double>(json, "fee", m_fee);
    readValue<double>(json, "feeRate", m_feeRate);
    readValue<std::string>(json, "future", m_future);
    readValue<std::int64_t>(json, "id", m_id);
    readValue<std::string>(json, "liquidity", m_liquidity);
    readValue<std::string>(json, "market", m_market);
    readValue<std::int64_t>(json, "orderId", m_orderId);
    readValue<std::int64_t>(json, "tradeId", m_tradeId);
    readValue<double>(json, "price", m_price);
    readEnum<Side>(json, "side", m_side);
    readValue<double>(json, "size", m_size);
    readValue<std::string>(json, "time", m_time);
    readValue<std::string>(json, "type", m_type);
}

nlohmann::json OrderData::toJson() const {
    nlohmann::json json;

    json["id"] = m_id;
    json["clientId"] = m_clientId;
    json["market"] = m_market;
    json["type"] = m_type._to_string();
    json["side"] = m_side._to_string();

    json["size"] = m_size;
    json["price"] = m_price;
    json["reduceOnly"] = m_reduceOnly;
    json["ioc"] = m_ioc;
    json["postOnly"] = m_postOnly;

    std::string statusStr = m_status._to_string();;
    std::transform(statusStr.begin(), statusStr.end(), statusStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    json["status"] = statusStr;
    json["filledSize"] = m_filledSize;
    json["remainingSize"] = m_remainingSize;
    json["avgFillPrice"] = m_avgFillPrice;
    return json;
}

void OrderData::fromJson(const nlohmann::json &json) {
    readValue<std::int64_t>(json, "id", m_id);
    readValue<std::string>(json, "clientId", m_clientId);
    readValue<std::string>(json, "market", m_market);
    readEnum<OrderType>(json, "type", m_type);
    readEnum<Side>(json, "side", m_side);
    readValue<double>(json, "size", m_size);
    readValue<double>(json, "price", m_price);
    readValue<bool>(json, "reduceOnly", m_reduceOnly);
    readValue<bool>(json, "ioc", m_ioc);
    readValue<bool>(json, "postOnly", m_postOnly);
    readEnum<OrderStatus>(json, "status", m_status);
    readValue<double>(json, "filledSize", m_filledSize);
    readValue<double>(json, "remainingSize", m_remainingSize);
    readValue<double>(json, "avgFillPrice", m_avgFillPrice);
}

nlohmann::json FTXPayData::toJson() const {
    nlohmann::json json;
    json["status"] = m_status;
    json["app"] = m_app;
    json["payment"] = m_payment;
    return json;
}

void FTXPayData::fromJson(const nlohmann::json &json) {
    readValue<std::string>(json, "status", m_status);
    m_app = json["app"];
    m_payment = json["payment"];
}

nlohmann::json Event::toJson() const {
    nlohmann::json json = m_subscriptionResponse.toJson();

    const TickerData *td = std::get_if<TickerData>(&m_eventData);
    const OrderBookData *odb = std::get_if<OrderBookData>(&m_eventData);
    const MarketsData *md = std::get_if<MarketsData>(&m_eventData);
    const TradesData *trd = std::get_if<TradesData>(&m_eventData);
    const FillData *fd = std::get_if<FillData>(&m_eventData);
    const OrderData *od = std::get_if<OrderData>(&m_eventData);
    const FTXPayData *ftd = std::get_if<FTXPayData>(&m_eventData);

    if (td != nullptr) {
        json["data"] = td->toJson();
    } else if (odb != nullptr) {
        json["data"] = odb->toJson();
    } else if (md != nullptr) {
        json["data"] = md->toJson();
    } else if (trd != nullptr) {
        json["data"] = trd->toJson();
    } else if (fd != nullptr) {
        json["data"] = fd->toJson();
    } else if (od != nullptr) {
        json["data"] = od->toJson();
    } else if (ftd != nullptr) {
        json["data"] = ftd->toJson();
    }

    return json;
}

void Event::fromJson(const nlohmann::json &json) {
    m_subscriptionResponse.fromJson(json);

    if (json.find("data") == json.end()) {
        return;
    }

    nlohmann::json dataObject = json["data"];

    switch (m_subscriptionResponse.m_channel) {
        case Channel::orderbook: {
            OrderBookData orderBookData;
            orderBookData.fromJson(dataObject);
            m_eventData = orderBookData;
            break;
        }
        case Channel::trades: {
            TradesData tradesData;
            tradesData.fromJson(dataObject);
            m_eventData = tradesData;
            break;
        }
        case Channel::ticker: {
            TickerData tickerData;
            tickerData.fromJson(dataObject);
            m_eventData = tickerData;
            break;
        }
        case Channel::markets: {
            MarketsData marketsData;
            marketsData.fromJson(dataObject);
            m_eventData = marketsData;
            break;
        }
        case Channel::fills: {
            FillData fillsData;
            fillsData.fromJson(dataObject);
            m_eventData = fillsData;
            break;
        }
        case Channel::orders: {
            OrderData ordersData;
            ordersData.fromJson(dataObject);
            m_eventData = ordersData;
            break;
        }
        case Channel::ftxpay: {
            FTXPayData ftxPayData;
            ftxPayData.fromJson(dataObject);
            m_eventData = ftxPayData;
            break;
        }
        case Channel::pingpong:
            break;
    }
}

nlohmann::json Ping::toJson() const {
    nlohmann::json json;
    json["op"] = m_op._to_string();
    return json;
}

void Ping::fromJson(const nlohmann::json &json) {
    readEnum<Operation>(json, "op", m_op);
}

nlohmann::json Pong::toJson() const {
    nlohmann::json json;
    json["op"] = m_op._to_string();
    return json;
}

void Pong::fromJson(const nlohmann::json &json) {
    readEnum<OperationResponse>(json, "op", m_op);
}
}