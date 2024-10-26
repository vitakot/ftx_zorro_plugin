/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ftx_api/ftx_ws_client.h>
#include <ftx_api/utils.h>
#include <openssl/hmac.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/callable_traits.hpp>
#include <variant>
#include <iostream>
#include <openssl/sha.h>

namespace ftx {

const char *FTX_FUTURES_WS_HOST = "ftx.com";
const char *FTX_FUTURES_WS_PORT = "443";

struct WebSocketClient::P {
    boost::asio::io_context m_ioContext;
    std::string m_host = {FTX_FUTURES_WS_HOST};
    std::string m_port = {FTX_FUTURES_WS_PORT};
    onMessageReceivedCB m_onMessageCallback;
    std::map<WebSocket::handle, std::weak_ptr<WebSocket>> m_map;
    std::thread m_ioThread;
    std::atomic<bool> m_isRunning = false;
    onLogMessage m_logMessageCB;
    std::string m_apiKey;
    std::string m_apiSecret;
    std::string m_subAccountName;
    const EVP_MD *m_evp_md;

    P() : m_evp_md(EVP_sha256()) {
        m_onMessageCallback = {};
    }

    static bool isApiError(const nlohmann::json &json) {
        return json.contains("error") && json.contains("code") && json.contains("msg");
    }

    static std::pair<int, std::string> constructError(const nlohmann::json &json) {

        int ec;
        std::string msg;
        readValue<int>(json, "code", ec);
        readValue<std::string>(json, "msg", msg);

        return std::make_pair(ec, std::move(msg));
    }

    static nlohmann::json createRequest(const std::string &pair, Channel channel) {

        if (channel == +Channel::pingpong) {
            Ping request;
            return request.toJson();
        } else {
            ChannelSubscriptionRequest request;
            request.m_op = Operation::subscribe;
            request.m_channel = channel;
            request.m_market = pair;
            return request.toJson();
        }
    }

    nlohmann::json createAuthenticationRequest() const {
        AuthenticationRequest request;
        request.m_key = m_apiKey;
        request.m_subAccount = m_subAccountName;
        request.m_time = getMsTimestamp(currentTime()).count();

        std::string authString(std::to_string(request.m_time));
        authString.append("websocket_login");

        unsigned char digest[SHA256_DIGEST_LENGTH];
        unsigned int digestLength = 32;

        HMAC(m_evp_md, m_apiSecret.data(), m_apiSecret.size(),
             reinterpret_cast<const unsigned char *>(authString.data()),
             authString.length(), digest, &digestLength);

        request.m_sign = stringToHex(digest, sizeof(digest));

        return request.toJson();
    }

    template<typename F>
    WebSocket::handle startChannel(const std::string &pair, Channel channel, F cb) {
        using argsTuple = typename boost::callable_traits::args<decltype(cb)>::type;
        using messageType = typename std::tuple_element<3, argsTuple>::type;

        auto ws = std::make_shared<WebSocket>(m_ioContext, m_logMessageCB);
        auto *h = ws.get();
        std::weak_ptr<WebSocket> wp{ws};

        std::string streamName = composeStreamName(pair, channel);
        std::vector<nlohmann::json> requests;

        requests.push_back(createRequest(pair, channel));

        if (channel == +Channel::orders || channel == +Channel::fills || channel == +Channel::ftxpay) {
            requests.push_back(createAuthenticationRequest());
        }

        ws->setStreamName(streamName);

        auto wsCallback = [this, streamName, requests, cb = std::move(cb)]
                (const char *fl, int ec, std::string errmsg, const char *ptr, std::size_t size) -> bool {
            if (ec) {
                try {
                    cb(fl, ec, std::move(errmsg), messageType{});
                } catch (const std::exception &ex) {

                    if (m_logMessageCB) {
                        m_logMessageCB(LogSeverity::Error, std::format("{}: {}\n", MAKE_FILELINE, ex.what()));
                    }
                }

                return false;
            }

            const nlohmann::json json = nlohmann::json::parse(ptr);

            if (json.is_object() && isApiError(json)) {
                auto error = constructError(json);
                auto errorCode = error.first;
                auto errorMsg = std::move(error.second);

                try {
                    messageType message{};
                    return cb(MAKE_FILELINE, errorCode, std::move(errorMsg), std::move(message));
                } catch (const std::exception &ex) {

                    if (m_logMessageCB) {
                        m_logMessageCB(LogSeverity::Error, std::format("{}: {}\n", MAKE_FILELINE, ex.what()));
                    }
                }
            }

            try {
                if (m_onMessageCallback) { m_onMessageCallback(streamName.c_str(), ptr, size); }
            } catch (const std::exception &ex) {
                if (m_logMessageCB) {
                    m_logMessageCB(LogSeverity::Error, std::format("{}: {}\n", MAKE_FILELINE, ex.what()));
                }
            }

            try {
                messageType message;
                message.fromJson(json);
                return cb(fl, ec, std::move(errmsg), std::move(message));
            } catch (const std::exception &ex) {
                if (m_logMessageCB) {
                    m_logMessageCB(LogSeverity::Error, std::format("{}: {}\n", MAKE_FILELINE, ex.what()));
                }
            }

            return false;
        };

        h->start(
                m_host, m_port, requests, std::move(wsCallback), std::move(ws)
        );

        removeDeadWebsockets();

        m_map.emplace(h, std::move(wp));

        return h;
    }

    template<typename F>
    void stopChannelImpl(WebSocket::handle h, F f) {
        auto it = m_map.find(h);
        if (it == m_map.end()) { return; }

        if (auto s = it->second.lock()) {
            f(s);
        }

        m_map.erase(it);

        removeDeadWebsockets();
    }

