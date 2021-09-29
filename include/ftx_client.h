/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_FTXCLIENT_H
#define FTX_ZORRO_PLUGIN_FTXCLIENT_H

#include "http_session.h"
#include "ftx_models.h"
#include <string>
#include <memory>

class FTXClient {
    std::unique_ptr<HTTPSession> m_httpSession;

public:
    FTXClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    FTXAccount getAccountInfo();

    std::vector<FTXPosition> getPositions();
};

#endif //FTX_ZORRO_PLUGIN_FTXCLIENT_H
