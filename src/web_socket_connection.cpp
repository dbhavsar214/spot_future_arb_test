#include "web_socket_connection.hpp"
#include <iostream>

session::session(net::io_context& ioc,
                 std::string host,
                 std::string stream,
                 tick_handler& market_handler)
    : resolver_(net::make_strand(ioc))
    , ctx_(ssl::context::tls_client)
    , ws_(net::make_strand(ioc), ctx_)
    , host_(std::move(host))
    , stream_(std::move(stream))
    ,market_handler_(market_handler)

{
    ctx_.set_default_verify_paths();
}

void session::run()
{
    resolver_.async_resolve(
        host_,
        "443",
        beast::bind_front_handler(
            &session::on_resolve,
            shared_from_this()
        )
    );
}

void session::on_resolve(beast::error_code ec,
                                 tcp::resolver::results_type results)
{
    if (ec) return fail(ec, "resolve");

    beast::get_lowest_layer(ws_).async_connect(
       results,
       beast::bind_front_handler(
           &session::on_connect,
           shared_from_this()
       )
   );
}

void session::on_connect(beast::error_code ec,
                                tcp::resolver::results_type::endpoint_type)
{
    if (ec) return fail(ec, "connect");

    ws_.next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(
            &session::on_tls_handshake,
            shared_from_this()
        )
    );
}

void session::on_tls_handshake(beast::error_code ec)
{
    if (ec) return fail(ec, "tls_handshake");

    // IMPORTANT: Binance requires Host header + stream path
    std::string target = "/ws/" + stream_;

    ws_.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::client
        )
    );

    ws_.set_option(websocket::stream_base::decorator(
        [this](websocket::request_type& req)
        {
            req.set(beast::http::field::host, host_);
            req.set(beast::http::field::user_agent, "binance-cpp-client");
        }
    ));

    ws_.async_handshake(
        host_,
        target,
        beast::bind_front_handler(
            &session::on_handshake,
            shared_from_this()
        )
    );
}

void session::on_handshake(beast::error_code ec)
{
    if (ec) return fail(ec, "handshake");

    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &session::on_read,
            shared_from_this()
        )
    );
}

void session::on_read(beast::error_code ec, std::size_t)
{
    if (ec) return fail(ec, "read");

    std::string msg =
        beast::buffers_to_string(buffer_.data());

    market_handler_.update_tick(msg);

    buffer_.consume(buffer_.size());

    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &session::on_read,
            shared_from_this()
        )
    );
}

void session::fail(beast::error_code ec, const char* what)
{
    std::cerr << what << ": " << ec.message() << std::endl;
}