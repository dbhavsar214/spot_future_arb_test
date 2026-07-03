#include <iostream>

#include <boost/asio.hpp>
#include "WebsocketConnection.hpp"

namespace  net = boost::asio;
int main()
{
    std::string key = "PK6W75W7TV6NJMOEBQV5SZSUJU";
    std::string secret = "HEhLqgs2fMeLTexgqXxkhVajupNFu9Fv7gF9ofX5tgdG";

    net::io_context ioc;

    auto s = std::make_shared<session>(ioc);
    s->run(key, secret);
    
    ioc.run();

}
