# AI Agent Guide: Virgox Trading Optimizer

## Project Overview

**Virgox** is a C++ application that combines Genetic Algorithms with TensorFlow to optimize trading strategies for the Italian Stock Exchange (Borsa Italiana). The system performs backtesting on historical data and uses evolutionary algorithms to find optimal strategy parameters.

## Architecture

The project is organized into the following modules:

### 1. Data Module (`src/data/`)
- **Purpose**: Load and parse historical stock data from CSV files
- **Key Classes**:
  - `Candle`: Represents a single candlestick (OHLCV data)
  - `DataHandler`: Loads CSV files and provides access to historical data
- **Data Format**: CSV with columns: `timestamp,open,high,low,close,volume`

### 2. Strategy Module (`src/strategy/`)
- **Purpose**: Define trading strategies and generate buy/sell signals
- **Key Classes**:
  - `StrategyParams`: Contains parameters optimized by the GA (MA periods, RSI levels, stop-loss, take-profit, neural network weight)
  - `Strategy`: Generates trading signals based on technical indicators (SMA, RSI) and optional neural network predictions
- **Signals**: `BUY`, `SELL`, `HOLD`

### 3. Backtesting Module (`src/backtesting/`)
- **Purpose**: Simulate trading strategies on historical data
- **Key Classes**:
  - `Trade`: Represents a single trade with entry/exit prices and profit/loss
  - `BacktestResult`: Contains performance metrics (total return, Sharpe ratio, max drawdown, win rate)
  - `Backtester`: Executes the backtest simulation
- **Features**: Supports stop-loss and take-profit mechanisms

### 4. Neural Network Module (`src/nn/`)
- **Purpose**: Integrate TensorFlow for price prediction
- **Key Classes**:
  - `TFModel`: Interface for loading and using TensorFlow models
- **Current Status**: Mock implementation using simple momentum calculation. In production, this should load a real TensorFlow model trained in Python.

### 5. Genetic Algorithm Module (`src/ga/`)
- **Purpose**: Optimize strategy parameters using evolutionary algorithms
- **Key Classes**:
  - `GAConfig`: Configuration for the genetic algorithm (population size, generations, mutation rate, etc.)
  - `GeneticOptimizer`: Implements the GA optimization loop
- **Process**: 
  1. Generate random population of strategies
  2. Evaluate fitness via backtesting
  3. Select best performers (elitism)
  4. Create offspring via crossover and mutation
  5. Repeat for N generations

### 6. Main Application (`src/main.cpp`)
- Orchestrates the entire workflow:
  1. Load historical data
  2. Optionally load TensorFlow model
  3. Run genetic algorithm optimization
  4. Display best strategy and results

## Build Instructions

### Prerequisites
- **C++ Compiler**: GCC 11+ or Clang 12+ with C++17 support
- **CMake**: Version 3.15 or higher
- **Python 3**: For data fetching script
- **Git**: For cloning dependencies

### Step-by-Step Build Process

1. **Clone the repository**:
   ```bash
   git clone https://github.com/simpego81/Virgox.git
   cd Virgox
   ```

2. **Ensure dependencies are present**:
   - Google Test is included as a submodule in `third_party/googletest`
   - OpenGA is a header-only library in `third_party/openGA.hpp`

3. **Create build directory**:
   ```bash
   mkdir -p build
   cd build
   ```

4. **Configure with CMake**:
   ```bash
   cmake ..
   ```

5. **Compile the project**:
   ```bash
   make -j$(nproc)
   ```

6. **Expected output**:
   - `trading_optimizer` (main executable)
   - `test_data`, `test_strategy`, `test_backtesting` (unit test executables)

### Common Build Issues and Solutions

#### Issue 1: Missing `size_t` or `ptrdiff_t` errors
**Cause**: Missing standard library headers in test files.
**Solution**: Add `#include <cstddef>` to affected files.

#### Issue 2: Virtual method override errors
**Cause**: The `generateSignal` method in `Strategy.h` must be declared as `virtual`.
**Solution**: Ensure line 54 of `src/strategy/Strategy.h` has:
```cpp
virtual Signal generateSignal(const std::vector<Candle>& candles, size_t current_idx, double nn_prediction = 0.0) const;
```

#### Issue 3: Google Test not found
**Cause**: CMake cannot locate Google Test.
**Solution**: The project includes Google Test as a subdirectory. Ensure `third_party/googletest` exists and is properly added in the root `CMakeLists.txt`.

## Running the Application

### 1. Fetch Historical Data

First, download stock data using the Python script:

```bash
cd scripts
python3 get_data.py ENEL.MI 2y
```

This creates `data/ENEL.MI.csv` with 2 years of daily data for Enel SpA.

**Other Italian stocks**:
- `ENI.MI` - Eni SpA
- `ISP.MI` - Intesa Sanpaolo
- `UCG.MI` - UniCredit
- `G.MI` - Generali

### 2. Run the Optimizer

From the `build` directory:

```bash
./trading_optimizer ../data/ENEL.MI.csv --generations 50 --population 30
```

**Command-line options**:
- `--model <path>`: Path to TensorFlow model (optional)
- `--generations <n>`: Number of GA generations (default: 100)
- `--population <n>`: GA population size (default: 50)

### 3. Expected Output

The application will:
1. Load the data
2. Run the genetic algorithm optimization
3. Print progress every 10 generations
4. Display the best strategy found
5. Run a final backtest with detailed metrics

