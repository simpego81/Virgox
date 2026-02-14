#include <iostream>
#include <string>
#include "data/DataHandler.h"
#include "strategy/Strategy.h"
#include "backtesting/Backtester.h"
#include "ga/GeneticOptimizer.h"
#include "nn/TFModel.h"

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <data_file.csv> [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --model <path>       Path to TensorFlow model (optional)" << std::endl;
    std::cout << "  --generations <n>    Number of GA generations (default: 100)" << std::endl;
    std::cout << "  --population <n>     GA population size (default: 50)" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  " << program_name << " data/ENEL.MI.csv --generations 50" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Trading Strategy Optimizer ===" << std::endl;
    std::cout << "Combining Genetic Algorithms with Neural Networks" << std::endl;
    std::cout << "for Italian Stock Exchange (Borsa Italiana)\n" << std::endl;
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string data_file = argv[1];
    std::string model_path;
    int generations = 100;
    int population = 50;
    
    // Parse command line arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--model" && i + 1 < argc) {
            model_path = argv[++i];
        } else if (arg == "--generations" && i + 1 < argc) {
            generations = std::stoi(argv[++i]);
        } else if (arg == "--population" && i + 1 < argc) {
            population = std::stoi(argv[++i]);
        }
    }
    
    // Load market data
    trading::DataHandler data_handler;
    if (!data_handler.loadFromCSV(data_file)) {
        std::cerr << "Failed to load data from: " << data_file << std::endl;
        return 1;
    }
    
    std::cout << "Loaded " << data_handler.size() << " data points" << std::endl;
    
    // Load TensorFlow model (optional)
    trading::TFModel* tf_model = nullptr;
    if (!model_path.empty()) {
        tf_model = new trading::TFModel();
        if (!tf_model->loadModel(model_path)) {
            std::cerr << "Warning: Failed to load TensorFlow model" << std::endl;
            delete tf_model;
            tf_model = nullptr;
        }
    }
    
    // Configure genetic algorithm
    trading::GAConfig ga_config;
    ga_config.max_generations = generations;
    ga_config.population_size = population;
    ga_config.mutation_rate = 0.15;
    ga_config.crossover_rate = 0.7;
    ga_config.elite_count = 5;
    
    // Run optimization
    trading::GeneticOptimizer optimizer(ga_config);
    trading::StrategyParams best_params = optimizer.optimize(data_handler, tf_model);
    
    // Run final backtest with best parameters
    std::cout << "\n=== Final Backtest with Optimized Strategy ===" << std::endl;
    trading::Strategy best_strategy(best_params);
    trading::Backtester backtester;
    trading::BacktestResult final_result = backtester.runBacktest(data_handler, best_strategy, tf_model);
    
    trading::Backtester::printResults(final_result);
    
    // Print fitness evolution
    const auto& fitness_history = optimizer.getFitnessHistory();
    std::cout << "\nFitness Evolution (every 10 generations):" << std::endl;
    for (size_t i = 0; i < fitness_history.size(); i += 10) {
        std::cout << "Gen " << i << ": " << fitness_history[i] << std::endl;
    }
    
    // Cleanup
    if (tf_model != nullptr) {
        delete tf_model;
    }
    
    std::cout << "\nOptimization complete!" << std::endl;
    return 0;
}
