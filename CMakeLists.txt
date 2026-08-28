cmake_minimum_required(VERSION 4.2)
project(SpotandFuturesArbitrage)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost CONFIG REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

add_executable(SpotandFuturesArbitrage main.cpp
        src/web_socket_connection.cpp
        src/tick_handler.cpp
        src/server.cpp
        src/arbitrage_checker.cpp
        src/db_connection.cpp
)

target_include_directories(SpotandFuturesArbitrage PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(SpotandFuturesArbitrage PRIVATE
        Boost::boost
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        RapidJSON rapidjson
        libpqxx::pqxx

)