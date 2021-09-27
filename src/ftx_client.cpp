/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "ftx_client.h"

const char *API_URI = "ftx.com";

FTXClient::FTXClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName) {

    m_httpSession = std::make_unique<HTTPSession>(API_URI, apiKey, apiSecret, subAccountName);
}

void FTXClient::getAccountInfo() {

    auto response = m_httpSession->methodGet("accosunt");
    auto prd = response.result();
    std::string bd = response.body();
}
