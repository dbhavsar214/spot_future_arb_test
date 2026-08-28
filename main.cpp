//
// Created by Divya on 2026-07-04.
//

#include "server.hpp"
#include "tick_handler.hpp"
#include "arbitrage_checker.hpp"
#include "db_connection.hpp"

#include <thread>
#include <chrono>

int main()
{
    tick_handler spot_tick_handler;
    tick_handler future_tick_handler;

    server spot_server(spot_tick_handler, "stream.binance.com", "btcusdt@bookTicker");
    server future_server(future_tick_handler, "fstream.binance.com", "btcusdt@bookTicker");

    db_connection db("localhost", "spot_fut_arb_test", "postgresql", "Bhavsar@8780");
    std::string table_name = "btc_trades";
    db.create_crypto_table(table_name);

    std::thread t1([&] { spot_server.run(); });
    std::thread t2([&] { future_server.run(); });

    t1.detach();
    t2.detach();


    int64_t qty = static_cast<int64_t>(0.025 * tick_handler::SCALE);


    arbitrage_checker checker(spot_tick_handler, future_tick_handler, qty, table_name, db);

    while (true)
    {
        checker.check_for_arbitrage();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}