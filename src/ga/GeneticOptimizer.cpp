#include "GeneticOptimizer.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

namespace trading {

static std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));

GeneticOptimizer::GeneticOptimizer() {}

GeneticOptimizer::GeneticOptimizer(const GAConfig& config) : config_(config) {}

GeneticOptimizer::~GeneticOptimizer() {}

void GeneticOptimizer::setConfig(const GAConfig& config) {
    config_ = config;
}

const std::vector<double>& GeneticOptimizer::getFitnessHistory() const {
    return fitness_history_;
}

StrategyParams GeneticOptimizer::optimize(const DataHandler& data, TFModel* tf_model) {
    std::cout << "\n=== Starting Genetic Algorithm Optimization ===" << std::endl;
    std::cout << "Population size: " << config_.population_size << std::endl;
    std::cout << "Max generations: " << config_.max_generations << std::endl;
    
    fitness_history_.clear();
    
    // Initialize population
    std::vector<StrategyParams> population;
    std::vector<double> fitness_scores;
    
    for (int i = 0; i < config_.population_size; ++i) {
        population.push_back(generateRandomParams());
    }
    
    StrategyParams best_params;
    double best_fitness = -1e9;
    
    // Evolution loop
    for (int generation = 0; generation < config_.max_generations; ++generation) {
        // Evaluate fitness for all individuals
        fitness_scores.clear();
        for (const auto& params : population) {
            double fitness = evaluateFitness(params, data, tf_model);
            fitness_scores.push_back(fitness);
            
            if (fitness > best_fitness) {
                best_fitness = fitness;
                best_params = params;
            }
        }
        
        fitness_history_.push_back(best_fitness);
        
        if (generation % 10 == 0) {
            std::cout << "Generation " << generation << " - Best Fitness: " << best_fitness << std::endl;
        }
        
        // Selection and reproduction
        std::vector<StrategyParams> new_population;
        
        // Elitism: keep best individuals
        std::vector<size_t> indices(population.size());
        for (size_t i = 0; i < indices.size(); ++i) indices[i] = i;
        
        std::sort(indices.begin(), indices.end(), 
                  [&fitness_scores](size_t a, size_t b) { 
                      return fitness_scores[a] > fitness_scores[b]; 
                  });
        
        for (int i = 0; i < config_.elite_count && i < config_.population_size; ++i) {
            new_population.push_back(population[indices[i]]);
        }
        
        // Generate offspring through crossover and mutation
        while (new_population.size() < static_cast<size_t>(config_.population_size)) {
            // Tournament selection
            int parent1_idx = randomInt(0, config_.population_size - 1);
            int parent2_idx = randomInt(0, config_.population_size - 1);
            
            // Pick better parent from tournament
            if (fitness_scores[parent1_idx] < fitness_scores[randomInt(0, config_.population_size - 1)]) {
                parent1_idx = randomInt(0, config_.population_size - 1);
            }
            if (fitness_scores[parent2_idx] < fitness_scores[randomInt(0, config_.population_size - 1)]) {
                parent2_idx = randomInt(0, config_.population_size - 1);
            }
            
            StrategyParams offspring;
            
            // Crossover
            if (randomDouble(0.0, 1.0) < config_.crossover_rate) {
                offspring = crossover(population[parent1_idx], population[parent2_idx]);
            } else {
                offspring = population[parent1_idx];
            }
            
            // Mutation
            if (randomDouble(0.0, 1.0) < config_.mutation_rate) {
                mutate(offspring);
            }
            
            new_population.push_back(offspring);
        }
        
        population = new_population;
    }
    
    std::cout << "\n=== Optimization Complete ===" << std::endl;
    std::cout << "Best Fitness: " << best_fitness << std::endl;
    std::cout << "Best Strategy: " << std::endl;
    Strategy best_strategy(best_params);
    std::cout << best_strategy.getDescription() << std::endl;
    
    return best_params;
}

StrategyParams GeneticOptimizer::generateRandomParams() {
    StrategyParams params;
    
    params.short_ma_period = randomInt(5, 30);
    params.long_ma_period = randomInt(30, 100);
    params.rsi_period = randomInt(10, 20);
    params.rsi_oversold = randomDouble(20.0, 35.0);
    params.rsi_overbought = randomDouble(65.0, 80.0);
    params.stop_loss_pct = randomDouble(0.01, 0.05);
    params.take_profit_pct = randomDouble(0.02, 0.10);
    params.nn_weight = randomDouble(0.0, 1.0);
    
    return params;
}

double GeneticOptimizer::evaluateFitness(const StrategyParams& params, const DataHandler& data, TFModel* tf_model) {
    Strategy strategy(params);
    BacktestResult result = backtester_.runBacktest(data, strategy, tf_model);
    return result.getFitness();
}

StrategyParams GeneticOptimizer::crossover(const StrategyParams& parent1, const StrategyParams& parent2) {
    StrategyParams offspring;
    
    // Uniform crossover
    offspring.short_ma_period = (randomDouble(0.0, 1.0) < 0.5) ? parent1.short_ma_period : parent2.short_ma_period;
    offspring.long_ma_period = (randomDouble(0.0, 1.0) < 0.5) ? parent1.long_ma_period : parent2.long_ma_period;
    offspring.rsi_period = (randomDouble(0.0, 1.0) < 0.5) ? parent1.rsi_period : parent2.rsi_period;
    offspring.rsi_oversold = (randomDouble(0.0, 1.0) < 0.5) ? parent1.rsi_oversold : parent2.rsi_oversold;
    offspring.rsi_overbought = (randomDouble(0.0, 1.0) < 0.5) ? parent1.rsi_overbought : parent2.rsi_overbought;
    offspring.stop_loss_pct = (randomDouble(0.0, 1.0) < 0.5) ? parent1.stop_loss_pct : parent2.stop_loss_pct;
    offspring.take_profit_pct = (randomDouble(0.0, 1.0) < 0.5) ? parent1.take_profit_pct : parent2.take_profit_pct;
    offspring.nn_weight = (randomDouble(0.0, 1.0) < 0.5) ? parent1.nn_weight : parent2.nn_weight;
    
    return offspring;
}

void GeneticOptimizer::mutate(StrategyParams& params) {
    // Randomly mutate one parameter
    int param_to_mutate = randomInt(0, 7);
    
    switch (param_to_mutate) {
        case 0:
            params.short_ma_period = randomInt(5, 30);
            break;
        case 1:
            params.long_ma_period = randomInt(30, 100);
            break;
        case 2:
            params.rsi_period = randomInt(10, 20);
            break;
        case 3:
            params.rsi_oversold = randomDouble(20.0, 35.0);
            break;
        case 4:
            params.rsi_overbought = randomDouble(65.0, 80.0);
            break;
        case 5:
            params.stop_loss_pct = randomDouble(0.01, 0.05);
            break;
        case 6:
            params.take_profit_pct = randomDouble(0.02, 0.10);
            break;
        case 7:
            params.nn_weight = randomDouble(0.0, 1.0);
            break;
    }
}

double GeneticOptimizer::randomDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

int GeneticOptimizer::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

} // namespace trading
