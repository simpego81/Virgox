add_library(data_handler
    DataHandler.cpp
    DataHandler.h
)

target_include_directories(data_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
