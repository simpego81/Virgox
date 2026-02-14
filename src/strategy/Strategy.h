#ifndef STRATEGY_H
#define STRATEGY_H

#include <vector>
#include <string>

namespace trading {

// Forward declaration
struct Candle;

enum class Signal {
    BUY,
    SELL,
    HOLD
};

// Strategy parameters that will be optimized by GA
struct StrategyParams {
    // Moving average periods
    int short_ma_period;
    int long_ma_period;
    
    // RSI parameters
    int rsi_period;
    double rsi_oversold;
    double rsi_overbought;
    
    // Stop loss and take profit
    double stop_loss_pct;
    double take_profit_pct;
    
    // Neural network weight (0.0 to 1.0)
    double nn_weight;
    
    StrategyParams()
        : short_ma_period(10), long_ma_period(50),
          rsi_period(14), rsi_oversold(30.0), rsi_overbought(70.0),
          stop_loss_pct(0.02), take_profit_pct(0.05),
          nn_weight(0.5) {}
};

class Strategy {
public:
    Strategy();
    explicit Strategy(const StrategyParams& params);
    ~Strategy();
    
    // Set strategy parameters
    void setParams(const StrategyParams& params);
    const StrategyParams& getParams() const;
    
    // Generate trading signal based on current market data
    virtual Signal generateSignal(const std::vector<Candle>& candles, size_t current_idx, double nn_prediction = 0.0) const;
    
    // Calculate Simple Moving Average
    static double calculateSMA(const std::vector<Candle>& candles, size_t end_idx, int period);
    
    // Calculate RSI
    static double calculateRSI(const std::vector<Candle>& candles, size_t end_idx, int period);
    
    // Get strategy description
    std::string getDescription() const;
    
private:
    StrategyParams params_;
};

} // namespace trading

#endif // STRATEGY_H
