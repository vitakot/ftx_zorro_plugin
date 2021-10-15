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
std::optional<ValueType> handleFTXResponse(const std::string &response) {
    ValueType retVal;
    FTXResponse ftxResponse;
    ftxResponse.fromJson(nlohmann::json::parse(response));

    if (ftxResponse.m_success) {
        retVal.fromJson(ftxResponse.m_result);
    }

    return {};
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

std::optional<FTXAccount> FTXClient::getAccountInfo() const {

    std::string response;
    std::string path = "/api/account";
    std::string header = createHeader("GET", path, std::string());
    std::string url = std::string(API_URI) + path;

    if (m_httpGetMethod(url, header, response)) {
        return handleFTXResponse<FTXAccount>(response);
    } else {
        throw std::exception("HTTP Get Method failed.");
    }
}

std::optional<FTXMarket> FTXClient::getMarket(const std::string &name) const {

    std::string response;
    std::string path = "/api/markets/" + name;
    std::string header = createHeader("GET", path, std::string());
    std::string url = std::string(API_URI) + path;

    if (m_httpGetMethod(url, header, response)) {
        return handleFTXResponse<FTXMarket>(response);
    } else {
        throw std::exception("HTTP Get Method failed.");
    }
}

std::optional<FTXPosition> FTXClient::getPosition(const std::string &symbol) const {

    // FTX API does not provide an endpoint for a single position
    const auto positions = getPositions();

    for(const auto &position : positions){
        if(position.m_future == symbol){
            return position;
        }
    }

    return {};
}

std::vector<FTXPosition> FTXClient::getPositions() const {
    std::string response;
    std::string path = "/api/positions";
    std::string header = createHeader("GET", path, std::string());
    std::string url = std::string(API_URI) + path;

    if (m_httpGetMethod(url, header, response)) {
        const auto responseData = handleFTXResponse<FTXPositions>(response);
        return (*responseData).m_positions;
    } else {
        throw std::exception("HTTP Get Method failed.");
    }
}
