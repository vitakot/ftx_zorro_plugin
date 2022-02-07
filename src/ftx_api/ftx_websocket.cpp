/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ftx_api/ftx_websocket.h>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>

using namespace std::chrono_literals;

namespace ftx {

#define FTX_CB_ON_ERROR(cb, ec) \
    cb(__FILE__ "(" BOOST_PP_STRINGIZE(__LINE__) ")", (ec).value(), (ec).message(), nullptr, 0);

struct WebSocket::P {

    boost::asio::ssl::context m_ssl;
    boost::asio::ip::tcp::resolver m_resolver;
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> m_ws;
    boost::beast::multi_buffer m_buf;
    std::string m_host;
    bool m_stopRequested;
    std::string m_streamName;
    boost::beast::multi_buffer m_wBuffer;
    std::vector<nlohmann::json> m_requests;
    std::atomic<bool> m_pingSent = true;

    explicit P(boost::asio::io_context &ioContext) : m_ssl{boost::asio::ssl::context::sslv23_client},
                                                     m_resolver{ioContext}, m_ws{ioContext, m_ssl}, m_buf{},
                                                     m_stopRequested{} {
    }

    void
    asyncStart(const std::string &host, const std::string &port, const std::vector<nlohmann::json> &requests,
               onMessageReceivedCB cb,
               holderType holder) {
        m_host = host;
        m_requests = requests;

        if (!m_requests.empty()) {
            auto msg = m_requests.back().dump();
            m_requests.pop_back();
            size_t n = buffer_copy(m_wBuffer.prepare(msg.size()), boost::asio::buffer(msg));
            m_wBuffer.commit(n);
        }

        m_resolver.async_resolve(
                m_host, port, [this, cb = std::move(cb), holder = std::move(holder)]
                        (boost::system::error_code ec,
                         const boost::asio::ip::tcp::resolver::results_type &res) mutable {
                    if (ec) {
                        if (!m_stopRequested) {
                            FTX_CB_ON_ERROR(cb, ec);
                        }
                    } else {
                        asyncConnect(res, std::move(cb), std::move(holder));
                    }
                }
        );
    }

    void
    asyncConnect(const boost::asio::ip::tcp::resolver::results_type &res, onMessageReceivedCB cb, holderType holder) {
        if (!SSL_set_tlsext_host_name(m_ws.next_layer().native_handle(), m_host.c_str())) {
            auto errorCode = boost::beast::error_code(
                    static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()
            );

            FTX_CB_ON_ERROR(cb, errorCode);

            return;
        }

        boost::asio::async_connect(
                m_ws.next_layer().next_layer(), res.begin(), res.end(),
                [this, cb = std::move(cb), holder = std::move(holder)]
                        (boost::system::error_code ec, const boost::asio::ip::tcp::resolver::iterator &) mutable {
                    if (ec) {
                        if (!m_stopRequested) { FTX_CB_ON_ERROR(cb, ec); }
                    } else {
                        onConnected(std::move(cb), std::move(holder));
                    }
                }
        );
    }

    void onConnected(onMessageReceivedCB cb, holderType holder) {
        m_ws.control_callback(
                [this]
                        (boost::beast::websocket::frame_type kind, boost::beast::string_view payload) mutable {
                    (void) kind;
                    (void) payload;
                }
        );

        m_ws.next_layer().async_handshake(
                boost::asio::ssl::stream_base::client, [this, cb = std::move(cb), holder = std::move(holder)]
                        (boost::system::error_code ec) mutable {
                    if (ec) {
                        if (!m_stopRequested) { FTX_CB_ON_ERROR(cb, ec); }
                    } else {
                        onAsyncSSLHandshake(std::move(cb), std::move(holder));
                    }
                }
        );
    }

