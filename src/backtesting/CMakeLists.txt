add_library(backtester
    Backtester.cpp
    Backtester.h
)

target_include_directories(backtester PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(backtester data_handler strategy)
