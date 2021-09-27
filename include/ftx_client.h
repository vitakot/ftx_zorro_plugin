/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_FTXCLIENT_H
#define FTX_ZORRO_PLUGIN_FTXCLIENT_H

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <string>
#include "httplib.h"

class FTXClient {

    std::unique_ptr<httplib::Client> m_httpClient;
    const std::string m_apiKey;
    const std::string m_apiSecret;
    const std::string m_subAccountName;

    httplib::Headers prepareHeaders(const std::string &method, const std::string &path, const std::string &body, const std::string &subAccountName);

public:
    FTXClient(std::string apiKey, std::string apiSecret, std::string subAccountName);
    void getAccountInfo();
};

#endif //FTX_ZORRO_PLUGIN_FTXCLIENT_H
