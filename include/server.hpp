#ifndef SPOTANDFUTURESARBITRAGE_SERVER_HPP
#define SPOTANDFUTURESARBITRAGE_SERVER_HPP

#include <boost/asio.hpp>


#include "web_socket_connection.hpp"
#include "tick_handler.hpp"

class server
{
public:
    server(tick_handler& market_handler, const std::string& host, const std::string& stream);
    void run();

private:
    tick_handler& market_handler_;
    boost::asio::io_context ioc;

    std::shared_ptr<session> market_session;

    std::string host_;
    std::string stream_;

    void init();

};

#endif
