#ifndef GENETIC_OPTIMIZER_H
#define GENETIC_OPTIMIZER_H

#include "strategy/Strategy.h"
#include "backtesting/Backtester.h"
#include "data/DataHandler.h"
#include "nn/TFModel.h"
#include <vector>
#include <functional>

namespace trading {

struct GAConfig {
    int population_size;
    int max_generations;
    double mutation_rate;
    double crossover_rate;
    int elite_count;
    
    GAConfig() 
        : population_size(50), max_generations(100), 
          mutation_rate(0.1), crossover_rate(0.7), elite_count(5) {}
};

class GeneticOptimizer {
public:
    GeneticOptimizer();
    explicit GeneticOptimizer(const GAConfig& config);
    ~GeneticOptimizer();
    
    // Run genetic algorithm to find optimal strategy parameters
    StrategyParams optimize(const DataHandler& data, TFModel* tf_model = nullptr);
    
    // Set configuration
    void setConfig(const GAConfig& config);
    
    // Get best fitness history
    const std::vector<double>& getFitnessHistory() const;
    
private:
    GAConfig config_;
    Backtester backtester_;
    std::vector<double> fitness_history_;
    
    // Generate random strategy parameters
    StrategyParams generateRandomParams();
    
    // Evaluate fitness of a strategy
    double evaluateFitness(const StrategyParams& params, const DataHandler& data, TFModel* tf_model);
    
    // Crossover two parent strategies
    StrategyParams crossover(const StrategyParams& parent1, const StrategyParams& parent2);
    
    // Mutate strategy parameters
    void mutate(StrategyParams& params);
    
    // Random double in range
    double randomDouble(double min, double max);
    
    // Random int in range
    int randomInt(int min, int max);
};

} // namespace trading

#endif // GENETIC_OPTIMIZER_H
