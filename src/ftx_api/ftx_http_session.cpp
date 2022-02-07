/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ftx_api/ftx_http_session.h>
#include <ftx_api/utils.h>
#include <openssl/hmac.h>
#include <boost/asio/ssl.hpp>
#include <boost/beast/version.hpp>

namespace ftx {

namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;

struct HTTPSession::P {

    net::io_context m_ioc;
    std::string m_uri;
    std::string m_apiKey;
    std::string m_apiSecret;
    std::string m_subAccountName;
    const EVP_MD *m_evp_md;

    P() : m_evp_md(EVP_sha256()) {

    }

    http::response<http::string_body> request(http::request<http::string_body> req);

    void authenticate(http::request<http::string_body> &req) const;
};

HTTPSession::HTTPSession(const std::string &uri, const std::string &apiKey, const std::string &apiSecret,
                         const std::string &subAccountName) : m_p(spimpl::make_unique_impl<P>()) {
    m_p->m_uri = uri;
    m_p->m_apiKey = apiKey;
    m_p->m_apiSecret = apiSecret;
    m_p->m_subAccountName = subAccountName;
}

http::response<http::string_body> HTTPSession::methodGet(const std::string &target) {
    std::string endpoint = "/api/" + target;
    http::request<http::string_body> req{http::verb::get, endpoint, 11};
    return m_p->request(req);
}

http::response<http::string_body> HTTPSession::methodPost(const std::string &target, const std::string &payload) {
    std::string endpoint = "/api/" + target;
    http::request<http::string_body> req{http::verb::post, endpoint, 11};
    req.body() = payload;
    req.prepare_payload();
    return m_p->request(req);
}

http::response<http::string_body> HTTPSession::methodDelete(const std::string &target) {
    std::string endpoint = "/api/" + target;
    http::request<http::string_body> req{http::verb::delete_, endpoint, 11};
    return m_p->request(req);
}

http::response<http::string_body> HTTPSession::P::request(
        http::request<http::string_body> req) {
    req.set(http::field::host, m_uri.c_str());
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    ssl::context ctx{ssl::context::sslv23_client};
    ctx.set_default_verify_paths();

    tcp::resolver resolver{m_ioc};
    ssl::stream<tcp::socket> stream{m_ioc, ctx};

    /// Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(stream.native_handle(), m_uri.c_str())) {
        boost::system::error_code ec{static_cast<int>(::ERR_get_error()),
                                     net::error::get_ssl_category()};
        throw boost::system::system_error{ec};
    }

    auto const results = resolver.resolve(m_uri.c_str(), "443");
    net::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(ssl::stream_base::client);

    authenticate(req);

    if (req.method() == http::verb::post) {
        req.set(http::field::content_type, "application/json");
    }

    http::write(stream, req);
    boost::beast::flat_buffer buffer;
    http::response<http::string_body> response;
    http::read(stream, buffer, response);

    boost::system::error_code ec;
    stream.shutdown(ec);
    if (ec == boost::asio::error::eof) {
        /// Rationale:
        /// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
        ec.assign(0, ec.category());
    }

    return response;
}

void HTTPSession::P::authenticate(http::request<http::string_body> &req) const {

    std::string method(req.method_string());
    std::string path(req.target());
    std::string body(req.body());

    auto ts = getMsTimestamp(currentTime()).count();
    std::string data = std::to_string(ts) + method + path;

    if (!body.empty()) {
        data += body;
    }

    unsigned char digest[SHA256_DIGEST_LENGTH];
    unsigned int digestLength = 32;

    HMAC(m_evp_md, m_apiSecret.data(), m_apiSecret.size(), reinterpret_cast<const unsigned char *>(data.data()),
         data.length(), digest, &digestLength);

    std::string sign = stringToHex(digest, sizeof(digest));

    req.set("FTX-KEY", m_apiKey);
    req.set("FTX-TS", std::to_string(ts));
    req.set("FTX-SIGN", sign);

    if (!m_subAccountName.empty()) {
        req.set("FTX-SUBACCOUNT", m_subAccountName);
    }
}
}
