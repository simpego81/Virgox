#include "Strategy.h"
#include "data/DataHandler.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>

namespace trading {

Strategy::Strategy() {}

Strategy::Strategy(const StrategyParams& params) : params_(params) {}

Strategy::~Strategy() {}

void Strategy::setParams(const StrategyParams& params) {
    params_ = params;
}

const StrategyParams& Strategy::getParams() const {
    return params_;
}

Signal Strategy::generateSignal(const std::vector<Candle>& candles, size_t current_idx, double nn_prediction) const {
    if (current_idx < static_cast<size_t>(params_.long_ma_period) || current_idx >= candles.size()) {
        return Signal::HOLD;
    }
    
    // Calculate technical indicators
    double short_ma = calculateSMA(candles, current_idx, params_.short_ma_period);
    double long_ma = calculateSMA(candles, current_idx, params_.long_ma_period);
    double rsi = calculateRSI(candles, current_idx, params_.rsi_period);
    
    // Combine technical analysis with neural network prediction
    double technical_score = 0.0;
    
    // Moving average crossover
    if (short_ma > long_ma) {
        technical_score += 1.0;
    } else {
        technical_score -= 1.0;
    }
    
    // RSI
    if (rsi < params_.rsi_oversold) {
        technical_score += 1.0;  // Oversold, potential buy
    } else if (rsi > params_.rsi_overbought) {
        technical_score -= 1.0;  // Overbought, potential sell
    }
    
    // Combine with neural network prediction (if available)
    double final_score = (1.0 - params_.nn_weight) * technical_score + params_.nn_weight * nn_prediction * 2.0;
    
    // Generate signal based on final score
    if (final_score > 0.5) {
        return Signal::BUY;
    } else if (final_score < -0.5) {
        return Signal::SELL;
    } else {
        return Signal::HOLD;
    }
}

double Strategy::calculateSMA(const std::vector<Candle>& candles, size_t end_idx, int period) {
    if (end_idx < static_cast<size_t>(period - 1) || candles.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (int i = 0; i < period; ++i) {
        sum += candles[end_idx - i].close;
    }
    
    return sum / period;
}

double Strategy::calculateRSI(const std::vector<Candle>& candles, size_t end_idx, int period) {
    if (end_idx < static_cast<size_t>(period) || candles.empty()) {
        return 50.0;  // Neutral RSI
    }
    
    double avg_gain = 0.0;
    double avg_loss = 0.0;
    
    // Calculate initial average gain and loss
    for (int i = 1; i <= period; ++i) {
        double change = candles[end_idx - period + i].close - candles[end_idx - period + i - 1].close;
        if (change > 0) {
            avg_gain += change;
        } else {
            avg_loss += std::abs(change);
        }
    }
    
    avg_gain /= period;
    avg_loss /= period;
    
    if (avg_loss == 0.0) {
        return 100.0;
    }
    
    double rs = avg_gain / avg_loss;
    double rsi = 100.0 - (100.0 / (1.0 + rs));
    
    return rsi;
}

std::string Strategy::getDescription() const {
    std::ostringstream oss;
    oss << "Strategy[SMA:" << params_.short_ma_period << "/" << params_.long_ma_period
        << ", RSI:" << params_.rsi_period << "(" << params_.rsi_oversold << "," << params_.rsi_overbought << ")"
        << ", SL:" << params_.stop_loss_pct << ", TP:" << params_.take_profit_pct
        << ", NN:" << params_.nn_weight << "]";
    return oss.str();
}

} // namespace trading
