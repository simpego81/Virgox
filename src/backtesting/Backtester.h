#ifndef BACKTESTER_H
#define BACKTESTER_H

#include <vector>
#include <string>

namespace trading {

// Forward declarations
class DataHandler;
class Strategy;
class TFModel;

struct Trade {
    size_t entry_idx;
    size_t exit_idx;
    double entry_price;
    double exit_price;
    double profit_loss;
    bool is_long;  // true for long, false for short
    
    Trade() : entry_idx(0), exit_idx(0), entry_price(0.0), exit_price(0.0), 
              profit_loss(0.0), is_long(true) {}
};

struct BacktestResult {
    double total_return;
    double sharpe_ratio;
    double max_drawdown;
    double win_rate;
    int total_trades;
    int winning_trades;
    int losing_trades;
    std::vector<Trade> trades;
    
    BacktestResult() : total_return(0.0), sharpe_ratio(0.0), max_drawdown(0.0),
                       win_rate(0.0), total_trades(0), winning_trades(0), losing_trades(0) {}
    
    // Fitness function for genetic algorithm (higher is better)
    double getFitness() const {
        // Combine multiple metrics with weights
        double fitness = total_return * 0.4 + sharpe_ratio * 0.3 - max_drawdown * 0.2 + win_rate * 0.1;
        return fitness;
    }
};

class Backtester {
public:
    Backtester();
    ~Backtester();
    
    // Run backtest on given data with a strategy
    BacktestResult runBacktest(const DataHandler& data, const Strategy& strategy, 
                               TFModel* tf_model = nullptr, double initial_capital = 10000.0);
    
    // Print backtest results
    static void printResults(const BacktestResult& result);
    
private:
    // Calculate Sharpe ratio from returns
    double calculateSharpeRatio(const std::vector<double>& returns) const;
    
    // Calculate maximum drawdown
    double calculateMaxDrawdown(const std::vector<double>& equity_curve) const;
};

} // namespace trading

#endif // BACKTESTER_H
