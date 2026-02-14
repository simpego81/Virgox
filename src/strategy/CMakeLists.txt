add_library(strategy
    Strategy.cpp
    Strategy.h
)

target_include_directories(strategy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(strategy data_handler)
