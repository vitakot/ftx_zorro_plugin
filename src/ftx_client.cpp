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

http::response<http::string_body> checkResponse(const http::response<http::string_body> &response){
    if (response.result() != boost::beast::http::status::ok) {
        throw std::exception((std::string("Bad HTTP response: ") + std::to_string(response.result_int())).c_str());
    }
    return response;
}

FTXClient::FTXClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName) {

    m_httpSession = std::make_unique<HTTPSession>(API_URI, apiKey, apiSecret, subAccountName);
}

FTXAccount FTXClient::getAccountInfo() {

    FTXAccount retVal;
    const auto response = checkResponse(m_httpSession->methodGet("account"));

    // TODO: Make a template for this
    FTXResponse ftxResponse;
    ftxResponse.fromJson(nlohmann::json::parse(response.body()));

    if (ftxResponse.m_success) {
        retVal.fromJson(ftxResponse.m_result);
    }

    return retVal;
}

std::vector<FTXPosition> FTXClient::getPositions(){

    std::vector<FTXPosition> retVal;

    const auto response = checkResponse(m_httpSession->methodGet("positions"));

    return retVal;
}