Example output:
```
=== Starting Genetic Algorithm Optimization ===
Population size: 50
Max generations: 100
Generation 0 - Best Fitness: 0.234
Generation 10 - Best Fitness: 0.456
...
=== Optimization Complete ===
Best Fitness: 0.789
Best Strategy: Strategy[SMA:15/45, RSI:14(28.5,72.3), SL:0.03, TP:0.07, NN:0.6]

=== Final Backtest with Optimized Strategy ===
Total Return: 23.45%
Sharpe Ratio: 1.87
Max Drawdown: 8.23%
Win Rate: 62.5%
Total Trades: 24
```

## Running Unit Tests

### Execute all tests:
```bash
cd build
ctest --output-on-failure
```

### Run individual test suites:
```bash
./tests/test_data
./tests/test_strategy
./tests/test_backtesting
```

### Test Coverage

1. **test_data**: Validates CSV loading, data parsing, and range queries
2. **test_strategy**: Tests SMA/RSI calculation and signal generation
3. **test_backtesting**: Verifies trade execution, profit calculation, and stop-loss functionality

## Debugging Guide

### Using GDB

1. **Compile with debug symbols**:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   make
   ```

2. **Run with GDB**:
   ```bash
   gdb ./trading_optimizer
   (gdb) run ../data/ENEL.MI.csv
   ```

3. **Useful GDB commands**:
   - `break main` - Set breakpoint at main
   - `break GeneticOptimizer::optimize` - Break in GA optimization
   - `print variable_name` - Inspect variable
   - `backtrace` - Show call stack
   - `continue` - Continue execution

### Using Visual Studio Code

1. **Install C++ extension**: ms-vscode.cpptools

2. **Create `.vscode/launch.json`**:
   ```json
   {
     "version": "0.2.0",
     "configurations": [
       {
         "name": "Debug Trading Optimizer",
         "type": "cppdbg",
         "request": "launch",
         "program": "${workspaceFolder}/build/trading_optimizer",
         "args": ["${workspaceFolder}/data/ENEL.MI.csv", "--generations", "10"],
         "stopAtEntry": false,
         "cwd": "${workspaceFolder}/build",
         "environment": [],
         "externalConsole": false,
         "MIMode": "gdb",
         "setupCommands": [
           {
             "description": "Enable pretty-printing for gdb",
             "text": "-enable-pretty-printing",
             "ignoreFailures": true
           }
         ]
       }
     ]
   }
   ```

3. **Set breakpoints** in the editor and press F5 to start debugging.

## Verification Checklist

To ensure the project works correctly, verify the following:

### ✅ Compilation
- [ ] Project compiles without errors
- [ ] All unit tests compile
- [ ] No warnings related to undefined behavior

### ✅ Unit Tests
- [ ] `test_data` passes all tests
- [ ] `test_strategy` passes all tests
- [ ] `test_backtesting` passes all tests

### ✅ Data Fetching
- [ ] Python script successfully downloads data
- [ ] CSV file is created in `data/` directory
- [ ] CSV contains valid OHLCV data

### ✅ Application Execution
- [ ] Application loads CSV data without errors
- [ ] Genetic algorithm runs and shows progress
- [ ] Final backtest results are displayed
- [ ] Fitness improves over generations

### ✅ Correctness Validation
- [ ] Strategy generates reasonable signals (not all BUY or all SELL)
- [ ] Backtester correctly calculates profit/loss
- [ ] Stop-loss triggers when price drops
- [ ] Take-profit triggers when price rises
- [ ] Sharpe ratio is calculated (should be between -3 and 3 typically)
- [ ] Max drawdown is between 0 and 1 (0% to 100%)

## Known Limitations

1. **TensorFlow Integration**: Currently a mock implementation. To use real TensorFlow:
   - Train a model in Python and export as `.pb` file
   - Install TensorFlow C++ API
   - Update `TFModel.cpp` to load and run inference

2. **Single Asset**: Currently optimizes for one stock at a time. Could be extended to portfolio optimization.

3. **Transaction Costs**: Not currently modeled. Real trading has commissions and slippage.

4. **Overfitting Risk**: Optimizing on the same data used for backtesting can lead to overfitting. Use train/test split in production.

## Extension Ideas

1. **Add more technical indicators**: MACD, Bollinger Bands, Volume analysis
2. **Multi-timeframe analysis**: Combine signals from different timeframes
3. **Portfolio optimization**: Optimize across multiple stocks simultaneously
4. **Walk-forward optimization**: Periodically re-optimize on recent data
5. **Real TensorFlow integration**: Train LSTM/Transformer models for price prediction
6. **Live trading interface**: Connect to broker API for paper/live trading

## Troubleshooting

### Problem: Segmentation fault during execution
**Check**:
- Data file exists and is valid CSV
- No out-of-bounds array access in strategy calculation
- Sufficient data points (need at least 100 candles)

### Problem: GA fitness doesn't improve
**Possible causes**:
- Data quality issues (missing values, zeros)
- Strategy parameters too constrained
- Population size too small
- Mutation rate too low

### Problem: Unrealistic backtest results (>1000% returns)
**Likely causes**:
- Bug in profit calculation
- Incorrect position sizing
- Data errors (e.g., stock split not adjusted)

## Contact and Support

For issues or questions about this project, please open an issue on the GitHub repository: https://github.com/simpego81/Virgox

---

**Note to AI Agent**: This project is a proof-of-concept for educational purposes. When assisting with development, prioritize code correctness, proper error handling, and realistic financial modeling. Always validate calculations against known financial formulas.
