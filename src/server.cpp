//
// Created by Divya on 2026-08-22.
//

#include "server.hpp"
#include <string>

server::server(tick_handler& market_handler, const std::string& host, const std::string& stream) :
    market_handler_(market_handler),
    host_(host),
    stream_(stream)

{
    init();
}

void server::init()
{
    market_session = std::make_shared<session>(
        ioc,
        host_,
        stream_,
        market_handler_
        );
}

void server::run()
{
    market_session->run();

    ioc.run();
}
