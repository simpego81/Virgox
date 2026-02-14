#include "gtest/gtest.h"
#include "strategy/Strategy.h"
#include "data/DataHandler.h"
#include <vector>

// Test fixture for Strategy tests
class StrategyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a sample set of candles for testing
        // Prices: 100, 102, 104, 106, 108, 110, 108, 106, 104, 102
        candles_.push_back(trading::Candle(0, 98, 101, 97, 100, 1000));
        candles_.push_back(trading::Candle(1, 100, 103, 99, 102, 1100));
        candles_.push_back(trading::Candle(2, 102, 105, 101, 104, 1200));
        candles_.push_back(trading::Candle(3, 104, 107, 103, 106, 1300));
        candles_.push_back(trading::Candle(4, 106, 109, 105, 108, 1400));
        candles_.push_back(trading::Candle(5, 108, 111, 107, 110, 1500));
        candles_.push_back(trading::Candle(6, 110, 110, 107, 108, 1600));
        candles_.push_back(trading::Candle(7, 108, 108, 105, 106, 1700));
        candles_.push_back(trading::Candle(8, 106, 106, 103, 104, 1800));
        candles_.push_back(trading::Candle(9, 104, 104, 101, 102, 1900));
    }

    std::vector<trading::Candle> candles_;
    trading::Strategy strategy_;
};

// Test case for calculating Simple Moving Average (SMA)
TEST_F(StrategyTest, CalculateSMA) {
    // SMA for last 5 periods at index 9: (110+108+106+104+102)/5 = 106
    double sma5 = trading::Strategy::calculateSMA(candles_, 9, 5);
    EXPECT_DOUBLE_EQ(sma5, 106.0);

    // SMA for last 3 periods at index 5: (108+110+108)/3 = 108.666...
    double sma3 = trading::Strategy::calculateSMA(candles_, 6, 3);
    EXPECT_NEAR(sma3, 108.6666, 0.001);
}

// Test case for calculating Relative Strength Index (RSI)
// Note: This is a simplified RSI calculation for testing purposes.
// Real RSI calculations use smoothed moving averages.
TEST_F(StrategyTest, CalculateRSI) {
    // Create a clear trend for RSI testing
    std::vector<trading::Candle> rsi_candles;
    for (int i = 0; i < 15; ++i) {
        rsi_candles.push_back(trading::Candle(i, 100+i, 100+i, 100+i, 100+i, 1000)); // Upward trend
    }
    // RSI should be high (overbought)
    double rsi_overbought = trading::Strategy::calculateRSI(rsi_candles, 14, 14);
    EXPECT_GT(rsi_overbought, 99.0); // Should be 100

    rsi_candles.clear();
    for (int i = 0; i < 15; ++i) {
        rsi_candles.push_back(trading::Candle(i, 100-i, 100-i, 100-i, 100-i, 1000)); // Downward trend
    }
    // RSI should be low (oversold)
    double rsi_oversold = trading::Strategy::calculateRSI(rsi_candles, 14, 14);
    EXPECT_LT(rsi_oversold, 1.0); // Should be 0
}

// Test case for generating a BUY signal
TEST_F(StrategyTest, GenerateBuySignal) {
    trading::StrategyParams params;
    params.short_ma_period = 3;
    params.long_ma_period = 7;
    params.rsi_period = 5;
    params.rsi_oversold = 30.0;
    params.rsi_overbought = 70.0;
    strategy_.setParams(params);

    // Create a scenario for a BUY signal
    // Short MA crosses above Long MA
    std::vector<trading::Candle> buy_candles;
    for(int i=0; i<10; ++i) buy_candles.push_back(trading::Candle(i, 100, 100, 100, 100, 1000));
    for(int i=10; i<20; ++i) buy_candles.push_back(trading::Candle(i, 100+i, 100+i, 100+i, 100+i, 1000));

    trading::Signal signal = strategy_.generateSignal(buy_candles, 19);
    EXPECT_EQ(signal, trading::Signal::BUY);
}

// Test case for generating a SELL signal
TEST_F(StrategyTest, GenerateSellSignal) {
    trading::StrategyParams params;
    params.short_ma_period = 3;
    params.long_ma_period = 7;
    params.rsi_period = 14;
    params.rsi_overbought = 70.0;
    strategy_.setParams(params);

    // Create a scenario for a SELL signal (overbought RSI)
    std::vector<trading::Candle> sell_candles;
    for (int i = 0; i < 20; ++i) {
        sell_candles.push_back(trading::Candle(i, 100 + i*2, 100 + i*2, 100 + i*2, 100 + i*2, 1000));
    }

    trading::Signal signal = strategy_.generateSignal(sell_candles, 19);
    EXPECT_EQ(signal, trading::Signal::SELL);
}
