/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "stdafx.h"
#include "ftx.h"
#include <ftx_api/utils.h>
#include <ftx_api/ftx_rest_client.h>
#include <ftx_api/ftx_ws_stream_manager.h>
#include <wtypes.h>
#include <string>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iomanip>
#include <algorithm>

#define PLUGIN_VERSION    2
#undef min

using namespace std::chrono_literals;

bool verbose = true;

static std::string currentSymbol;
static int lastOrderId = 0;
static int orderType = 0;
static double lotAmount = 1.0;
static int loopMs = 50;     // Actually unused
static int waitMs = 30000;  // Actually unused
static std::unique_ptr<ftx::RESTClient> ftxClient;
static std::unique_ptr<ftx::WSStreamManager> streamManager;
static int64_t lastKeepAliveTime = 0;

enum ExchangeStatus {
    Unavailable = 0,
    Closed = 1,
    Open = 2
};

__int64 convertTime(DATE Date) {
    return (__int64) ((Date - 25569.) * 24. * 60. * 60.);
}

DLLFUNC_C int BrokerOpen(char *Name, FARPROC fpError, FARPROC fpProgress) {
    strcpy_s(Name, 32, "FTX");
    (FARPROC &) BrokerError = fpError;
    (FARPROC &) BrokerProgress = fpProgress;
    return PLUGIN_VERSION;
}

void logFunction(ftx::LogSeverity severity, const std::string &errmsg) {

    switch (severity) {
        case ftx::LogSeverity::Info:
            spdlog::info(errmsg);
            break;
        case ftx::LogSeverity::Warning:
            spdlog::warn(errmsg);
            break;
        case ftx::LogSeverity::Critical:
            spdlog::critical(errmsg);
            break;
        case ftx::LogSeverity::Error:
            spdlog::error(errmsg);
            break;
        case ftx::LogSeverity::Debug:
            spdlog::debug(errmsg);
            break;
        case ftx::LogSeverity::Trace:
            spdlog::trace(errmsg);
            break;
    }
}

DLLFUNC_C int BrokerLogin(char *User, char *Pwd, char *Type, char *Account) {

    if (!User) {
        streamManager.reset();
        ftxClient.reset();
        spdlog::info("Logout");
        spdlog::shutdown();
        return 1;
    } else if (((std::string) Type) == "Demo") {
        const auto msg = "Demo mode not supported by this plugin.";
        spdlog::error(msg);
        BrokerError(msg);
        return 0;
    } else {
        if (!ftxClient) {
            auto logger = spdlog::basic_logger_mt("ftx_logger", R"(./Log/ftx.log)");
            spdlog::set_default_logger(logger);
            spdlog::flush_on(spdlog::level::info);

            if (!std::string_view(User).empty() && !std::string_view(Pwd).empty()) {
                ftxClient = std::make_unique<ftx::RESTClient>(User, Pwd, Account);
                time_t Time;
                time(&Time);
                lastOrderId = (int) Time;
                spdlog::info("Logged into account: {}", Account);
            } else {
                const auto msg = "Missing or Incomplete Account credentials.";
                spdlog::error(msg);
                BrokerError(msg);
                return 0;
            }
        } else {
            ftxClient->setCredentials(User, Pwd, Account);
        }

        if (!streamManager) {
            streamManager = std::make_unique<ftx::WSStreamManager>(User, Pwd, Account);
            streamManager->setLoggerCallback(&logFunction);

            lastKeepAliveTime = duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();

        }
    }

    try {
        const auto account = ftxClient->getAccountInfo();

        if (verbose) {
            spdlog::info("Calling BrokerLogin end, user: {}, pswd: {}, type: {}, account: {}", User, Pwd, Type,
                         Account);
        }

        return 1;
    }
    catch (std::exception &e) {
        spdlog::error("Cannot log into account: {}, reason: {}", Account, e.what());
        return 0;
    }
}

DLLFUNC_C int
BrokerAsset(char *Asset, double *pPrice, double *pSpread, double *pVolume, double *pPip, double *pPipCost,
            double *pLotAmount, double *pMarginCost, double *pRollLong, double *pRollShort) {

    /// NOTE: Do not log normal state, this function is called every second!
    if (!ftxClient) {
        spdlog::critical("FTX Client instance not initialized.");
        return 0;
    }

    if (pPip != nullptr) {
        try {
            const auto market = ftxClient->getMarket(Asset);

            if (market.m_ask == 0.0 || market.m_bid == 0.0) {
                return 0;
            }

            if (pPrice) {
                *pPrice = market.m_ask;
            }
            if (pSpread) {
                *pSpread = market.m_ask - market.m_bid;
            }
            if (pVolume) {
                *pVolume = market.m_quoteVolume24h;
            }
            if (pPip) {
                *pPip = market.m_sizeIncrement;
            }
            if (pLotAmount) {
                *pLotAmount = market.m_minProvideSize;
            }
            if (pPipCost) {
                *pPipCost = market.m_minProvideSize * market.m_sizeIncrement;
            }
            return 1;
        }
        catch (std::exception &e) {
            spdlog::error("Cannot acquire asset info from server, reason: {}", e.what());
            BrokerError("Cannot acquire asset info from server.");
        }
    } else {
        try {
            /// Subscribe stream for Asset - if not already subscribed
            streamManager->subscribeTickerStream(Asset);

            const auto tickPrice = streamManager->readTickerData(Asset);

            if (tickPrice) {

                const auto tickerPrice = *tickPrice;

                if (tickerPrice.m_ask == 0.0 || tickerPrice.m_bid == 0.0) {
                    return 0;
                }

                if (pPrice) {
                    *pPrice = tickerPrice.m_ask;
                }
                if (pSpread) {
                    *pSpread = tickerPrice.m_ask - tickerPrice.m_bid;
                }
                if (pVolume) {
                    *pVolume = tickerPrice.m_askSize + tickerPrice.m_bidSize;
                }

                return 1;
            }
        }
        catch (std::exception &e) {
            spdlog::error("Cannot acquire asset info from server, reason: {}", e.what());
            BrokerError("Cannot acquire asset info from server.");
        }
    }
    return 0;
}

