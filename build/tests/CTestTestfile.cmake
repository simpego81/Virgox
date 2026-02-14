# CMake generated Testfile for 
# Source directory: /home/ubuntu/trading-optimizer-cpp/tests
# Build directory: /home/ubuntu/trading-optimizer-cpp/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_data "/home/ubuntu/trading-optimizer-cpp/build/tests/test_data")
set_tests_properties(test_data PROPERTIES  _BACKTRACE_TRIPLES "/home/ubuntu/trading-optimizer-cpp/tests/CMakeLists.txt;5;add_test;/home/ubuntu/trading-optimizer-cpp/tests/CMakeLists.txt;0;")
add_test(test_strategy "/home/ubuntu/trading-optimizer-cpp/build/tests/test_strategy")
set_tests_properties(test_strategy PROPERTIES  _BACKTRACE_TRIPLES "/home/ubuntu/trading-optimizer-cpp/tests/CMakeLists.txt;10;add_test;/home/ubuntu/trading-optimizer-cpp/tests/CMakeLists.txt;0;")
add_test(test_backtesting "/home/ubuntu/trading-optimizer-cpp/build/tests/test_backtesting")
set_tests_properties(test_backtesting PROPERTIES  _BACKTRACE_TRIPLES "/home/ubuntu/trading-optimizer-cpp/tests/CMakeLists.txt;15;add_test;/home/ubuntu/trading-optimizer-cpp/tests/CMakeLists.txt;0;")
