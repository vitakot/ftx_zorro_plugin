/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ftx_models.h>
#include <iostream>
#include <sstream>
#include "ftx_client.h"
#include "utils.h"
#include "sha256.h"

const char *API_URI = "ftx.com";

template<typename ValueType>
ValueType handleFTXResponse(const std::string &response) {
    ValueType retVal;
    FTXResponse ftxResponse;
    ftxResponse.fromJson(nlohmann::json::parse(response));

    if (ftxResponse.m_success) {
        retVal.fromJson(ftxResponse.m_result);
    } else {
        throw std::exception("Bad FTXResponse: success == false");
    }

    return retVal;
}

FTXClient::FTXClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName) {
    m_apiKey = apiKey;
    m_apiSecret = apiSecret;
    m_subAccountName = subAccountName;
}

void FTXClient::setHttpGetMethod(
        const std::function<bool(const std::string &, const std::string &, std::string &)> &method) {
    m_httpGetMethod = method;
}

void
FTXClient::setCredentials(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName) {
    m_apiKey = apiKey;
    m_apiSecret = apiSecret;
    m_subAccountName = subAccountName;
}

std::string FTXClient::createHeader(const std::string &method, const std::string &path, const std::string &body) const {

    auto ts = getMsTimestamp(currentTime()).count();
    std::string data = std::to_string(ts) + method + path;

    if (!body.empty()) {
        data += body;
    }

    unsigned char digest[32];
    HMAC256(std::string(m_apiSecret), (unsigned char *) data.data(), data.length(), digest);
    std::string sign = stringToHex(digest, sizeof(digest));

    std::stringstream headerStream;
    headerStream << "FTX-KEY:" << m_apiKey << std::endl;
    headerStream << "FTX-TS:" << std::to_string(ts) << std::endl;
    headerStream << "FTX-SIGN:" << sign << std::endl;

    if (!m_subAccountName.empty()) {
        headerStream << "FTX-SUBACCOUNT:" << m_subAccountName << std::endl;
    }

    return headerStream.str();
}

FTXAccount FTXClient::getAccountInfo() {

    std::string header = createHeader("GET", "/api/account", std::string());
    std::string response;
    std::string url = std::string(API_URI) + "/api/account";

    if (m_httpGetMethod(url, header, response)) {
        return handleFTXResponse<FTXAccount>(response);
    } else {
        throw std::exception("HTTP Get Method failed.");
    }
}

//std::vector<FTXPosition> FTXClient::getPositions() {
//
//    std::vector<FTXPosition> retVal;
//    const auto response = checkResponse(m_httpSession->methodGet("positions"));
//    return retVal;
//}
//
//std::vector<FTXMarket> FTXClient::getMarkets() {
//    const auto response = checkResponse(m_httpSession->methodGet("markets"));
//    const auto markets = handleFTXResponse<FTXMarkets>(response);
//    return markets.m_markets;
//}
//
//std::vector<FTXCandle>
//FTXClient::getHistoricalPrices(const std::string &marketName, std::int32_t resolutionInSecs, std::int64_t from,
//                               std::int64_t to) {
//    std::vector<FTXCandle> retVal;
//    std::stringstream pathStream;
//    pathStream << "markets/" << marketName << "/candles" << "?resolution=" << resolutionInSecs << "&start_time=" << from
//               << "&end_time=" << to;
//    const auto response = checkResponse(m_httpSession->methodGet(pathStream.str()));
//    const auto candles = handleFTXResponse<FTXCandles>(response);
//    return candles.m_candles;
//}
