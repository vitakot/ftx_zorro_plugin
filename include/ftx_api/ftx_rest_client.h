/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_REST_CLIENT_H
#define FTX_REST_CLIENT_H

#include "ftx_models.h"
#include <string>
#include <memory>
#include <functional>
#include <optional>
#include <spimpl.h>

namespace ftx {
class HTTPSession;

class RESTClient {

    struct P;
    spimpl::unique_impl_ptr<P> m_p{};

public:

    RESTClient(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    /**
     * Set credentials to the RESTClient instance
     * @param apiKey
     * @param apiSecret
     * @param subAccountName
     */
    void setCredentials(const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    /**
     * Helper for ensuring valid candle resolution - 15, 60, 300, 900, 3600, 14400, 86400, or any
     * multiple of 86400 up to 30*86400
     * @param resolution
     * @return True if valid resolution
     */
    bool static isValidCandleResolution(std::int32_t resolution);

    /**
     * Get Account information - https://docs.ftx.com/#account
     * @return Account structure
     */
    [[nodiscard]] Account getAccountInfo() const;

    /**
     * Get single Market information - https://docs.ftx.com/#get-single-market
     * @param name market name e.g. BTC-PERP
     * @return Market structure
     */
    [[nodiscard]] Market getMarket(const std::string &name) const;

    /**
     * Get Position information - https://docs.ftx.com/#get-positions
     * @param name market name e.g. BTC-PERP
     * @return Position structure
     */
    [[nodiscard]] Position getPosition(const std::string &name) const;

    /**
     * Get Positions information - https://docs.ftx.com/#get-positions
     * @return array of Position structures
     */
    [[nodiscard]] std::vector<Position> getPositions() const;

    /**
     * Place order
     * @param order Order structure filled with the requested order parameters
     * @return ACK Order structure filled by the server
     */
    [[nodiscard]] Order placeOrder(const Order &order) const;

    /**
     * Cancel order
     * @param id order id - can be either API order ID or a client order ID based on the isClientId parameter
     * @param isClientId a flag specifying the meaning of the id parameter (API order ID or a client order ID)
     * @return True if successful
     */
    [[nodiscard]] bool cancelOrder(std::int32_t id, bool isClientId = false) const;

    /**
     * Get Order status
     * @param id order id - can be either API order ID or a client order ID based on the isClientId parameter
     * @param isClientId a flag specifying the meaning of the id parameter (API order ID or a client order ID)
     * @return Order structure - the same structure is used also for placing new orders.
     */
    [[nodiscard]] Order getOrderStatus(std::int32_t id, bool isClientId) const;

    /**
     * Cancel all orders for a given market
     * @param market market name e.g. BTC-PERP
     * @return True if successful
     */
    [[nodiscard]] bool cancelAllOrders(const std::string &market) const;

    /**
     * Download historical candles
     * @param marketName market name e.g. BTC-PERP
     * @param resolutionInSecs 15, 60, 300, 900, 3600, 14400, 86400, or any multiple of 86400 up to 30*86400
     * @param from timestamp in s, must be smaller then "to"
     * @param to timestamp in s, must be greater then "from"
     * @return array of Candle structures
     */
    [[nodiscard]] std::vector<Candle>
    getHistoricalPrices(const std::string &marketName, std::int32_t resolutionInSecs, std::int64_t from,
                        std::int64_t to) const;
};
}
#endif //FTX_REST_CLIENT_H
