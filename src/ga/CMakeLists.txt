add_library(genetic_optimizer
    GeneticOptimizer.cpp
    GeneticOptimizer.h
)

target_include_directories(genetic_optimizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(genetic_optimizer strategy backtester data_handler tf_model)
