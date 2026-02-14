#include "gtest/gtest.h"
#include "data/DataHandler.h"
#include <fstream>

// Test fixture for DataHandler tests
class DataHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a dummy CSV file for testing
        std::ofstream test_csv("test_data.csv");
        test_csv << "timestamp,open,high,low,close,volume\n";
        test_csv << "1640995200,100,105,99,102,1000\n";
        test_csv << "1641081600,102,108,101,107,1200\n";
        test_csv << "1641168000,107,110,105,109,1500\n";
        test_csv.close();
    }

    void TearDown() override {
        // Clean up the dummy CSV file
        std::remove("test_data.csv");
    }

    trading::DataHandler data_handler_;
};

// Test case for loading a valid CSV file
TEST_F(DataHandlerTest, LoadValidCSV) {
    ASSERT_TRUE(data_handler_.loadFromCSV("test_data.csv"));
    ASSERT_EQ(data_handler_.size(), 3);
}

// Test case for loading a non-existent CSV file
TEST_F(DataHandlerTest, LoadNonExistentCSV) {
    ASSERT_FALSE(data_handler_.loadFromCSV("non_existent_file.csv"));
}

// Test case for checking the content of loaded data
TEST_F(DataHandlerTest, CheckLoadedData) {
    data_handler_.loadFromCSV("test_data.csv");
    const auto& candles = data_handler_.getCandles();
    
    ASSERT_EQ(candles.size(), 3);
    
    // Check first candle
    EXPECT_EQ(candles[0].timestamp, 1640995200);
    EXPECT_DOUBLE_EQ(candles[0].open, 100.0);
    EXPECT_DOUBLE_EQ(candles[0].high, 105.0);
    EXPECT_DOUBLE_EQ(candles[0].low, 99.0);
    EXPECT_DOUBLE_EQ(candles[0].close, 102.0);
    EXPECT_EQ(candles[0].volume, 1000);
    
    // Check third candle
    EXPECT_EQ(candles[2].timestamp, 1641168000);
    EXPECT_DOUBLE_EQ(candles[2].close, 109.0);
}

// Test case for getting candles in a range
TEST_F(DataHandlerTest, GetCandlesInRange) {
    data_handler_.loadFromCSV("test_data.csv");
    auto range = data_handler_.getCandlesInRange(1, 3);
    
    ASSERT_EQ(range.size(), 2);
    EXPECT_DOUBLE_EQ(range[0].close, 107.0);
    EXPECT_DOUBLE_EQ(range[1].close, 109.0);
}
