// tick_handler.cpp
#include "tick_handler.hpp"

std::string extract_value(const std::string& msg, const std::string& key)
{
    std::string pattern = "\"" + key + "\":\"";
    size_t start = msg.find(pattern);
    if (start == std::string::npos) return "";
    start += pattern.size();
    size_t end = msg.find("\"", start);
    return msg.substr(start, end - start);
}

void tick_handler::update_tick(const std::string &tick_msg)
{
    tick t;
    t.symbol = extract_value(tick_msg, "s");
    t.spot_a = static_cast<int64_t>(std::stold(extract_value(tick_msg, "a")) * SCALE);
    t.spot_b = static_cast<int64_t>(std::stold(extract_value(tick_msg, "b")) * SCALE);

    std::lock_guard<std::mutex> lock(mutex_);
    latest_tick_ = t;
}

int64_t tick_handler::get_bid_price()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return latest_tick_.spot_b;
}

int64_t tick_handler::get_ask_price()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return latest_tick_.spot_a;
}