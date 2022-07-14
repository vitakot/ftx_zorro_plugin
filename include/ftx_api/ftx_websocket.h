/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_WEBSOCKET_H
#define FTX_WEBSOCKET_H

#include <spimpl.h>
#include <string>
#include <functional>
#include <ftx_api/ftx_models.h>
#include <ftx_api/utils.h>

namespace boost::asio {
class io_context;
}

namespace ftx {

class WebSocket : public std::enable_shared_from_this<WebSocket> {

    struct P;
    spimpl::unique_impl_ptr<P> m_p{};

    /// when 'false' returned the stop will called
    using onMessageReceivedCB = std::function<bool(const char *fl, int ec, std::string errmsg, const char *ptr,
                                                   std::size_t size)>;

    using holderType = std::shared_ptr<void>;

public:
    using handle = void *;

    explicit WebSocket(boost::asio::io_context &ioContext, const onLogMessage &onLogMessageCB);

    virtual ~WebSocket() = default;

    void start(const std::string &host, const std::string &port, const std::vector<nlohmann::json> &requests, onMessageReceivedCB cb,
               holderType holder);

    void stop();

    std::string streamName() const;

    void setStreamName(const std::string &streamName);
};

}
#endif //FTX_WEBSOCKET_H
