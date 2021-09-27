/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ftx_models.h>
#include <iostream>
#include "ftx_client.h"

const char *API_URI = "ftx.com";

FTXClient::FTXClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName) {

    m_httpSession = std::make_unique<HTTPSession>(API_URI, apiKey, apiSecret, subAccountName);
}

Account FTXClient::getAccountInfo() {

    Account retVal;
    auto response = m_httpSession->methodGet("account");

    if (response.result() != boost::beast::http::status::ok) {
        throw std::exception((std::string("Invalid HTTP response: ") + std::to_string(response.result_int())).c_str());
    }

    // TODO: Make a template for this
    FTXResponse ftxResponse;
    ftxResponse.fromJson(nlohmann::json::parse(response.body()));

    if (ftxResponse.m_success) {
        retVal.fromJson(ftxResponse.m_result);
    }

    return retVal;
}
