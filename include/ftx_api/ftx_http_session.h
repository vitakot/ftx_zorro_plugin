/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_HTTP_SESSION_H
#define FTX_HTTP_SESSION_H

#include <boost/asio/connect.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <spimpl.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

namespace ftx {
class HTTPSession {

    struct P;
    spimpl::unique_impl_ptr<P> m_p{};

public:
    HTTPSession(const std::string &uri, const std::string &apiKey, const std::string &apiSecret,
                const std::string &subAccountName);

    http::response<http::string_body> methodGet(const std::string &target);

    http::response<http::string_body> methodPost(const std::string &target, const std::string &payload);

    http::response<http::string_body> methodDelete(const std::string &target);
};
}
#endif //FTX_HTTP_SESSION_H