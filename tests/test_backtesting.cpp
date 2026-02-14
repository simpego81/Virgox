'''#include "gtest/gtest.h"
#include "backtesting/Backtester.h"
#include "data/DataHandler.h"
#include "strategy/Strategy.h"
#include <fstream>

// Test fixture for Backtester tests
class BacktesterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a dummy CSV file with a clear trend
        std::ofstream test_csv("backtest_data.csv");
        test_csv << "timestamp,open,high,low,close,volume\n";
        for (int i = 0; i < 100; ++i) {
            double price = 100.0 + i * 0.5; // Upward trend
            test_csv << 1640995200 + i * 86400 << "," << price << "," << price + 2 << "," << price - 2 << "," << price << ",1000\n";
        }
        test_csv.close();
        
        data_handler_.loadFromCSV("backtest_data.csv");
    }

    void TearDown() override {
        std::remove("backtest_data.csv");
    }

    trading::DataHandler data_handler_;
    trading::Backtester backtester_;
};

// Test case for a simple backtest run
TEST_F(BacktesterTest, RunSimpleBacktest) {
    // Use a simple strategy that always buys
    class AlwaysBuyStrategy : public trading::Strategy {
    public:
        trading::Signal generateSignal(const std::vector<trading::Candle>&, size_t, double = 0.0) const {
            return trading::Signal::BUY;
        }
    };

    AlwaysBuyStrategy strategy;
    trading::BacktestResult result = backtester_.runBacktest(data_handler_, strategy);

    // Expect one long trade that lasts the entire period
    EXPECT_EQ(result.total_trades, 1);
    EXPECT_TRUE(result.total_return > 0.0);
    EXPECT_EQ(result.winning_trades, 1);
    EXPECT_EQ(result.losing_trades, 0);
    EXPECT_DOUBLE_EQ(result.win_rate, 1.0);
}

// Test case for checking profit calculation
TEST_F(BacktesterTest, ProfitCalculation) {
    // Strategy: Buy on day 20, sell on day 80
    class BuyAndHoldStrategy : public trading::Strategy {
    public:
        trading::Signal generateSignal(const std::vector<trading::Candle>&, size_t current_idx, double = 0.0) const {
            if (current_idx == 20) return trading::Signal::BUY;
            if (current_idx == 80) return trading::Signal::SELL;
            return trading::Signal::HOLD;
        }
    };

    BuyAndHoldStrategy strategy;
    trading::BacktestResult result = backtester_.runBacktest(data_handler_, strategy, nullptr, 10000.0);

    ASSERT_EQ(result.total_trades, 1);
    const auto& trade = result.trades[0];

    double entry_price = 100.0 + 20 * 0.5;
    double exit_price = 100.0 + 80 * 0.5;
    double shares = 10000.0 / entry_price;
    double expected_profit = (exit_price - entry_price) * shares;

    EXPECT_DOUBLE_EQ(trade.entry_price, entry_price);
    EXPECT_DOUBLE_EQ(trade.exit_price, exit_price);
    EXPECT_NEAR(trade.profit_loss, expected_profit, 0.01);
    EXPECT_TRUE(result.total_return > 0.0);
}

// Test case for stop-loss functionality
TEST_F(BacktesterTest, StopLoss) {
    // Create data with a sharp drop
    std::ofstream sl_csv("sl_data.csv");
    sl_csv << "timestamp,open,high,low,close,volume\n";
    for (int i = 0; i < 20; ++i) sl_csv << i << "," << 100+i << "," << 100+i << "," << 100+i << "," << 100+i << ",1000\n";
    for (int i = 20; i < 40; ++i) sl_csv << i << "," << 120-(i-20) << "," << 120-(i-20) << "," << 120-(i-20) << "," << 120-(i-20) << ",1000\n";
    sl_csv.close();
    
    trading::DataHandler sl_data;
    sl_data.loadFromCSV("sl_data.csv");

    class BuyOnceStrategy : public trading::Strategy {
    public:
        trading::Signal generateSignal(const std::vector<trading::Candle>&, size_t current_idx, double = 0.0) const {
            if (current_idx == 15) return trading::Signal::BUY;
            return trading::Signal::HOLD;
        }
    };

    trading::StrategyParams params;
    params.stop_loss_pct = 0.05; // 5% stop loss
    params.take_profit_pct = 0.50; // High take profit to avoid triggering
    
    BuyOnceStrategy strategy_raw;
    strategy_raw.setParams(params);

    trading::BacktestResult result = backtester_.runBacktest(sl_data, strategy_raw);

    ASSERT_EQ(result.total_trades, 1);
    const auto& trade = result.trades[0];

    double entry_price = 115.0;
    double stop_price = entry_price * (1.0 - 0.05);
    
    // The exit should be around the stop price
    EXPECT_LT(trade.exit_price, entry_price);
    EXPECT_NEAR(trade.exit_price, stop_price, 2.0); // Allow some tolerance for daily price moves
    EXPECT_TRUE(trade.profit_loss < 0);

    std::remove("sl_data.csv");
}
'''
