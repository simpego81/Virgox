add_library(tf_model
    TFModel.cpp
    TFModel.h
)

target_include_directories(tf_model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
