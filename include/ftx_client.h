/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_FTXCLIENT_H
#define FTX_ZORRO_PLUGIN_FTXCLIENT_H

#include "ftx_models.h"
#include <string>
#include <memory>
#include <functional>
#include <optional>

class FTXClient {

    std::string m_apiKey;
    std::string m_apiSecret;
    std::string m_subAccountName;
    mutable std::string m_lastError;

    /**
     * Prepare request header - each request must contain encrypted credentials
     * @return HTTP header as a string
     */
    [[nodiscard]] std::string createHeader(const std::string &method, const std::string &path, const std::string &body) const;

    std::function<bool(const std::string &url, const std::string &header, std::string &response)> m_httpGetMethod;

    std::function<bool(const std::string &url, const std::string &header,  const std::string &body, std::string &response)> m_httpDeleteMethod;

    std::function<bool(const std::string &url, const std::string &header, const std::string &body, std::string &response)> m_httpPostMethod;

public:

    FTXClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    void setCredentials(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    void setHttpGetMethod(const std::function<bool(const std::string &, const std::string &, std::string &)> &method);

    void setHttpDeleteMethod(const std::function<bool(const std::string &, const std::string &, const std::string &body, std::string &)> &method);

    void setHttpPostMethod(const std::function<bool(const std::string &, const std::string &, const std::string &body, std::string &)> &method);

    std::string getlastError() const;

    bool static isValidCandleResolution(std::int32_t resolution);

    [[nodiscard]] std::optional<FTXAccount> getAccountInfo() const;

    [[nodiscard]] std::optional<FTXMarket> getMarket(const std::string &name) const;

    [[nodiscard]] std::optional<FTXPosition> getPosition(const std::string &symbol) const;

    [[nodiscard]] std::optional<std::vector<FTXPosition>> getPositions() const;

    [[nodiscard]] std::optional<FTXOrder> placeOrder(const FTXOrder &order) const;

    [[nodiscard]] bool cancelOrder(std::int32_t id, bool isClientId = false) const;

    [[nodiscard]] std::optional<FTXOrder> getOrderStatus(std::int32_t id) const;

    [[nodiscard]] bool cancelAllOrders(const std::string &market) const;

    [[nodiscard]] std::optional<std::vector<FTXCandle>> getHistoricalPrices(const std::string& marketName, std::int32_t resolutionInSecs, std::int64_t from, std::int64_t to) const;
};

#endif //FTX_ZORRO_PLUGIN_FTXCLIENT_H
