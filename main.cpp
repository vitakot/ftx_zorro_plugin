/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <fstream>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ftx_client.h>
#include "utils.h"

int main(int argc, char **argv) {

    std::unique_ptr<FTXClient> ftxClient;
    std::filesystem::path pathToCfg{"C:/Users/vitez/Documents/ftx.cfg"};
    std::ifstream ifs(pathToCfg.string());

    if (!ifs.is_open()) {
        std::cerr << "Couldn't open config file";
        return 1;
    }

    try {
        std::string apiKey;
        std::string apiSecret;
        std::string subAccountName;

        nlohmann::json json = nlohmann::json::parse(ifs);
        readValue<std::string>(json, "ApiKey", apiKey);
        readValue<std::string>(json, "ApiSecret", apiSecret);
        readValue<std::string>(json, "SubAccountName", subAccountName);

        ftxClient = std::make_unique<FTXClient>(apiKey, apiSecret, subAccountName);
    }

    catch (nlohmann::json::exception &e) {
        std::cerr << "Couldn't parse config file";
        ifs.close();
        return 1;
    }

    catch(std::exception &e){
        std::cerr << e.what();
    }

    auto ret = ftxClient->getAccountInfo();

    return 0;
}