DLLFUNC_C int BrokerAccount(char *Account, double *pdBalance, double *pdTradeVal, double *pdMarginVal) {

    if (!ftxClient) {
        spdlog::critical("FTX Client instance not initialized.");
        return 0;
    }

    try {
        const auto account = ftxClient->getAccountInfo();

        if (pdBalance) {
            *pdBalance = std::round(account.m_totalAccountValue);
        }
        return 1;
    }
    catch (std::exception &e) {
        spdlog::error("Cannot acquire account info from server, reason: {}", e.what());
        BrokerError("Cannot acquire account info from server.");
    }

    if (verbose && pdBalance) {
        spdlog::info("Calling BrokerAccount end, account: {}, balance: {}", Account, *pdBalance);
    }

    return 0;
}

DLLFUNC_C int BrokerTime(DATE *pTimeGMT) {

    if (!ftxClient) {
        return ExchangeStatus::Unavailable;
    }

    const auto currentTime = duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    /// Send keep alive message after each 15 seconds
    if ((lastKeepAliveTime + 15 * 1000) < currentTime) {
        if (streamManager) {
            streamManager->pingAll();
        }

        lastKeepAliveTime = currentTime;
    }

    /// FTX Exchange never closes
    return ExchangeStatus::Open;
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

        if (!ftx::RESTClient::isValidCandleResolution(resolution)) {
            std::string msg = "Invalid data resolution: " + std::to_string(resolution) + ".";
            spdlog::error(msg);
            BrokerError(msg.c_str());
            return 0;
        }

        int64_t offsetStart = convertTime(tEnd) - nTicks * resolution;
        auto candles = ftxClient->getHistoricalPrices(Asset, resolution, offsetStart, convertTime(tEnd));

        const auto maxCandles = std::min(nTicks, (int) candles.size());
        std::reverse(candles.begin(), candles.end());

        /// From most recent to oldest.
        for (int i = 0; i < maxCandles; i++, ticks++) {
            ticks->fOpen = static_cast<float>(candles[i].m_open);
            ticks->fHigh = static_cast<float>(candles[i].m_high);
            ticks->fLow = static_cast<float>(candles[i].m_low);
            ticks->fClose = static_cast<float>(candles[i].m_close);
            ticks->fVol = static_cast<float>(candles[i].m_volume);

            std::tm candleTime{};
            std::istringstream ss(candles[i].m_startTime);
            ss >> std::get_time(&candleTime, "%Y-%m-%dT%H:%M:%S:%z");
            ticks->time = ftx::systemTimeToVariantTimeMs(candleTime.tm_year + 1900, candleTime.tm_mon + 1,
                                                         candleTime.tm_mday,
                                                         candleTime.tm_hour, candleTime.tm_min, candleTime.tm_sec, 0);
        }

        return maxCandles;
    }
    catch (std::exception &e) {
        spdlog::error("Cannot acquire historical data from server, reason: {}", e.what());
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

        ftx::Order order;
        order.m_market = Asset;

        if (Amount > 0) {
            order.m_side = ftx::Side::buy;
        } else {
            order.m_side = ftx::Side::sell;
        }

        if (Limit > 0.) {
            order.m_price = Limit;
            order.m_type = ftx::OrderType::limit;
        } else if (dStopDist != 0.0 && dStopDist != -1) {
            order.m_price = Limit;
            order.m_triggerPrice = dStopDist;
            order.m_type = ftx::OrderType::stop;
        } else {
            order.m_type = ftx::OrderType::market;
        }

        order.m_size = lotAmount * std::abs(Amount);

        if (order.m_type == +ftx::OrderType::market) {
            order.m_ioc = true;
        }

        order.m_clientId = std::to_string(lastOrderId++);

        const auto confirmedOrder = ftxClient->placeOrder(order);

        ftx::Order ackOrder;
        int maxAttempts = 10;
        int attemptNo = 0;

        while (ackOrder.m_status != +ftx::OrderStatus::closed) {

            ackOrder = ftxClient->getOrderStatus(stoi(confirmedOrder.m_clientId), true);
            attemptNo++;

            if (attemptNo == maxAttempts) {
                spdlog::error("Cannot send order to server, reason: order was not closed/filled");
                BrokerError("Cannot send order to server.");
                return 0;
            }

            std::this_thread::sleep_for(500ms);
        }

        if (pPrice) {
            *pPrice = ackOrder.m_avgFillPrice;
        }
        if (pFill) {
            *pFill = std::round(ackOrder.m_filledSize / lotAmount);
        }
        spdlog::info("Order placed for asset: " + std::string(Asset) + ", filled size: " +
                     std::to_string(confirmedOrder.m_filledSize / lotAmount) + ", price" +
                     std::to_string(confirmedOrder.m_price) + ", clientId: " + confirmedOrder.m_clientId);

        return stoi(confirmedOrder.m_clientId);
    }
    catch (std::exception &e) {
        spdlog::error("Cannot send order to server, reason: {}", e.what());
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
                const char *symbol = (char *) dwParameter;
                try {
                    auto position = ftxClient->getPosition(symbol);
                    return position.m_openSize;
                }
                catch (std::exception &e) {
                    const auto msg = std::string("Cannot get position of " + std::string(symbol));
                    spdlog::error("{}, reason: {}", msg, e.what());
                    BrokerError(msg.c_str());
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