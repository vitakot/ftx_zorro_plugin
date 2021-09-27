/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "ftx_client.h"

#include <utility>
#include "utils.h"
#include <boost/asio/ip/tcp.hpp>

FTXClient::FTXClient(std::string apiKey, std::string apiSecret, std::string subAccountName)
        : m_apiKey(std::move(apiKey)), m_apiSecret(std::move(apiSecret)), m_subAccountName(std::move(subAccountName)) {

    //m_httpClient = std::make_unique<httplib::Client>("https://ftx.com/api");
}

void FTXClient::getAccountInfo() {

//    auto headers = prepareHeaders("GET", "/account", "", m_subAccountName);
//
//    httplib::SSLClient cli("https://www.stonky.cz/"); // host
//    cli.enable_server_certificate_verification(true);
//    if (auto res = cli.Get("/account",headers)) {
//        auto pr = res->status;
//        auto pr2 = res->body;
//    }else{
//        int prd = 5;
//    }

}

//httplib::Headers
//FTXClient::prepareHeaders(const std::string &method, const std::string &path, const std::string &body,
//                          const std::string &subAccountName) {
//
//    httplib::Headers headers;
//
//    auto ts = getMsTimestamp(currentTime()).count();
//    std::string data = std::to_string(ts) + method + path;
//    std::string hmacHashString = hmacString(std::string(m_apiSecret), data, 32);
//    std::string sign = stringToHex((unsigned char *) hmacHashString.c_str(), 32);
//
//    headers = {
//            {"FTX-KEY",  m_apiKey},
//            {"FTX-TS",   std::to_string(ts)},
//            {"FTX-SIGN", sign}
//    };
//
//    if (!subAccountName.empty()) {
//        headers.insert(std::make_pair("FTX-SUBACCOUNT", "subAccountName"));
//    }
//
//    return headers;
//}
