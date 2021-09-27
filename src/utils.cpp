/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "utils.h"
#include <openssl/hmac.h>

constexpr char hexMap[] = {'0',
                           '1',
                           '2',
                           '3',
                           '4',
                           '5',
                           '6',
                           '7',
                           '8',
                           '9',
                           'a',
                           'b',
                           'c',
                           'd',
                           'e',
                           'f'};

std::string hmacString(const std::string &secret, std::string msg, std::size_t signedLen) {

    return {reinterpret_cast<const char *const>(HMAC(EVP_sha256(), secret.data(), secret.size(),
                                                     reinterpret_cast<const unsigned char *>(msg.data()), msg.size(),
                                                     nullptr, nullptr)), signedLen};
}

std::string stringToHex(const unsigned char *data, std::size_t len) {
    std::string s(len * 2, ' ');
    for (std::size_t i = 0; i < len; ++i) {
        s[2 * i] = hexMap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexMap[data[i] & 0x0F];
    }
    return s;
}