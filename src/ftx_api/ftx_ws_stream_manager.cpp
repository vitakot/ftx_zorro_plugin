/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ftx_api/ftx_ws_stream_manager.h>
#include <ftx_api/ftx_ws_client.h>
#include <mutex>

using namespace std::chrono_literals;

namespace ftx {

struct WSStreamManager::P {
    std::unique_ptr<WebSocketClient> m_wsClient;
    int m_timeout = 30;
    mutable std::recursive_mutex m_tickerLocker;
    mutable std::recursive_mutex m_fillsLocker;
    mutable std::recursive_mutex m_ordersLocker;
    std::map<std::string, TickerData> m_tickPrices;
    std::vector<FillData> m_fillsData;
    std::vector<OrderData> m_ordersData;
    onLogMessage m_logMessageCB;

    explicit P(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName) {
        m_wsClient = std::make_unique<WebSocketClient>(apiKey, apiSecret, subAccountName);
    }

    static std::string formatMessage(const Event &msg) {
        std::string msgString;

        if (msg.m_subscriptionResponse.m_type != +OperationResponse::error) {
            msgString = std::format("Channel: {}, {}",
                                    msg.m_subscriptionResponse.m_channel._to_string(),
                                    msg.m_subscriptionResponse.m_type._to_string());
        } else {
            msgString = std::format("Channel: {}, {}, code: {}, msg: {}",
                                    msg.m_subscriptionResponse.m_channel._to_string(),
                                    msg.m_subscriptionResponse.m_type._to_string(),
                                    msg.m_subscriptionResponse.m_code,
                                    msg.m_subscriptionResponse.m_msg);

        }

        return msgString;
    }
};

WSStreamManager::WSStreamManager(const std::string &apiKey, const std::string &apiSecret,
                                 const std::string &subAccountName) : m_p(
        spimpl::make_unique_impl<P>(apiKey, apiSecret, subAccountName)) {
}

void WSStreamManager::subscribeTickerStream(const std::string &pair, bool force) {

    auto handle = m_p->m_wsClient->findStream(WebSocketClient::composeStreamName(pair, Channel::ticker));

    if (handle && force) {
        m_p->m_wsClient->unsubscribe(handle);
    } else if (handle) {
        return;
    }

    handle = m_p->m_wsClient->ticker(pair, [&](const char *fl, int ec, const std::string &errmsg,
                                               const Event &msg) -> bool {
                                         if (ec) {

                                             if (m_p->m_logMessageCB) {
                                                 const auto msgString = std::format(
                                                         "ticker: fl={}, ec={}, errmsg: {}", fl, ec, errmsg);
                                                 m_p->m_logMessageCB(LogSeverity::Error, msgString);
                                             }

                                             return false;
                                         }

                                         std::lock_guard<std::recursive_mutex> lk(m_p->m_tickerLocker);
                                         const TickerData *td = std::get_if<TickerData>(&msg.m_eventData);

                                         if (td != nullptr) {
                                             m_p->m_tickPrices.insert_or_assign(msg.m_subscriptionResponse.m_market, *td);
                                         } else {
                                             m_p->m_logMessageCB(LogSeverity::Info, m_p->formatMessage(msg));
                                         }
                                         return true;
                                     }
    );

    if (!m_p->m_wsClient->isRunning()) {
        m_p->m_wsClient->run();
    }
}

void WSStreamManager::subscribeOrdersStream(bool force) {
    auto handle = m_p->m_wsClient->findStream(WebSocketClient::composeStreamName("", Channel::orders));

    if (handle && force) {
        m_p->m_wsClient->unsubscribe(handle);
    } else if (handle) {
        return;
    }

    handle = m_p->m_wsClient->orders([&](const char *fl, int ec, const std::string &errmsg,
                                         const Event &msg) -> bool {
                                         if (ec) {

                                             if (m_p->m_logMessageCB) {
                                                 const auto msgString = std::format(
                                                         "orders: fl={}, ec={}, errmsg: {}", fl, ec, errmsg);
                                                 m_p->m_logMessageCB(LogSeverity::Error, msgString);
                                             }

                                             return false;
                                         }

                                         std::lock_guard<std::recursive_mutex> lk(m_p->m_ordersLocker);
                                         const OrderData *od = std::get_if<OrderData>(&msg.m_eventData);

                                         if (od != nullptr) {
                                             m_p->m_ordersData.push_back(*od);
                                         } else {
                                             m_p->m_logMessageCB(LogSeverity::Info, m_p->formatMessage(msg));
                                         }
                                         return true;
                                     }
    );

    if (!m_p->m_wsClient->isRunning()) {
        m_p->m_wsClient->run();
    }
}

void WSStreamManager::subscribeFillsStream(bool force) {
    auto handle = m_p->m_wsClient->findStream(WebSocketClient::composeStreamName("", Channel::fills));

    if (handle && force) {
        m_p->m_wsClient->unsubscribe(handle);
    } else if (handle) {
        return;
    }

    handle = m_p->m_wsClient->fills([&](const char *fl, int ec, const std::string &errmsg,
                                        const Event &msg) -> bool {
                                        if (ec) {

                                            if (m_p->m_logMessageCB) {
                                                const auto msgString = std::format(
                                                        "fills: fl={}, ec={}, errmsg: {}", fl, ec, errmsg);
                                                m_p->m_logMessageCB(LogSeverity::Error, msgString);
                                            }

                                            return false;
                                        }

                                        std::lock_guard<std::recursive_mutex> lk(m_p->m_fillsLocker);
                                        const FillData *fd = std::get_if<FillData>(&msg.m_eventData);

                                        if (fd != nullptr) {
                                            m_p->m_fillsData.push_back(*fd);
                                        } else {

                                            m_p->m_logMessageCB(LogSeverity::Info, m_p->formatMessage(msg));
                                        }
                                        return true;
                                    }
    );

    if (!m_p->m_wsClient->isRunning()) {
        m_p->m_wsClient->run();
    }
}

void WSStreamManager::pingAll() {
    m_p->m_wsClient->pingAll();
}

void WSStreamManager::setTimeout(int seconds) {
    m_p->m_timeout = seconds;
}

int WSStreamManager::timeout() const {
    return m_p->m_timeout;
}

void WSStreamManager::setLoggerCallback(const onLogMessage &onLogMessageCB) {
    m_p->m_logMessageCB = onLogMessageCB;
    m_p->m_wsClient->setLoggerCallback(onLogMessageCB);
}

std::optional<TickerData> WSStreamManager::readTickerData(const std::string &pair) {

    int numTries = 0;
    int maxNumTries = static_cast<int>(m_p->m_timeout / 0.01);

    while (numTries <= maxNumTries) {
        m_p->m_tickerLocker.lock();
        const auto it = m_p->m_tickPrices.find(pair);

        if (it != m_p->m_tickPrices.end()) {
            auto retVal = it->second;
            m_p->m_tickerLocker.unlock();
            return retVal;
        }
        m_p->m_tickerLocker.unlock();
        numTries++;
        std::this_thread::sleep_for(10ms);
    }

    return {};
}

std::optional<FillData> WSStreamManager::readFillData(const Order &order) {
    int numTries = 0;
    int maxNumTries = static_cast<int>(m_p->m_timeout / 0.01);

    while (numTries <= maxNumTries) {
        m_p->m_fillsLocker.lock();
        std::vector<FillData>::iterator it;

        for (it = m_p->m_fillsData.begin(); it != m_p->m_fillsData.end(); it++) {
            if ((*it).m_id == order.m_id || (*it).m_id == std::stoll(order.m_clientId)) {
                FillData fillData = *it;
                m_p->m_fillsData.erase(it);
                m_p->m_fillsLocker.unlock();
                return fillData;
            }
        }

        m_p->m_fillsLocker.unlock();
        numTries++;
        std::this_thread::sleep_for(3ms);
    }

    return {};
}

std::optional<OrderData> WSStreamManager::readOrderData(const Order &order) {
    int numTries = 0;
    int maxNumTries = static_cast<int>(m_p->m_timeout / 0.01);

    while (numTries <= maxNumTries) {
        m_p->m_ordersLocker.lock();
        std::vector<OrderData>::iterator it;

        for (it = m_p->m_ordersData.begin(); it != m_p->m_ordersData.end(); it++) {
            if ((*it).m_id == order.m_id || (*it).m_id == std::stoll(order.m_clientId)) {
                OrderData orderData = *it;
                m_p->m_ordersData.erase(it);
                m_p->m_ordersLocker.unlock();
                return orderData;
            }
        }

        m_p->m_ordersLocker.unlock();
        numTries++;
        std::this_thread::sleep_for(3ms);
    }

    return {};
}
}