if(BUILD_TESTS)
    # Test for DataHandler
    add_executable(test_data test_data.cpp)
    target_link_libraries(test_data data_handler gtest_main)
    add_test(NAME test_data COMMAND test_data)

    # Test for Strategy
    add_executable(test_strategy test_strategy.cpp)
    target_link_libraries(test_strategy strategy data_handler gtest_main)
    add_test(NAME test_strategy COMMAND test_strategy)

    # Test for Backtester
    add_executable(test_backtesting test_backtesting.cpp)
    target_link_libraries(test_backtesting backtester strategy data_handler tf_model gtest_main)
    add_test(NAME test_backtesting COMMAND test_backtesting)
endif()
