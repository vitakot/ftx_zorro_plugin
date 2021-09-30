/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "stdafx.h"
#include "ftx.h"
#include "sha256.h"
#include <wtypes.h>
#include <string>

#define PLUGIN_VERSION    2

std::string consumerKey;
std::string consumerSecret;
std::string accountId;

enum class HTTPMethod : int {
    Get = 0,
    Post,
    Delete
};

/**
 * Prepare request header - each request must contain encrypted credentials
 */
std::string createHeader(std::string) {
//    std::string method(req.method_string());
//    std::string path(req.target());
//    std::string body(req.body());
//
//    auto ts = getMsTimestamp(currentTime()).count();
//    std::string data = std::to_string(ts) + method + path;
//
//    if (!body.empty()) {
//        data += body;
//    }
//
//    std::string hmaStr = hmacString(std::string(m_apiSecret), data, 32);
//    std::string sign = stringToHex((unsigned char *) hmaStr.c_str(), 32);
//
//    req.set("FTX-KEY", m_apiKey);
//    req.set("FTX-TS", std::to_string(ts));
//    req.set("FTX-SIGN", sign);
//
//    if (!m_subAccountName.empty()) {
//        req.set("FTX-SUBACCOUNT", m_subAccountName);
//    }

    return "";
}

DLLFUNC_C int BrokerOpen(char *Name, FARPROC fpError, FARPROC fpProgress) {
    strcpy_s(Name, 32, "FTX");
    (FARPROC &) BrokerError = fpError;
    (FARPROC &) BrokerProgress = fpProgress;
    return PLUGIN_VERSION;
}

DLLFUNC_C void BrokerHTTP(FARPROC fpSend, FARPROC fpStatus, FARPROC fpResult, FARPROC fpFree) {
    (FARPROC &) http_send = fpSend;
    (FARPROC &) http_status = fpStatus;
    (FARPROC &) http_result = fpResult;
    (FARPROC &) http_free = fpFree;
}

DLLFUNC_C int BrokerLogin(char *User, char *Pwd, char *Type, char *Account) {

    if (!User) {
        // log out
        return 0;
    } else if (((std::string) Type) == "Demo") {
        BrokerError("Demo mode not supported by this plugin.");
        return 0;
    } else {
        // Username is being provided
        if (accountId.empty()) {
            accountId = User;
        }
        if (consumerKey.empty() || consumerSecret.empty()) {
            std::string pwd;
            pwd = Pwd;
            if ((pwd.length() % 4 == 0) && (pwd.length() > 0)) {
                auto len = pwd.length() / 4;
                consumerKey = pwd.substr(0, len);
                consumerSecret = pwd.substr(1 * len, len);
            } else {
                BrokerError(
                        "Error: Password must be divisible \n\ninto four equal strings.\n\nThe password is: consumerKey,consumerSecret,\n\noauthToken,oauthTokenSecret \n\nwith each item back-to-back (no commas).");
                return 0;
            }
        }
    }

//    std::string response;
//    if (!GetResponse(&response, HGET, ACCOUNTS_ID_BALANCES, XML, "", "")) {
//        return 0;
//    }
//
//    if (diag) {
//        SaveMessage(response, "BrokerLogin");
//    }
//
//    if (ConfirmAccount(response)) {
//        return 1; // success!
//    } else {
//        return 0;
//    }

    return 0;
}

DLLFUNC_C int BrokerTime(DATE *pTimeGMT) {
    return 0;
}

DLLFUNC_C int BrokerAsset(char *Asset, double *pPrice, double *pSpread, double *pVolume, double *pPip, double *pPipCost,
                          double *pLotAmount, double *pMarginCost, double *pRollLong, double *pRollShort) {
    return 0;
}

DLLFUNC_C int BrokerHistory2(char *Asset, DATE tStart, DATE tEnd, int nTickMinutes, int nTicks, T6 *ticks) {
    return 0;
}

DLLFUNC_C int BrokerBuy(char *Asset, int nAmount, double dStopDist, double *pPrice) {
    return 0;
}
DLLFUNC_C double BrokerCommand(int Command, DWORD dwParameter) {
    return 0;
}