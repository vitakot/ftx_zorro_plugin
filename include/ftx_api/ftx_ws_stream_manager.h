/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_WS_STREAM_MANAGER_H
#define FTX_WS_STREAM_MANAGER_H

#include <ftx_api/utils.h>
#include <ftx_api/ftx_models.h>
#include <optional>
#include <spimpl.h>

namespace ftx {

class WSStreamManager {

    struct P;
    spimpl::unique_impl_ptr<P> m_p{};

public:

    WSStreamManager(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    /**
     * Check if the Ticker Stream is already subscribed for a selected pair, if not then subscribe it. When force parameter
     * is true then re-subscribe if ef already subscribed
     * @param pair e.g BTCUSDT
     * @param force If true then re-subscribe if already subscribed
     */
    void subscribeTickerStream(const std::string &pair, bool force = false);

    /**
     * Check if the Orders Stream is already subscribed, if not then subscribe it. When force parameter
     * is true then re-subscribe even if already subscribed
     * @param force If true then re-subscribe if already subscribed
     */
    void subscribeOrdersStream(bool force = false);

    /**
     * Check if the Fills Stream is already subscribed, if not then subscribe it. When force parameter
     * is true then re-subscribe even if already subscribed
     * @param force If true then re-subscribe if already subscribed
     */
    void subscribeFillsStream(bool force = false);

    /**
     * Set time of all reading operations
     * @param seconds
     */
    void setTimeout(int seconds);

    /**
     * Get time of all reading operations
     * @return
     */
    [[nodiscard]] int timeout() const;

    /**
     * Set logger callback, if no set then all errors are writen to the stderr stream only
     * @param onLogMessageCB
     */
    void setLoggerCallback(const onLogMessage &onLogMessageCB);

    /**
     * Try to read TickerData structure. It will block at most Timeout time.
     * @param pair
     * @return TickerData structure if successful
     */
    [[nodiscard]] std::optional<TickerData> readTickerData(const std::string &pair);

    /**
     * For keeping the communication alive - should be sent each 15 seconds
     */
    void pingAll();

    /**
     * Waits for FillData with given OrderId received via WebSocket and read it. It will block at most Timeout time.
     * @param order An ACK response returned when placing order by REST
     * @return FillData structure if successful
     */
    [[nodiscard]] std::optional<FillData> readFillData(const Order &order);

    /**
     * Waits for OrderData with given OrderId received via WebSocket and read it. It will block at most Timeout time.
     * @param order An ACK response returned when placing order by REST
     * @return OrderData structure if successful
     */
    [[nodiscard]] std::optional<OrderData> readOrderData(const Order &order);
};

}

#endif //FTX_WS_STREAM_MANAGER_H