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

class FTXClient {

    std::string m_apiKey;
    std::string m_apiSecret;
    std::string m_subAccountName;

    /**
     * Prepare request header - each request must contain encrypted credentials
     * @return HTTP header as a string
     */
    [[nodiscard]] std::string createHeader(const std::string &method, const std::string &path, const std::string &body) const;

    std::function<bool(const std::string &url, const std::string &header, std::string &response)> m_httpGetMethod;

public:

    FTXClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    void setCredentials(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    void setHttpGetMethod(const std::function<bool(const std::string &, const std::string &, std::string &)> &method);

    FTXAccount getAccountInfo();

    std::vector<FTXPosition> getPositions();

    std::vector<FTXMarket> getMarkets();

    std::vector<FTXCandle>
    getHistoricalPrices(const std::string &marketName, std::int32_t resolutionInSecs, std::int64_t from,
                        std::int64_t to);
};

#endif //FTX_ZORRO_PLUGIN_FTXCLIENT_H
