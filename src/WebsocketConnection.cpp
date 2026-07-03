
#include "WebsocketConnection.hpp"
#include <iostream>

// Constructor
// Sets TLS context + WebSocket stream
session::session(net::io_context& ioc) :
    resolver_(net::make_strand(ioc)), ctx_(ssl::context::tlsv12_client), ws_(net::make_strand(ioc), ctx_),
    host_("stream.data.alpaca.markets")
{
    ctx_.set_default_verify_paths();
}

// Print error
void session::fail(beast::error_code ec, const char* what) { std::cerr << what << ": " << ec.message() << "\n"; }

// Start connection flow
// Step 1: start DNS lookup
void session::run(const std::string& key, const std::string& secret)
{
    key_ = key;
    secret_ = secret;

    resolver_.async_resolve(host_, "443", beast::bind_front_handler(&session::on_resolve, shared_from_this()));
}

// DNS resolved → connect TCP
void session::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec)
        return fail(ec, "resolve");

    beast::get_lowest_layer(ws_).async_connect(results,
                                               beast::bind_front_handler(&session::on_connect, shared_from_this()));
}

// TCP connected → start TLS handshake
void session::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
{
    if (ec)
        return fail(ec, "connect");

    beast::get_lowest_layer(ws_).expires_never();

    SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str());

    ws_.next_layer().async_handshake(ssl::stream_base::client,
                                     beast::bind_front_handler(&session::on_tls_handshake, shared_from_this()));
}

// TLS done → start WebSocket handshake
void session::on_tls_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "tls_handshake");

    ws_.async_handshake(host_, "/v2/iex", beast::bind_front_handler(&session::on_handshake, shared_from_this()));
}

// WebSocket connected → send auth
void session::on_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "ws_handshake");

    std::cout << "Connected\n";

    std::string auth = R"({"action":"auth","key":")" + key_ + R"(","secret":")" + secret_ + R"("})";

    ws_.async_write(net::buffer(auth), beast::bind_front_handler(&session::on_auth_write, shared_from_this()));
}

// After auth → send subscribe
void session::on_auth_write(beast::error_code ec, std::size_t)
{
    if (ec)
        return fail(ec, "auth_write");

    std::string sub = R"({"action":"subscribe","trades":["AAPL"]})";

    ws_.async_write(net::buffer(sub), beast::bind_front_handler(&session::on_sub_write, shared_from_this()));
}

// After subscribe → start reading stream
void session::on_sub_write(beast::error_code ec, std::size_t)
{
    if (ec)
        return fail(ec, "sub_write");

    ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
}

// Continuous market data stream
void session::on_read(beast::error_code ec, std::size_t)
{
    if (ec)
        return fail(ec, "read");

    const std::string msg = beast::buffers_to_string(buffer_.data());
    std::cout << msg ;

    buffer_.consume(buffer_.size());

    // keep reading
    ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
}

