//
// Created by Divya on 2026-07-04.
//

#include "include/web_socket_connection.hpp"
#include <boost/asio.hpp>

int main()
{

    net::io_context ioc;

    auto binance_spot_session = std::make_shared<session>(
     ioc,
     "stream.binance.com",
     "btcusdt@bookTicker"
 );

    binance_spot_session->run();
    ioc.run();
    return 0;
}