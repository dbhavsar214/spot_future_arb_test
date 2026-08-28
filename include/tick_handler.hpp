// tick_handler.hpp
#ifndef SPOTANDFUTURESARBITRAGE_TICK_HANDLER_HPP
#define SPOTANDFUTURESARBITRAGE_TICK_HANDLER_HPP
#include <string>
#include <mutex>

struct tick
{
    std::string symbol;
    int64_t spot_b;
    int64_t spot_a;
};

class tick_handler
{
public:
    static constexpr int64_t SCALE = 100000000;

    void update_tick(const std::string &tick_message);
    int64_t get_bid_price();
    int64_t get_ask_price();

private:
    tick latest_tick_;
    std::mutex mutex_;   // race-condition fix, discussed earlier
};
#endif