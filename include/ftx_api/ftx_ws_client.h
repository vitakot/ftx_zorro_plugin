/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_WS_CLIENT_H
#define FTX_WS_CLIENT_H

#include <ftx_api/ftx_websocket.h>
#include <ftx_api/ftx_models.h>
#include <ftx_api/utils.h>
#include <spimpl.h>
#include <string>
#include <functional>

namespace ftx {

class WebSocketClient {

    struct P;
    spimpl::unique_impl_ptr<P> m_p{};

    using onMessageReceivedCB = std::function<void(const char *channel, const char *ptr, std::size_t size)>;

    using onEventCB = std::function<bool(const char *fl, int ec, std::string errmsg, Event msg)>;

    using onPongCB = std::function<bool(const char *fl, int ec, std::string errmsg, Pong msg)>;

public:

    WebSocketClient(const WebSocketClient &) = delete;

    WebSocketClient &operator=(const WebSocketClient &) = delete;

    WebSocketClient(WebSocketClient &&) noexcept = default;

    WebSocketClient &operator=(WebSocketClient &&) noexcept = default;

    WebSocketClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    ~WebSocketClient();

    /**
     * Compose full stream name from pair symbol and Channel
     * @param pair e.g. "BTCUSDT"
     * @param channel e.g. Channel::ticker
     * @return full stream name
     */
    static std::string composeStreamName(const std::string &pair, Channel channel);

    /**
     * Run the WebSocket IO Context asynchronously and returns immediately without blocking the thread execution
     */
    void run();

    /**
     * Run the WebSocket IO Context synchronously and block the thread execution
     */
    void runBlocking();

    /**
     * Check if the WebSocket IO Context
     * @return True if WebSocket IO Context is running
     */
    [[nodiscard]] bool isRunning() const;

    /**
     * Run the WebSocket IO Context synchronously for a specified amount of time and block the thread
     * execution for that time
     */
    void runFor(int seconds);

    /**
     * Run the WebSocket IO Context asynchronously for a specified amount of time and returns immediately
     * without blocking the thread execution
     */
    void runBlockingFor(int seconds);

    /**
     * Unsubscribe a WebSocket synchronously
     * @param h WebSocket handle
     */
    void unsubscribe(WebSocket::handle h);

    /**
     * Unsubscribe all WebSockets synchronously
     */
    void unsubscribeAll();

    /**
     * Set logger callback, if no set then all errors are writen to the stderr stream only
     * @param onLogMessageCB
     */
    void setLoggerCallback(const onLogMessage &onLogMessageCB);

    /**
     * Check if stream is already subscribed, if so then return corresponding WebSocket handle.
     * @param streamName a combination of Pair (e.g. BTCUSDT) and API stream name (e.g. bookTicker)
     * @return WebSocket handle
     */
    [[nodiscard]] WebSocket::handle findStream(const std::string &streamName);

    /**
     * Subscribe WebSocket to the Ticker channel
     * @param pair currency pair e.g. BTCUSDT
     * @param cb handle to process the incoming data
     * @return WebSocket handle
     */
    WebSocket::handle ticker(const std::string &pair, onEventCB cb);

    /**
     * Subscribe WebSocket to the Markets channel
     * @param pair currency pair e.g. BTCUSDT
     * @param cb handle to process the incoming data
     * @return WebSocket handle
     */
    WebSocket::handle markets(const std::string &pair, onEventCB cb);

    /**
     * Subscribe WebSocket to the Orders channel
     * @param cb handle to process the incoming data
     * @return WebSocket handle
     */
    WebSocket::handle orders(onEventCB cb);

    /**
     * Subscribe WebSocket to the Fills channel
     * @param cb handle to process the incoming data
     * @return WebSocket handle
     */
    WebSocket::handle fills(onEventCB cb);
};
}

#endif //FTX_WS_CLIENT_H
