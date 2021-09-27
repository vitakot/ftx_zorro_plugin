/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_HTTP_SESSION_H
#define FTX_ZORRO_PLUGIN_HTTP_SESSION_H

#include <boost/asio/connect.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

class HTTPSession {

    net::io_context m_ioc;
    std::string m_uri;
    std::string m_apiKey;
    std::string m_apiSecret;
    std::string m_subAccountName;

    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    http::response<http::string_body> request(http::request<http::string_body> req);

    void authenticate(http::request<http::string_body> &req);

public:
    HTTPSession(const std::string &uri, const std::string &apiKey, const std::string &apiSecret, const std::string &subAccountName);

    http::response<http::string_body> methodGet(const std::string &target);

    http::response<http::string_body> methodPost(const std::string &target, const std::string &payload);

    http::response<http::string_body> methodDelete(const std::string &target);
};

#endif //FTX_ZORRO_PLUGIN_HTTP_SESSION_H