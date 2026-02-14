#include "Backtester.h"
#include "data/DataHandler.h"
#include "strategy/Strategy.h"
#include "nn/TFModel.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace trading {

Backtester::Backtester() {}

Backtester::~Backtester() {}

BacktestResult Backtester::runBacktest(const DataHandler& data, const Strategy& strategy, 
                                       TFModel* tf_model, double initial_capital) {
    BacktestResult result;
    
    const auto& candles = data.getCandles();
    if (candles.size() < 100) {
        std::cerr << "Not enough data for backtesting" << std::endl;
        return result;
    }
    
    double capital = initial_capital;
    double position = 0.0;  // Number of shares held
    double entry_price = 0.0;
    size_t entry_idx = 0;
    bool in_position = false;
    
    std::vector<double> equity_curve;
    std::vector<double> returns;
    
    const auto& params = strategy.getParams();
    
    // Start from a point where we have enough history for indicators
    size_t start_idx = std::max(params.long_ma_period, params.rsi_period) + 10;
    
    for (size_t i = start_idx; i < candles.size(); ++i) {
        double current_price = candles[i].close;
        
        // Get neural network prediction if available
        double nn_prediction = 0.0;
        if (tf_model != nullptr) {
            std::vector<double> features;
            // Use last 20 closing prices as features
            for (size_t j = std::max(i, size_t(20)) - 20; j < i; ++j) {
                features.push_back(candles[j].close);
            }
            nn_prediction = tf_model->predict(features);
        }
        
        // Generate signal
        Signal signal = strategy.generateSignal(candles, i, nn_prediction);
        
        // Execute trades based on signal
        if (!in_position && signal == Signal::BUY) {
            // Enter long position
            position = capital / current_price;
            entry_price = current_price;
            entry_idx = i;
            in_position = true;
        } else if (in_position) {
            // Check stop loss and take profit
            double price_change_pct = (current_price - entry_price) / entry_price;
            
            bool should_exit = false;
            if (signal == Signal::SELL) {
                should_exit = true;
            } else if (price_change_pct <= -params.stop_loss_pct) {
                should_exit = true;  // Stop loss triggered
            } else if (price_change_pct >= params.take_profit_pct) {
                should_exit = true;  // Take profit triggered
            }
            
            if (should_exit) {
                // Exit position
                capital = position * current_price;
                double profit_loss = capital - (position * entry_price);
                
                Trade trade;
                trade.entry_idx = entry_idx;
                trade.exit_idx = i;
                trade.entry_price = entry_price;
                trade.exit_price = current_price;
                trade.profit_loss = profit_loss;
                trade.is_long = true;
                
                result.trades.push_back(trade);
                
                if (profit_loss > 0) {
                    result.winning_trades++;
                } else {
                    result.losing_trades++;
                }
                
                position = 0.0;
                in_position = false;
            }
        }
        
        // Calculate current equity
        double current_equity = in_position ? (position * current_price) : capital;
        equity_curve.push_back(current_equity);
        
        if (equity_curve.size() > 1) {
            double ret = (current_equity - equity_curve[equity_curve.size() - 2]) / equity_curve[equity_curve.size() - 2];
            returns.push_back(ret);
        }
    }
    
    // Close any open position at the end
    if (in_position) {
        double final_price = candles.back().close;
        capital = position * final_price;
        
        Trade trade;
        trade.entry_idx = entry_idx;
        trade.exit_idx = candles.size() - 1;
        trade.entry_price = entry_price;
        trade.exit_price = final_price;
        trade.profit_loss = capital - (position * entry_price);
        trade.is_long = true;
        
        result.trades.push_back(trade);
        
        if (trade.profit_loss > 0) {
            result.winning_trades++;
        } else {
            result.losing_trades++;
        }
    }
    
    // Calculate metrics
    result.total_trades = result.trades.size();
    result.total_return = (capital - initial_capital) / initial_capital;
    result.sharpe_ratio = calculateSharpeRatio(returns);
    result.max_drawdown = calculateMaxDrawdown(equity_curve);
    result.win_rate = result.total_trades > 0 ? 
        static_cast<double>(result.winning_trades) / result.total_trades : 0.0;
    
    return result;
}

void Backtester::printResults(const BacktestResult& result) {
    std::cout << "\n=== Backtest Results ===" << std::endl;
    std::cout << "Total Return: " << (result.total_return * 100.0) << "%" << std::endl;
    std::cout << "Sharpe Ratio: " << result.sharpe_ratio << std::endl;
    std::cout << "Max Drawdown: " << (result.max_drawdown * 100.0) << "%" << std::endl;
    std::cout << "Win Rate: " << (result.win_rate * 100.0) << "%" << std::endl;
    std::cout << "Total Trades: " << result.total_trades << std::endl;
    std::cout << "Winning Trades: " << result.winning_trades << std::endl;
    std::cout << "Losing Trades: " << result.losing_trades << std::endl;
    std::cout << "Fitness Score: " << result.getFitness() << std::endl;
    std::cout << "========================\n" << std::endl;
}

double Backtester::calculateSharpeRatio(const std::vector<double>& returns) const {
    if (returns.empty()) {
        return 0.0;
    }
    
    double mean_return = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    
    double variance = 0.0;
    for (double ret : returns) {
        variance += (ret - mean_return) * (ret - mean_return);
    }
    variance /= returns.size();
    
    double std_dev = std::sqrt(variance);
    
    if (std_dev == 0.0) {
        return 0.0;
    }
    
    // Annualized Sharpe ratio (assuming daily returns, 252 trading days)
    return (mean_return / std_dev) * std::sqrt(252.0);
}

double Backtester::calculateMaxDrawdown(const std::vector<double>& equity_curve) const {
    if (equity_curve.empty()) {
        return 0.0;
    }
    
    double max_drawdown = 0.0;
    double peak = equity_curve[0];
    
    for (double equity : equity_curve) {
        if (equity > peak) {
            peak = equity;
        }
        
        double drawdown = (peak - equity) / peak;
        if (drawdown > max_drawdown) {
            max_drawdown = drawdown;
        }
    }
    
    return max_drawdown;
}

} // namespace trading
