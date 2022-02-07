/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ftx_api/utils.h>
#include <ftx_api/ftx_models.h>
#include <ftx_api/ftx_rest_client.h>
#include <ftx_api/ftx_http_session.h>

namespace ftx {

const char *API_URI = "ftx.com";

struct RESTClient::P {
    std::shared_ptr<HTTPSession> m_httpSession;
    std::string m_apiKey;
    std::string m_apiSecret;
    std::string m_subAccountName;

    [[nodiscard]] std::vector<Candle>
    getHistoricalPrices(const std::string &marketName, std::int32_t resolutionInSecs, std::int64_t from,
                        std::int64_t to) const;
};

template<typename ValueType>
ValueType handleFTXResponse(const http::response<http::string_body> &response) {
    ValueType retVal;
    Response ftxResponse;
    ftxResponse.fromJson(nlohmann::json::parse(response.body()));

    if constexpr(!std::is_same<Response, ValueType>::value) {
        if (ftxResponse.m_success) {
            retVal.fromJson(ftxResponse.m_result);
            return retVal;
        } else {
            throw std::exception(std::format("FTX API error: {}", ftxResponse.m_error).c_str());
        }
    } else {
        if (ftxResponse.m_success) {
            retVal.m_success = true;
            return retVal;
        } else {
            throw std::exception(std::format("FTX API error: {}", ftxResponse.m_error).c_str());
        }
    }
}

http::response<http::string_body> checkResponse(const http::response<http::string_body> &response) {
    if (response.result() != boost::beast::http::status::ok) {
        throw std::exception(std::format("Bad response, code {}, msg: {}", response.result_int(), response.body()).c_str());
    }
    return response;
}

RESTClient::RESTClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName)
        : m_p(spimpl::make_unique_impl<P>()) {
    m_p->m_apiKey = apiKey;
    m_p->m_apiSecret = apiSecret;
    m_p->m_subAccountName = subAccountName;

    m_p->m_httpSession = std::make_shared<HTTPSession>(API_URI, m_p->m_apiKey, m_p->m_apiSecret, m_p->m_subAccountName);
}

bool RESTClient::isValidCandleResolution(std::int32_t resolution) {

    if (resolution > 86400) {
        return !(resolution % 86400);
    }

    switch (resolution) {
        case 15:
        case 60:
        case 300:
        case 900:
        case 3600:
        case 14400:
        case 86400:
            return true;
        default:
            return false;
    }
}

void
RESTClient::setCredentials(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName) {
    m_p->m_apiKey = apiKey;
    m_p->m_apiSecret = apiSecret;
    m_p->m_subAccountName = subAccountName;

    m_p->m_httpSession.reset();
    m_p->m_httpSession = std::make_shared<HTTPSession>(API_URI, m_p->m_apiKey, m_p->m_apiSecret, m_p->m_subAccountName);
}

Account RESTClient::getAccountInfo() const {

    const auto response = checkResponse(m_p->m_httpSession->methodGet("account"));
    return handleFTXResponse<Account>(response);
}

Market RESTClient::getMarket(const std::string &name) const {

    const auto response = checkResponse(m_p->m_httpSession->methodGet("markets/" + name));
    return handleFTXResponse<Market>(response);
}

Position RESTClient::getPosition(const std::string &name) const {

    /// FTX API does not provide an endpoint for a single position
    const auto positions = getPositions();

    for (const auto &position: positions) {
        if (position.m_future == name) {
            return position;
        }
    }

    return {};
}

std::vector<Position> RESTClient::getPositions() const {

    const auto response = checkResponse(m_p->m_httpSession->methodGet("positions"));
    const auto responseData = handleFTXResponse<Positions>(response);
    return responseData.m_positions;
}

Order RESTClient::placeOrder(const Order &order) const {

    const auto response = checkResponse(m_p->m_httpSession->methodPost("orders", order.toJson().dump()));
    return handleFTXResponse<Order>(response);
}

bool RESTClient::cancelOrder(std::int32_t id, bool isClientId) const {

    std::string path;

    if (!isClientId) {
        path = "orders/" + std::to_string(id);
    } else {
        path = "orders/by_client_id/" + std::to_string(id);
    }

    const auto response = checkResponse(m_p->m_httpSession->methodDelete(path));
    return handleFTXResponse<Response>(response).m_success;
}

Order RESTClient::getOrderStatus(std::int32_t id, bool isClientId) const {

    std::string path;

    if (!isClientId) {
        path = "orders/" + std::to_string(id);
    } else {
        path = "orders/by_client_id/" + std::to_string(id);
    }

    const auto response = checkResponse(m_p->m_httpSession->methodGet(path));
    return handleFTXResponse<Order>(response);
}

bool RESTClient::cancelAllOrders(const std::string &market) const {

    const auto response = checkResponse(m_p->m_httpSession->methodDelete("orders"));
    return handleFTXResponse<Response>(response).m_success;
}

std::vector<Candle>
RESTClient::P::getHistoricalPrices(const std::string &marketName, std::int32_t resolutionInSecs, std::int64_t from,
                                std::int64_t to) const {

    std::stringstream pathStream;
    pathStream << "markets/" << marketName << "/candles" << "?resolution=" << resolutionInSecs << "&start_time="
               << from << "&end_time=" << to;

    const auto response = checkResponse(m_httpSession->methodGet(pathStream.str()));
    const auto responseData = handleFTXResponse<Candles>(response);

    return responseData.m_candles;
}

std::vector<Candle>
RESTClient::getHistoricalPrices(const std::string &marketName, std::int32_t resolutionInSecs, std::int64_t from,
                                std::int64_t to) const {

    std::vector<Candle> retVal;
    std::int64_t lastTo = to;
    std::vector<Candle> candles;

    if (from < lastTo) {
        candles = m_p->getHistoricalPrices(marketName, resolutionInSecs, from, lastTo);
    }
    while (!candles.empty()) {

        const auto first = candles.front();
        const auto last = candles.back();

        retVal.insert(retVal.begin(), candles.begin(), candles.end());
        lastTo = getTimeStampFromString(candles.front().m_startTime, "%Y-%m-%dT%H:%M:%S:%z") - resolutionInSecs;
        candles.clear();

        if (from < lastTo) {
            candles = m_p->getHistoricalPrices(marketName, resolutionInSecs, from, lastTo);
        }
    }

    return retVal;
}
}