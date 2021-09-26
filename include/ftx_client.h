/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_FTXCLIENT_H
#define FTX_ZORRO_PLUGIN_FTXCLIENT_H

#include <string>

class FTXClient {

  //  util::HTTPSession http_client;
    const std::string uri = "ftx.com";
    const std::string api_key = "";
    const std::string api_secret = "";
    const std::string subaccount_name = "";

public:

    void getAccountInfo();

};


#endif //FTX_ZORRO_PLUGIN_FTXCLIENT_H
