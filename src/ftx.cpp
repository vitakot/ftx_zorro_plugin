/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "stdafx.h"
#include "ftx.h"
#include <wtypes.h>
#include <string>
#include <ftx_client.h>
#include <fstream>
#include <spdlog/spdlog.h>

#define PLUGIN_VERSION    2

bool verbose = true;
std::unique_ptr<FTXClient> ftxClient;

bool httpMethod(const std::string &url, const std::string &header, const std::string &body, std::string &response) {
    int n;
    int id = 0;

    if(body.empty()) {
        id = http_send(const_cast<char *>(url.data()), nullptr,
                           const_cast<char *>(header.data()));
    }
    else{
        id = http_send(const_cast<char *>(url.data()), const_cast<char *>(body.data()),
                       const_cast<char *>(header.data()));
    }

    if (!id) {
        if (verbose) {
            const auto msg = "Cannot connect to server.";
            spdlog::error(msg);
            BrokerError(msg);
        }
        return true;
    }

    while (!http_status(id)) {
        Sleep(100); // wait for the server to reply
        if (!BrokerProgress(1)) {
            if (verbose) {
                const auto msg = "BrokerProgress returned zero. Aborting...";
                spdlog::error(msg);
                BrokerError(msg);
            }
            http_free(id); //always clean up the id!
            return false; //failure
        } // print dots, abort if returns zero.
    }

    n = http_status(id);

    if (n > 0)  //transfer successful?
    {
        char *output;
        output = (char *) malloc(n + 1);
        http_result(id, output, n);   //get the replied IP
        response = output;
        free(output); //free up memory allocation
        http_free(id); //always clean up the id!
        return true; //success
    } else {
        if (verbose) {
            const auto msg = "Unknown error during transfer from server.";
            spdlog::error(msg);
            BrokerError(msg);
        }
        http_free(id); //always clean up the id!
        return false; //failure
    }
}

bool httpGetMethod(const std::string &url, const std::string &header, std::string &response) {
    return httpMethod(url, header, "", response);
}

bool httpDeleteMethod(const std::string &url, const std::string &header, std::string &response) {
    return httpMethod(url, header, "", response);
}

bool httpPostMethod(const std::string &url, const std::string &header, const std::string &body, std::string &response) {
    return httpMethod(url, header, body, response);
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
        return 0;
    } else if (((std::string) Type) == "Demo") {
        const auto msg = "Demo mode not supported by this plugin.";
        spdlog::error(msg);
        BrokerError(msg);
        return 0;
    } else {
        if (!ftxClient) {
            if (!std::string_view(User).empty() && !std::string_view(Pwd).empty()) {
                ftxClient = std::make_unique<FTXClient>(User, Pwd, Account);
                ftxClient->setHttpGetMethod(httpGetMethod);
                ftxClient->setHttpDeleteMethod(httpDeleteMethod);
                ftxClient->setHttpPostMethod(httpPostMethod);
            } else {
                const auto msg = "Missing or Incomplete Account credentials.";
                spdlog::error(msg);
                BrokerError(msg);
                return 0;
            }
        } else {
            ftxClient->setCredentials(User, Pwd, Account);
        }
    }

    try {
        const auto account = ftxClient->getAccountInfo();
        if (!account) {
            const auto msg = "Cannot log into account: " + std::string(Account) + ".";
            spdlog::error(msg);
            return 0;
        }
        return 1;
    }
    catch (std::exception &e) {
        spdlog::error(e.what());
        return 0;
    }
}

DLLFUNC_C int
BrokerAsset(char *Asset, double *pPrice, double *pSpread, double *pVolume, double *pPip, double *pPipCost,
            double *pLotAmount, double *pMarginCost, double *pRollLong, double *pRollShort) {

    if (!ftxClient) {
        spdlog::critical("FTX Client instance not initialized.");
        return 0;
    }

    try {
        const auto market = ftxClient->getMarket(Asset);

        if (market) {
            if ((*market).m_ask == 0.0 || (*market).m_bid == 0.0) {
                return 0;
            }

            if (pPrice) {
                *pPrice = (*market).m_ask;
            }
            if (pSpread) {
                *pSpread = (*market).m_ask - (*market).m_bid;
            }
            if (pVolume) {
                *pVolume = (*market).m_quoteVolume24h;
            }

            return 1;
        }

        auto msg = ftxClient->getlastError();

        if (msg.empty()) {
            msg = "Cannot acquire asset info from server.";
            BrokerError(msg.c_str());
        }
        spdlog::error(msg);
        BrokerError(msg.c_str());
    }
    catch (std::exception &e) {
        spdlog::error(e.what());
        BrokerError("Cannot acquire asset info from server.");
    }

    return 0;
}

DLLFUNC int BrokerAccount(char *Account, double *pdBalance, double *pdTradeVal, double *pdMarginVal) {
    if (!ftxClient) {
        spdlog::critical("FTX Client instance not initialized.");
        return 0;
    }

    try {
        const auto account = ftxClient->getAccountInfo();

        if (account) {
            if (pdBalance) {
                *pdBalance = (*account).m_totalAccountValue;
            }
            if (pdTradeVal) {
                *pdTradeVal = (*account).m_totalPositionSize;
            }
            if (pdMarginVal) {
                // TODO: TBD
            }
            return 1;
        }
    }
    catch (std::exception &e) {
        spdlog::error(e.what());
        BrokerError("Cannot acquire account info from server.");
    }

    return 0;
}


DLLFUNC_C int BrokerTime(DATE *pTimeGMT) {

    // TODO: Implement ping
    if (!ftxClient) {
        return 0;
    }
    return 2;
}

DLLFUNC_C int BrokerHistory2(char *Asset, DATE tStart, DATE tEnd, int nTickMinutes, int nTicks, T6 *ticks) {
    return 0;
}

DLLFUNC_C int BrokerBuy(char *Asset, int nAmount, double dStopDist, double *pPrice) {
    return 0;
}
DLLFUNC_C double BrokerCommand(int Command, DWORD dwParameter) {

    static std::string currentSymbol;

    switch (Command) {
        case SET_DELAY:
            break;

        case SET_SYMBOL:
            currentSymbol = (char *) dwParameter;
            return 1;
        case GET_POSITION:
            if (ftxClient) {
                auto position = ftxClient->getPosition(currentSymbol);

                if (position) {
                    return (*position).m_openSize;
                }
            }
            break;
        case GET_MAXREQUESTS:
            return 10;
        case GET_MAXTICKS:
            return 500;
    }

    return 0;
}