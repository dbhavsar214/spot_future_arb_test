#ifndef SPOTANDFUTUREARBITRAGE_WEBSOCKETCONNECTION_CPP
#define SPOTANDFUTUREARBITRAGE_WEBSOCKETCONNECTION_CPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <memory>
#include <string>
#include <functional>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    // Constructor: sets up networking objects
    explicit session(net::io_context& ioc);

    // Starts full connection flow (DNS → connect → stream)
    void run(const std::string& key, const std::string& secret);


private:

    // Called after DNS lookup completes
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);

    // Called after TCP connection is made
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);

    // Called after TLS handshake is done
    void on_tls_handshake(beast::error_code ec);

    // Called after WebSocket handshake is complete
    void on_handshake(beast::error_code ec);

    // Called after sending auth message
    void on_auth_write(beast::error_code ec, std::size_t bytes);

    // Called after sending subscribe message
    void on_sub_write(beast::error_code ec, std::size_t bytes);

    // Called whenever a message is received
    void on_read(beast::error_code ec, std::size_t bytes);

    // Prints error message
    void fail(beast::error_code ec, const char* what);

private:
    tcp::resolver resolver_;
    ssl::context ctx_;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
    beast::flat_buffer buffer_;

    std::string host_;
    std::string key_;
    std::string secret_;
};

#endif
