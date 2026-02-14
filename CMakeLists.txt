cmake_minimum_required(VERSION 3.15)
project(TradingOptimizerCpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build unit tests" ON)

# Find packages
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Add subdirectories
add_subdirectory(src/data)
add_subdirectory(src/strategy)
add_subdirectory(src/backtesting)
add_subdirectory(src/ga)
add_subdirectory(src/nn)

# Main executable
add_executable(trading_optimizer src/main.cpp)
target_link_libraries(trading_optimizer
    data_handler
    strategy
    backtester
    genetic_optimizer
    tf_model
    Threads::Threads
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    # Add Google Test directly from the source directory
    add_subdirectory(third_party/googletest)
    add_subdirectory(tests)
endif()