    void onAsyncSSLHandshake(onMessageReceivedCB cb, holderType holder) {
        m_ws.async_handshake(
                m_host, "/ws/", [this, cb = std::move(cb), holder = std::move(holder)]
                        (boost::system::error_code ec) mutable { startReadWrite(ec, std::move(cb), std::move(holder)); }
        );
    }

    void stop() {
        m_stopRequested = true;

        if (m_ws.next_layer().next_layer().is_open()) {
            boost::system::error_code ec;
            m_ws.close(boost::beast::websocket::close_code::normal, ec);
        }
    }

    void startReadWrite(boost::system::error_code ec, const onMessageReceivedCB &cb, const holderType &holder) {
        startRead(ec, cb, holder);
        startWrite(ec, cb, holder);
    }

    void startRead(boost::system::error_code ec, onMessageReceivedCB cb, holderType holder) {
        if (ec) {
            if (!m_stopRequested) {
                FTX_CB_ON_ERROR(cb, ec);
            }

            stop();

            return;
        }

        m_ws.async_read(
                m_buf, [this, cb = std::move(cb), holder = std::move(holder)]
                        (boost::system::error_code ec, std::size_t rd) mutable {
                    onRead(ec, rd, std::move(cb), std::move(holder));
                }
        );
    }

    void startWrite(boost::system::error_code ec, onMessageReceivedCB cb, holderType holder) {

        if (ec) {
            if (!m_stopRequested) {
                FTX_CB_ON_ERROR(cb, ec);
            }

            stop();

            return;
        }

        m_ws.async_write(
                m_wBuffer.data(),
                [this, cb = std::move(cb), holder = std::move(holder)]
                        (boost::system::error_code ec, std::size_t rd) mutable {
                    onWrite(ec, rd, std::move(cb), std::move(holder));
                }
        );
    }

    void onRead(boost::system::error_code ec, std::size_t rd, onMessageReceivedCB cb, holderType holder) {
        if (ec) {
            if (!m_stopRequested) {
                FTX_CB_ON_ERROR(cb, ec);
            }

            stop();

            return;
        }

        auto size = m_buf.size();
        assert(size == rd);

        std::string strBuffer;
        strBuffer.reserve(size);

        for (const auto &it: m_buf.data()) {
            strBuffer.append(static_cast<const char *>(it.data()), it.size());
        }

        m_buf.consume(m_buf.size());

        bool ok = cb(nullptr, 0, std::string{}, strBuffer.data(), strBuffer.size());
        if (!ok) {
            stop();
        } else {
            startRead(boost::system::error_code{}, std::move(cb), std::move(holder));
        }
    }

    void onWrite(boost::system::error_code ec, std::size_t rd, onMessageReceivedCB cb, holderType holder) {
        boost::ignore_unused(rd);

        if (ec) {
            return;
        }

        // Clear the buffer
        m_wBuffer.consume(m_wBuffer.size());

        if (!m_requests.empty()) {
            auto msg = m_requests.back().dump();
            m_requests.pop_back();
            size_t n = buffer_copy(m_wBuffer.prepare(msg.size()), boost::asio::buffer(msg));
            m_wBuffer.commit(n);
            startWrite(boost::system::error_code{}, std::move(cb), std::move(holder));
        }
    }
};

WebSocket::WebSocket(boost::asio::io_context &ioContext) : m_p(spimpl::make_unique_impl<P>(ioContext)) {

}

void WebSocket::stop() {
    return m_p->stop();
}

void WebSocket::start(const std::string &host, const std::string &port, const std::vector<nlohmann::json> &requests,
                      WebSocket::onMessageReceivedCB cb, WebSocket::holderType holder) {
    return m_p->asyncStart(host, port, requests, std::move(cb), std::move(holder));
}

std::string WebSocket::streamName() const {
    return m_p->m_streamName;
}

void WebSocket::setStreamName(const std::string &streamName) {
    m_p->m_streamName = streamName;
}

void WebSocket::ping() {

    if (m_p->m_ws.is_open()) {
        m_p->m_ws.ping({});
    }
}
}