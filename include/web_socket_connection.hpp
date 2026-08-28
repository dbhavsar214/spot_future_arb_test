//
// Created by Divya on 2026-08-15.
//

#ifndef SPOTANDFUTURESARBITRAGE_WEB_SOCKET_CONNECTION_HPP
#define SPOTANDFUTURESARBITRAGE_WEB_SOCKET_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <memory>
#include <string>

#include "tick_handler.hpp"


namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    explicit session(net::io_context& ioc,
                            std::string host,
                            std::string stream,
                            tick_handler& market_handler
                            );


    void run();

private:
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec,
                     tcp::resolver::results_type::endpoint_type ep);
    void on_tls_handshake(beast::error_code ec);
    void on_handshake(beast::error_code ec);

    void on_read(beast::error_code ec, std::size_t bytes);

    void fail(beast::error_code ec, const char* what);

private:
    tcp::resolver resolver_;
    ssl::context ctx_;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
    beast::flat_buffer buffer_;

    std::string host_;
    std::string stream_;
    tick_handler& market_handler_;
};

#endif //SPOTANDFUTURESARBITRAGE_WEB_SOCKET_CONNECTION_HPP
