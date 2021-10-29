/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "stdafx.h"
#include "ftx.h"
#include "utils.h"
#include <wtypes.h>
#include <string>
#include <ftx_client.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iomanip>
#include <algorithm>

#define PLUGIN_VERSION    2
#undef min

bool verbose = true;

static std::string currentSymbol;
static int lastOrderId = 0;
static int orderType = 0;
static double lotAmount = 1.0;
static int loopMs = 50;     // Actually unused
static int waitMs = 30000;  // Actually unused
static std::unique_ptr<FTXClient> ftxClient;

__int64 convertTime(DATE Date) {
    return (__int64) ((Date - 25569.) * 24. * 60. * 60.);
}

bool httpMethod(const std::string &url, const std::string &header, const std::string &body, std::string &response) {
    int n;
    int id = 0;

    if (body.empty()) {
        id = http_send(const_cast<char *>(url.data()), nullptr,
                       const_cast<char *>(header.data()));
    } else {
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

bool
httpDeleteMethod(const std::string &url, const std::string &header, const std::string &body, std::string &response) {
    return httpMethod(url, header, body, response);
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

    if (verbose) {
        spdlog::info("Calling BrokerLogin, user: {}, pswd: {}, type: {}, account: {}", User, Pwd, Type, Account);
    }

    if (!User) {
        return 0;
    } else if (((std::string) Type) == "Demo") {
        const auto msg = "Demo mode not supported by this plugin.";
        spdlog::error(msg);
        BrokerError(msg);
        return 0;
    } else {
        if (!ftxClient) {
            auto logger = spdlog::basic_logger_mt("ftx_logger", R"(C:\Users\Administrator\Zorro\Plugin\ftx.log)");
            //auto logger = spdlog::basic_logger_mt("ftx_logger", R"(C:\Users\vitez\Zorro\Plugin\ftx.log)");
            spdlog::set_default_logger(logger);
            spdlog::flush_on(spdlog::level::info);

            if (!std::string_view(User).empty() && !std::string_view(Pwd).empty()) {
                ftxClient = std::make_unique<FTXClient>(User, Pwd, Account);
                ftxClient->setHttpGetMethod(httpGetMethod);
                ftxClient->setHttpDeleteMethod(httpDeleteMethod);
                ftxClient->setHttpPostMethod(httpPostMethod);

                time_t Time;
                time(&Time);
                lastOrderId = (int) Time;
                spdlog::info("Logged into account: " + std::string(Account));
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
        if (verbose) {
            spdlog::info("Calling BrokerLogin end, user: {}, pswd: {}, type: {}, account: {}", User, Pwd, Type, Account);
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

    if (verbose) {
        spdlog::info("Calling BrokerAsset, asset: {}", Asset);
    }

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

DLLFUNC_C int BrokerAccount(char *Account, double *pdBalance, double *pdTradeVal, double *pdMarginVal) {

    if (verbose) {
        spdlog::info("Calling BrokerAccount, account: {}", Account);
    }

    if (!ftxClient) {
        spdlog::critical("FTX Client instance not initialized.");
        return 0;
    }

    try {
        const auto account = ftxClient->getAccountInfo();

        if (account) {
            if (pdBalance) {
                *pdBalance = std::round((*account).m_totalAccountValue);
            }
            return 1;
        }
    }
    catch (std::exception &e) {
        spdlog::error(e.what());
        BrokerError("Cannot acquire account info from server.");
    }

    if (verbose && pdBalance) {
        spdlog::info("Calling BrokerAccount end, account: {}, balance: {}", Account, *pdBalance);
    }

    return 0;
}

DLLFUNC_C int BrokerTime(DATE *pTimeGMT) {

    if (!ftxClient) {
        return 0;
    }
    return 2;
}

DLLFUNC_C int BrokerHistory2(char *Asset, DATE tStart, DATE tEnd, int nTickMinutes, int nTicks, T6 *ticks) {

    if (verbose) {
        spdlog::info("Calling BrokerHistory2, asset: {}, start: {}, end: {}, res_minutes: {}, ticks: {}", Asset,
                     convertTime(tStart), convertTime(tEnd), nTickMinutes, nTicks);
    }

    if (!Asset || !ticks || !nTicks) {
        return 0;
    }

    if (!ftxClient) {
        spdlog::critical("FTX Client instance not initialized.");
        return 0;
    }

    try {

        auto resolution = nTickMinutes * 60;

        if (!FTXClient::isValidCandleResolution(resolution)) {
            std::string msg = "Invalid data resolution: " + std::to_string(resolution) + ".";
            spdlog::error(msg);
            BrokerError(msg.c_str());
            return 0;
        }

        auto candles = ftxClient->getHistoricalPrices(Asset, resolution, convertTime(tStart), convertTime(tEnd));

        if (!candles) {
            std::string msg = "No historical data.";
            spdlog::error(msg);
            BrokerError(msg.c_str());
            return 0;
        }

        const auto maxCandles = std::min(nTicks, (int) (*candles).size());
        std::reverse((*candles).begin(), (*candles).end());

        // From most recent to oldest.
        for (int i = 0; i < maxCandles; i++, ticks++) {
            ticks->fOpen = static_cast<float>((*candles)[i].m_open);
            ticks->fHigh = static_cast<float>((*candles)[i].m_high);
            ticks->fLow = static_cast<float>((*candles)[i].m_low);
            ticks->fClose = static_cast<float>((*candles)[i].m_close);
            ticks->fVol = static_cast<float>((*candles)[i].m_volume);

            std::tm candleTime{};
            std::istringstream ss((*candles)[i].m_startTime);
            ss >> std::get_time(&candleTime, "%Y-%m-%dT%H:%M:%S:%z");
            ticks->time = systemTimeToVariantTimeMs(candleTime.tm_year + 1900, candleTime.tm_mon + 1,
                                                    candleTime.tm_mday,
                                                    candleTime.tm_hour, candleTime.tm_min, candleTime.tm_sec, 0);
        }

        return maxCandles;
    }
    catch (std::exception &e) {
        spdlog::error(e.what());
        BrokerError("Cannot acquire historical data from server.");
    }

    if (verbose) {
        spdlog::info("Calling BrokerHistory2 end, asset: {}", Asset);
    }

    return 0;
}

DLLFUNC_C int BrokerBuy2(char *Asset, int Amount, double dStopDist, double Limit, double *pPrice, int *pFill) {

    if (verbose) {
        spdlog::info("Calling BrokerBuy2, asset: {}, amount: {}, stopDist: {}, limit: {}", Asset, Amount, dStopDist,
                     Limit);
    }

    if (!ftxClient) {
        spdlog::critical("FTX Client instance not initialized.");
        return 0;
    }
    try {
        spdlog::info("New Order for asset: " + std::string(Asset) + ", amount: " + std::to_string(Amount) + ", size: " +
                     std::to_string(lotAmount * std::abs(Amount)) + ", limit: " + std::to_string(Limit));

        FTXOrder order;
        order.m_market = Asset;

        if (Amount > 0) {
            order.m_side = Side::Buy;
        } else {
            order.m_side = Side::Sell;
        }

        if (Limit > 0.) {
            order.m_price = Limit;
            order.m_type = OrderType::Limit;
        } else {
            order.m_type = OrderType::Market;
        }

        order.m_size = lotAmount * std::abs(Amount);

        if (orderType == 1) {
            order.m_ioc = true;
        }

        order.m_clientId = lastOrderId++;

        const auto confirmedOrder = ftxClient->placeOrder(order);

        if (!confirmedOrder) {
            std::string msg = "Cannot place order: " + std::string(Asset) + ", size: " + std::to_string(Amount);
            spdlog::error(msg);
            BrokerError(msg.c_str());
            return 0;
        }

        if (pPrice) {
            *pPrice = confirmedOrder->m_price;
        }
        if (pFill) {
            *pFill = confirmedOrder->m_filledSize / lotAmount;
        }
        spdlog::info("Order placed for asset: " + std::string(Asset) + ", filled size: " +
                     std::to_string(confirmedOrder->m_filledSize / lotAmount) + ", price" +
                     std::to_string(confirmedOrder->m_price) + ", clientId: " +
                     std::to_string(confirmedOrder->m_clientId));
        return confirmedOrder->m_clientId;
    }
    catch (std::exception &e) {
        spdlog::error(e.what());
        BrokerError("Cannot send order to server.");
    }

    if (verbose && pPrice && pFill) {
        spdlog::info("Calling BrokerBuy2 end, asset: {}, amount: {}, stopDist: {}, limit: {}, price: {}, fill: {}",
                     Asset,
                     Amount, dStopDist, Limit, *pPrice, *pFill);
    }

    return 0;
}
DLLFUNC_C double BrokerCommand(int Command, DWORD dwParameter) {

    if (verbose) {
        spdlog::info("Calling BrokerCommand, command: {}, parameter: {}", Command, dwParameter);
    }

    switch (Command) {
        case SET_ORDERTYPE:
            return orderType = dwParameter;
        case SET_DELAY:
            loopMs = dwParameter;
        case GET_DELAY:
            return loopMs;
        case SET_AMOUNT:
            lotAmount = *(double *) dwParameter;
            return 1;
        case SET_WAIT:
            waitMs = dwParameter;
        case GET_WAIT:
            return waitMs;
        case SET_SYMBOL:
            currentSymbol = (char *) dwParameter;
            return 1;
        case GET_POSITION:
            if (ftxClient) {
                try {
                    auto position = ftxClient->getPosition(currentSymbol);

                    if (position) {
                        return (*position).m_openSize;
                    }
                }
                catch (std::exception &e) {
                    spdlog::error(e.what());
                    BrokerError((std::string("Cannot get position of " + currentSymbol).c_str()));
                }
            }
            break;
        case GET_MAXREQUESTS:
            return 10;
        case GET_MAXTICKS:
            return 250;
        case GET_COMPLIANCE:
            return 2; // No hedging
        case DO_CANCEL:
            if (ftxClient) {
                try {
                    auto retVal = ftxClient->cancelOrder(dwParameter, true);

                    if (!retVal) {
                        BrokerError((std::string("Cannot cancel order id " + std::to_string(dwParameter)).c_str()));
                        return 0;
                    }
                    return 1;
                }
                catch (std::exception &e) {
                    spdlog::error(e.what());
                    BrokerError((std::string("Cannot cancel order id " + std::to_string(dwParameter)).c_str()));
                }
            }
            break;

        default:
            return 0;
    }

    return 0;
}

DLLFUNC_C void CreateDummyClient() {

    auto logger = spdlog::basic_logger_mt("ftx_logger", R"(C:\Users\vitez\Zorro\Plugin\ftx_test.log)");
    spdlog::set_default_logger(logger);
    spdlog::flush_on(spdlog::level::info);

    ftxClient = std::make_unique<FTXClient>("User", "Pwd", "Account");
    ftxClient->setHttpGetMethod(httpGetMethod);
    ftxClient->setHttpDeleteMethod(httpDeleteMethod);
    ftxClient->setHttpPostMethod(httpPostMethod);

    time_t Time;
    time(&Time);
    lastOrderId = (int) Time;
    spdlog::info("Logged into account: " + std::string("Account"));
}