    void stopChannel(WebSocket::handle h) {
        return stopChannelImpl(h, [](const auto &sp) { sp->stop(); });
    }

    template<typename F>
    void unsubscribe_all_impl(F f) {
        for (auto it = m_map.begin(); it != m_map.end();) {
            if (auto s = it->second.lock()) {
                f(s);
            }

            it = m_map.erase(it);
        }
    }

    void unsubscribeAll() {
        return unsubscribe_all_impl([](const auto &sp) { sp->stop(); });
    }

    void removeDeadWebsockets() {
        for (auto it = m_map.begin(); it != m_map.end();) {
            if (!it->second.lock()) {
                it = m_map.erase(it);
            } else {
                ++it;
            }
        }
    }
};

WebSocketClient::WebSocketClient(const std::string &apiKey, const std::string &apiSecret,
                                 const std::string &subAccountName) : m_p(spimpl::make_unique_impl<P>()) {
    m_p->m_apiKey = apiKey;
    m_p->m_apiSecret = apiSecret;
    m_p->m_subAccountName = subAccountName;
}

WebSocketClient::~WebSocketClient() {
    if (m_p->m_ioThread.joinable()) {
        m_p->m_ioThread.join();
    }
}

std::string WebSocketClient::composeStreamName(const std::string &pair, Channel channel) {
    std::string res;
    if (!pair.empty()) {
        res += pair;
        if (pair != "!") {
            boost::algorithm::to_lower(res);
        }

        res += '@';
    }

    res += channel._to_string();

    return res;
}

bool WebSocketClient::isRunning() const {
    return m_p->m_isRunning;
}

void WebSocketClient::run() {

    if (m_p->m_isRunning) {
        return;
    }

    m_p->m_isRunning = true;

    if (m_p->m_ioThread.joinable()) {
        m_p->m_ioThread.join();
    }

    m_p->m_ioThread = std::thread([&] {
        for (;;) {
            try {
                m_p->m_isRunning = true;
                if (m_p->m_ioContext.stopped()) {
                    m_p->m_ioContext.restart();
                }
                m_p->m_ioContext.run();
                m_p->m_isRunning = false;
                break;
            }
            catch (std::exception &e) {
                m_p->m_ioContext.restart();

                if (m_p->m_logMessageCB) {
                    m_p->m_logMessageCB(LogSeverity::Error, std::format("{}: {}\n", MAKE_FILELINE, e.what()));
                }
            }
        }
        m_p->m_isRunning = false;
    });
}

void WebSocketClient::runBlocking() {

    for (;;) {
        try {
            m_p->m_isRunning = true;
            if (m_p->m_ioContext.stopped()) {
                m_p->m_ioContext.restart();
            }
            m_p->m_ioContext.run();
            m_p->m_isRunning = false;
            break;
        }
        catch (std::exception &e) {
            m_p->m_ioContext.restart();

            if (m_p->m_logMessageCB) {
                m_p->m_logMessageCB(LogSeverity::Error, std::format("{}: {}\n", MAKE_FILELINE, e.what()));
            }
        }
    }
}

void WebSocketClient::unsubscribe(WebSocket::handle h) {
    return m_p->stopChannel(h);
}

void WebSocketClient::unsubscribeAll() {
    return m_p->unsubscribeAll();
}

void WebSocketClient::setLoggerCallback(const onLogMessage &onLogMessageCB) {
    m_p->m_logMessageCB = onLogMessageCB;
}

WebSocket::handle WebSocketClient::findStream(const std::string &streamName) {

    m_p->removeDeadWebsockets();

    for (const auto &el: m_p->m_map) {
        if (auto s = el.second.lock()) {
            if (streamName == s->streamName()) {
                return el.first;
            }
        }
    }

    return nullptr;
}

void WebSocketClient::runFor(int seconds) {

    if (m_p->m_ioThread.joinable()) {
        return;
    }

    m_p->m_ioThread = std::thread([&] {

        boost::asio::steady_timer unsubscribe_timer{m_p->m_ioContext};
        unsubscribe_timer.expires_after(std::chrono::seconds{seconds});
        unsubscribe_timer.async_wait(
                [this](const boost::system::error_code &) {
                    unsubscribeAll();
                }
        );
        m_p->m_isRunning = true;
        m_p->m_ioContext.run();
        m_p->m_isRunning = false;
    });
}

void WebSocketClient::runBlockingFor(int seconds) {

    boost::asio::steady_timer unsubscribe_timer{m_p->m_ioContext};
    unsubscribe_timer.expires_after(std::chrono::seconds{seconds});
    unsubscribe_timer.async_wait(
            [this](const boost::system::error_code &) {
                unsubscribeAll();
            }
    );
    m_p->m_isRunning = true;
    m_p->m_ioContext.run();
    m_p->m_isRunning = false;
}

WebSocket::handle WebSocketClient::ticker(const std::string &pair, onEventCB cb) {
    return m_p->startChannel(pair, Channel::ticker, std::move(cb));
}

WebSocket::handle WebSocketClient::markets(const std::string &pair, onEventCB cb) {
    return m_p->startChannel(pair, Channel::markets, std::move(cb));
}

WebSocket::handle WebSocketClient::orders(onEventCB cb) {
    return m_p->startChannel("", Channel::orders, std::move(cb));
}

WebSocket::handle WebSocketClient::fills(onEventCB cb) {
    return m_p->startChannel("", Channel::fills, std::move(cb));
}
}