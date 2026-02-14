# Test Plan: Virgox Trading Optimizer

## Document Information

- **Project**: Virgox - C++ Trading Strategy Optimizer
- **Version**: 1.0
- **Date**: February 2026
- **Purpose**: Comprehensive testing plan to verify all functionalities

## Table of Contents

1. [Test Environment Setup](#test-environment-setup)
2. [Unit Tests](#unit-tests)
3. [Integration Tests](#integration-tests)
4. [System Tests](#system-tests)
5. [Performance Tests](#performance-tests)
6. [Validation Tests](#validation-tests)
7. [Test Execution Schedule](#test-execution-schedule)
8. [Test Report Template](#test-report-template)

---

## Test Environment Setup

### Prerequisites Verification

**Test ID**: ENV-001  
**Objective**: Verify build environment is correctly configured  
**Priority**: Critical

**Steps**:
1. Check GCC/Clang version: `g++ --version` (should be 11.0+)
2. Check CMake version: `cmake --version` (should be 3.15+)
3. Check Python version: `python3 --version` (should be 3.7+)
4. Verify Git is installed: `git --version`

**Expected Result**: All tools are installed with correct versions

**Pass Criteria**: ✅ All version checks pass

---

**Test ID**: ENV-002  
**Objective**: Verify project structure is complete  
**Priority**: Critical

**Steps**:
1. Clone repository: `git clone https://github.com/simpego81/Virgox.git`
2. Navigate to project: `cd Virgox`
3. Check directory structure:
   ```bash
   ls -la src/
   ls -la tests/
   ls -la scripts/
   ls -la third_party/
   ```
4. Verify key files exist:
   - `CMakeLists.txt`
   - `src/main.cpp`
   - `scripts/get_data.py`
   - `third_party/openGA.hpp`

**Expected Result**: All directories and key files are present

**Pass Criteria**: ✅ No missing files or directories

---

## Unit Tests

### Data Module Tests

**Test ID**: UNIT-DATA-001  
**Test Name**: Load Valid CSV File  
**Module**: `src/data/DataHandler.cpp`  
**Priority**: Critical

**Test Code**: `tests/test_data.cpp::LoadValidCSV`

**Steps**:
1. Create test CSV with 3 candles
2. Call `DataHandler::loadFromCSV()`
3. Verify `size()` returns 3
4. Check first candle data matches input

**Expected Result**: Data loaded correctly, size is 3

**Pass Criteria**: ✅ Test passes without assertions failing

---

**Test ID**: UNIT-DATA-002  
**Test Name**: Handle Non-Existent File  
**Module**: `src/data/DataHandler.cpp`  
**Priority**: High

**Test Code**: `tests/test_data.cpp::LoadNonExistentCSV`

**Steps**:
1. Call `loadFromCSV()` with invalid path
2. Verify function returns `false`
3. Verify `size()` returns 0

**Expected Result**: Function handles error gracefully

**Pass Criteria**: ✅ No crash, returns false

---

**Test ID**: UNIT-DATA-003  
**Test Name**: Parse CSV Data Correctly  
**Module**: `src/data/DataHandler.cpp`  
**Priority**: Critical

**Test Code**: `tests/test_data.cpp::CheckLoadedData`

**Steps**:
1. Load test CSV
2. Verify each field of first candle:
   - timestamp = 1640995200
   - open = 100.0
   - high = 105.0
   - low = 99.0
   - close = 102.0
   - volume = 1000

**Expected Result**: All fields parsed correctly

**Pass Criteria**: ✅ All EXPECT_EQ assertions pass

---

**Test ID**: UNIT-DATA-004  
**Test Name**: Get Candles in Range  
**Module**: `src/data/DataHandler.cpp`  
**Priority**: Medium

**Test Code**: `tests/test_data.cpp::GetCandlesInRange`

**Steps**:
1. Load 3 candles
2. Request range [1, 3)
3. Verify returned vector has 2 elements
4. Check values match expected candles

**Expected Result**: Correct subset returned

**Pass Criteria**: ✅ Range query works correctly

---

### Strategy Module Tests

**Test ID**: UNIT-STRAT-001  
**Test Name**: Calculate Simple Moving Average  
**Module**: `src/strategy/Strategy.cpp`  
**Priority**: Critical

**Test Code**: `tests/test_strategy.cpp::CalculateSMA`

**Steps**:
1. Create candles with known prices: [100, 102, 104, 106, 108, 110, 108, 106, 104, 102]
2. Calculate SMA(5) at index 9
3. Expected: (110+108+106+104+102)/5 = 106.0
4. Verify result matches

**Expected Result**: SMA = 106.0

**Pass Criteria**: ✅ EXPECT_DOUBLE_EQ passes

---

**Test ID**: UNIT-STRAT-002  
**Test Name**: Calculate RSI  
**Module**: `src/strategy/Strategy.cpp`  
**Priority**: Critical

**Test Code**: `tests/test_strategy.cpp::CalculateRSI`

**Steps**:
1. Create upward trending candles (100, 101, 102, ..., 114)
2. Calculate RSI(14) at index 14
3. Expected: RSI > 99 (strong uptrend)
4. Create downward trending candles (100, 99, 98, ..., 86)
5. Calculate RSI(14)
6. Expected: RSI < 1 (strong downtrend)

**Expected Result**: RSI correctly identifies trends

**Pass Criteria**: ✅ RSI > 99 for uptrend, RSI < 1 for downtrend

---

**Test ID**: UNIT-STRAT-003  
**Test Name**: Generate Buy Signal  
**Module**: `src/strategy/Strategy.cpp`  
**Priority**: High

**Test Code**: `tests/test_strategy.cpp::GenerateBuySignal`

**Steps**:
1. Create scenario where short MA > long MA
2. Create scenario where RSI < oversold threshold
3. Call `generateSignal()`
4. Verify signal is `Signal::BUY`

**Expected Result**: BUY signal generated

**Pass Criteria**: ✅ Signal equals BUY

---

**Test ID**: UNIT-STRAT-004  
**Test Name**: Generate Sell Signal  
**Module**: `src/strategy/Strategy.cpp`  
**Priority**: High

**Test Code**: `tests/test_strategy.cpp::GenerateSellSignal`

**Steps**:
1. Create scenario where short MA < long MA
2. Create scenario where RSI > overbought threshold
3. Call `generateSignal()`
4. Verify signal is `Signal::SELL`

**Expected Result**: SELL signal generated

**Pass Criteria**: ✅ Signal equals SELL

---

### Backtesting Module Tests

**Test ID**: UNIT-BACK-001  
**Test Name**: Run Simple Backtest  
**Module**: `src/backtesting/Backtester.cpp`  
**Priority**: Critical

**Test Code**: `tests/test_backtesting.cpp::RunSimpleBacktest`

**Steps**:
1. Create upward trending data (100 candles)
2. Use strategy that always buys
3. Run backtest
4. Verify:
   - total_trades = 1
   - total_return > 0
   - winning_trades = 1
   - win_rate = 1.0

**Expected Result**: One profitable trade

**Pass Criteria**: ✅ All metrics match expectations

---

**Test ID**: UNIT-BACK-002  
**Test Name**: Verify Profit Calculation  
**Module**: `src/backtesting/Backtester.cpp`  
**Priority**: Critical

**Test Code**: `tests/test_backtesting.cpp::ProfitCalculation`

**Steps**:
1. Create known price data
2. Strategy buys at index 20 (price = 110)
3. Strategy sells at index 80 (price = 140)
4. Initial capital = 10,000
5. Calculate expected profit manually
6. Verify backtest profit matches

**Expected Result**: Profit calculation is accurate

**Pass Criteria**: ✅ Calculated profit matches expected within 0.01

---

**Test ID**: UNIT-BACK-003  
**Test Name**: Stop-Loss Functionality  
**Module**: `src/backtesting/Backtester.cpp`  
**Priority**: High

**Test Code**: `tests/test_backtesting.cpp::StopLoss`

**Steps**:
1. Create data with sharp price drop
2. Set stop-loss at 5%
3. Buy at price 115
4. Verify exit occurs when price drops to ~109.25
5. Verify trade has negative profit

**Expected Result**: Stop-loss triggers correctly

**Pass Criteria**: ✅ Exit price near stop-loss level, profit < 0

---

### Neural Network Module Tests

**Test ID**: UNIT-NN-001  
**Test Name**: Load Model  
**Module**: `src/nn/TFModel.cpp`  
**Priority**: Medium

**Manual Test** (automated test not included)

**Steps**:
1. Create `TFModel` instance
2. Call `loadModel("dummy_path")`
3. Verify `isLoaded()` returns true
4. Verify no crash occurs

**Expected Result**: Model loads (mock implementation)

**Pass Criteria**: ✅ No exceptions, isLoaded() = true

---

**Test ID**: UNIT-NN-002  
**Test Name**: Predict with Features  
**Module**: `src/nn/TFModel.cpp`  
**Priority**: Medium

**Manual Test**

**Steps**:
1. Load model
2. Create feature vector with 20 prices
3. Call `predict(features)`
4. Verify return value is between -1.0 and 1.0

**Expected Result**: Prediction in valid range

**Pass Criteria**: ✅ -1.0 ≤ prediction ≤ 1.0

---

### Genetic Algorithm Module Tests

**Test ID**: UNIT-GA-001  
**Test Name**: Generate Random Parameters  
**Module**: `src/ga/GeneticOptimizer.cpp`  
**Priority**: Medium

**Manual Test**

**Steps**:
1. Call `generateRandomParams()` 100 times
2. Verify all parameters are within valid ranges:
   - short_ma_period: [5, 30]
   - long_ma_period: [30, 100]
   - rsi_period: [10, 20]
   - rsi_oversold: [20, 35]
   - rsi_overbought: [65, 80]
   - stop_loss_pct: [0.01, 0.05]
   - take_profit_pct: [0.02, 0.10]
   - nn_weight: [0.0, 1.0]

**Expected Result**: All parameters in valid ranges

**Pass Criteria**: ✅ No out-of-range values

---

**Test ID**: UNIT-GA-002  
**Test Name**: Crossover Operation  
**Module**: `src/ga/GeneticOptimizer.cpp`  
**Priority**: Low

**Manual Test**

**Steps**:
1. Create two parent strategies with different parameters
2. Call `crossover(parent1, parent2)`
3. Verify offspring has mix of parent parameters
4. Repeat 100 times, verify randomness

**Expected Result**: Offspring combines parent traits

**Pass Criteria**: ✅ Offspring parameters come from either parent

---

**Test ID**: UNIT-GA-003  
**Test Name**: Mutation Operation  
**Module**: `src/ga/GeneticOptimizer.cpp`  
**Priority**: Low

**Manual Test**

**Steps**:
1. Create strategy with known parameters
2. Call `mutate()` 100 times
3. Verify at least one parameter changes each time
4. Verify mutated values are still in valid ranges

**Expected Result**: Mutation changes parameters

**Pass Criteria**: ✅ Parameters change, remain valid

---

## Integration Tests

### Data to Strategy Integration

**Test ID**: INT-001  
**Test Name**: Load Data and Generate Signals  
**Priority**: High

**Steps**:
1. Load real CSV file (ENEL.MI)
2. Create strategy with default parameters
3. Iterate through all candles
4. Generate signal for each candle
5. Verify signals are generated without errors
6. Check signal distribution (not all BUY or all SELL)

**Expected Result**: Signals generated for all candles

**Pass Criteria**: 
- ✅ No crashes
- ✅ At least 10% of signals are BUY
- ✅ At least 10% of signals are SELL

---

### Strategy to Backtesting Integration

**Test ID**: INT-002  
**Test Name**: Backtest with Real Data  
**Priority**: Critical

**Steps**:
1. Load ENEL.MI data (2 years)
2. Create strategy with reasonable parameters:
   - short_ma: 10, long_ma: 50
   - rsi: 14, oversold: 30, overbought: 70
   - stop_loss: 2%, take_profit: 5%
3. Run backtest
4. Verify results are reasonable:
   - Total return: -50% to +100%
   - Sharpe ratio: -2 to +3
   - Max drawdown: 0% to 50%
   - Win rate: 30% to 70%
   - Total trades: > 0

**Expected Result**: Backtest completes with reasonable metrics

**Pass Criteria**: ✅ All metrics in expected ranges

---

### Genetic Algorithm to Backtesting Integration

**Test ID**: INT-003  
**Test Name**: GA Optimization Improves Fitness  
**Priority**: Critical

**Steps**:
1. Load ENEL.MI data
2. Configure GA:
   - Population: 20
   - Generations: 10
   - Mutation rate: 0.15
3. Run optimization
4. Record initial best fitness (generation 0)
5. Record final best fitness (generation 10)
6. Verify final > initial

**Expected Result**: Fitness improves over generations

**Pass Criteria**: ✅ Final fitness > Initial fitness

---

### Neural Network Integration

**Test ID**: INT-004  
**Test Name**: Strategy Uses NN Predictions  
**Priority**: Medium

**Steps**:
1. Load data
2. Create TFModel and load (mock)
3. Create strategy with nn_weight = 0.8
4. Generate signals with and without NN
5. Verify signals differ when NN is used

**Expected Result**: NN influences strategy decisions

**Pass Criteria**: ✅ At least 20% of signals differ with NN

---

## System Tests

### End-to-End Workflow

**Test ID**: SYS-001  
**Test Name**: Complete Optimization Workflow  
**Priority**: Critical

**Steps**:
1. Download fresh data:
   ```bash
   cd scripts
   python3 get_data.py ENEL.MI 2y
   ```
2. Verify CSV created in `data/`
3. Build project:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
4. Run optimizer:
   ```bash
   ./trading_optimizer ../data/ENEL.MI.csv --generations 20 --population 30
   ```
5. Verify output includes:
   - "Starting Genetic Algorithm Optimization"
   - Progress updates every 10 generations
   - "Optimization Complete"
   - Final backtest results
   - Fitness evolution

**Expected Result**: Complete workflow executes successfully

**Pass Criteria**: 
- ✅ Data downloaded
- ✅ Build successful
- ✅ Application runs to completion
- ✅ Results displayed

**Execution Time**: ~5-10 minutes

---

### Multi-Stock Testing

**Test ID**: SYS-002  
**Test Name**: Optimize Multiple Italian Stocks  
**Priority**: Medium

**Steps**:
1. Download data for 5 stocks:
   - ENEL.MI
   - ENI.MI
   - ISP.MI
   - UCG.MI
   - G.MI
2. Run optimizer on each stock
3. Compare results across stocks
4. Verify different optimal parameters for each

**Expected Result**: Optimizer works on all stocks

**Pass Criteria**: 
- ✅ All 5 stocks complete successfully
- ✅ Parameters differ between stocks

**Execution Time**: ~30-40 minutes

---

### Command-Line Interface

**Test ID**: SYS-003  
**Test Name**: Test CLI Arguments  
**Priority**: High

**Steps**:
1. Test with no arguments:
   ```bash
   ./trading_optimizer
   ```
   Expected: Usage message displayed

2. Test with invalid file:
   ```bash
   ./trading_optimizer nonexistent.csv
   ```
   Expected: Error message, exit code 1

3. Test with custom parameters:
   ```bash
   ./trading_optimizer ../data/ENEL.MI.csv --generations 5 --population 10
   ```
   Expected: Runs with specified parameters

4. Test help (if implemented):
   ```bash
   ./trading_optimizer --help
   ```

**Expected Result**: CLI handles all cases correctly

**Pass Criteria**: ✅ All scenarios produce expected behavior

---

## Performance Tests

### Execution Time

**Test ID**: PERF-001  
**Test Name**: Optimization Performance  
**Priority**: Medium

**Steps**:
1. Load ENEL.MI data (506 candles)
2. Configure GA:
   - Population: 50
   - Generations: 100
3. Measure execution time
4. Record time per generation

**Expected Result**: Completes in reasonable time

**Pass Criteria**: 
- ✅ Total time < 10 minutes
- ✅ Time per generation < 6 seconds

---

**Test ID**: PERF-002  
**Test Name**: Backtest Performance  
**Priority**: Low

**Steps**:
1. Load 2 years of data (~500 candles)
2. Run single backtest
3. Measure execution time

**Expected Result**: Backtest is fast

**Pass Criteria**: ✅ Single backtest < 100ms

---

### Memory Usage

**Test ID**: PERF-003  
**Test Name**: Memory Consumption  
**Priority**: Low

**Steps**:
1. Run optimizer with large population (100)
2. Monitor memory usage with `top` or `htop`
3. Verify no memory leaks (use valgrind if available)

**Expected Result**: Memory usage is reasonable

**Pass Criteria**: 
- ✅ Peak memory < 500 MB
- ✅ No memory leaks detected

---

## Validation Tests

### Financial Correctness

**Test ID**: VAL-001  
**Test Name**: Verify Sharpe Ratio Calculation  
**Priority**: High

**Steps**:
1. Create synthetic returns: [0.01, 0.02, -0.01, 0.03, 0.01]
2. Calculate Sharpe manually:
   - Mean = 0.012
   - Std Dev = 0.0141
   - Sharpe (daily) = 0.012 / 0.0141 = 0.851
   - Sharpe (annualized) = 0.851 * sqrt(252) = 13.5
3. Run backtest and compare

**Expected Result**: Sharpe ratio matches manual calculation

**Pass Criteria**: ✅ Difference < 0.1

---

**Test ID**: VAL-002  
**Test Name**: Verify Max Drawdown Calculation  
**Priority**: High

**Steps**:
1. Create equity curve: [10000, 11000, 10500, 12000, 9000, 9500]
2. Calculate max drawdown manually:
   - Peak = 12000
   - Trough = 9000
   - Drawdown = (12000 - 9000) / 12000 = 25%
3. Run backtest and compare

**Expected Result**: Max drawdown = 25%

**Pass Criteria**: ✅ Exact match

---

**Test ID**: VAL-003  
**Test Name**: Verify Position Sizing  
**Priority**: Critical

**Steps**:
1. Initial capital = 10,000
2. Buy signal at price = 100
3. Expected shares = 10,000 / 100 = 100
4. Price rises to 110
5. Expected equity = 100 * 110 = 11,000
6. Verify backtest calculates same

**Expected Result**: Position sizing is correct

**Pass Criteria**: ✅ Equity matches expected

---

### Edge Cases

**Test ID**: VAL-004  
**Test Name**: Handle Insufficient Data  
**Priority**: Medium

**Steps**:
1. Create CSV with only 10 candles
2. Attempt to run optimizer
3. Verify graceful error handling

**Expected Result**: Error message, no crash

**Pass Criteria**: ✅ Informative error, clean exit

---

**Test ID**: VAL-005  
**Test Name**: Handle Zero Volume  
**Priority**: Low

**Steps**:
1. Create CSV with some zero-volume candles
2. Run backtest
3. Verify no division by zero

**Expected Result**: Handles gracefully

**Pass Criteria**: ✅ No crash, reasonable results

---

**Test ID**: VAL-006  
**Test Name**: Handle Flat Prices  
**Priority**: Low

**Steps**:
1. Create CSV where all prices are identical (e.g., 100)
2. Run optimizer
3. Verify no trades are executed (no opportunities)

**Expected Result**: Zero trades, zero return

**Pass Criteria**: ✅ total_trades = 0, total_return = 0

---

## Test Execution Schedule

### Phase 1: Unit Testing (Day 1)
- Execute all UNIT-DATA tests (4 tests)
- Execute all UNIT-STRAT tests (4 tests)
- Execute all UNIT-BACK tests (3 tests)
- Execute UNIT-NN tests (2 tests)
- Execute UNIT-GA tests (3 tests)
- **Total**: 16 unit tests
- **Expected Duration**: 2-3 hours

### Phase 2: Integration Testing (Day 2)
- Execute INT-001 through INT-004
- **Total**: 4 integration tests
- **Expected Duration**: 3-4 hours

### Phase 3: System Testing (Day 3)
- Execute SYS-001 (critical path)
- Execute SYS-002 (multi-stock)
- Execute SYS-003 (CLI)
- **Total**: 3 system tests
- **Expected Duration**: 4-5 hours

### Phase 4: Performance & Validation (Day 4)
- Execute PERF-001 through PERF-003
- Execute VAL-001 through VAL-006
- **Total**: 9 tests
- **Expected Duration**: 3-4 hours

---

## Test Report Template

### Test Execution Summary

**Date**: _____________  
**Tester**: _____________  
**Environment**: _____________

| Test ID | Test Name | Status | Notes |
|---------|-----------|--------|-------|
| ENV-001 | Prerequisites Check | ⬜ Pass / ⬜ Fail | |
| ENV-002 | Project Structure | ⬜ Pass / ⬜ Fail | |
| UNIT-DATA-001 | Load Valid CSV | ⬜ Pass / ⬜ Fail | |
| UNIT-DATA-002 | Handle Non-Existent File | ⬜ Pass / ⬜ Fail | |
| UNIT-DATA-003 | Parse CSV Data | ⬜ Pass / ⬜ Fail | |
| UNIT-DATA-004 | Get Candles in Range | ⬜ Pass / ⬜ Fail | |
| UNIT-STRAT-001 | Calculate SMA | ⬜ Pass / ⬜ Fail | |
| UNIT-STRAT-002 | Calculate RSI | ⬜ Pass / ⬜ Fail | |
| UNIT-STRAT-003 | Generate Buy Signal | ⬜ Pass / ⬜ Fail | |
| UNIT-STRAT-004 | Generate Sell Signal | ⬜ Pass / ⬜ Fail | |
| UNIT-BACK-001 | Run Simple Backtest | ⬜ Pass / ⬜ Fail | |
| UNIT-BACK-002 | Verify Profit Calculation | ⬜ Pass / ⬜ Fail | |
| UNIT-BACK-003 | Stop-Loss Functionality | ⬜ Pass / ⬜ Fail | |
| INT-001 | Data to Strategy | ⬜ Pass / ⬜ Fail | |
| INT-002 | Strategy to Backtesting | ⬜ Pass / ⬜ Fail | |
| INT-003 | GA Optimization | ⬜ Pass / ⬜ Fail | |
| INT-004 | NN Integration | ⬜ Pass / ⬜ Fail | |
| SYS-001 | End-to-End Workflow | ⬜ Pass / ⬜ Fail | |
| SYS-002 | Multi-Stock Testing | ⬜ Pass / ⬜ Fail | |
| SYS-003 | CLI Arguments | ⬜ Pass / ⬜ Fail | |
| PERF-001 | Optimization Performance | ⬜ Pass / ⬜ Fail | |
| PERF-002 | Backtest Performance | ⬜ Pass / ⬜ Fail | |
| PERF-003 | Memory Usage | ⬜ Pass / ⬜ Fail | |
| VAL-001 | Sharpe Ratio | ⬜ Pass / ⬜ Fail | |
| VAL-002 | Max Drawdown | ⬜ Pass / ⬜ Fail | |
| VAL-003 | Position Sizing | ⬜ Pass / ⬜ Fail | |
| VAL-004 | Insufficient Data | ⬜ Pass / ⬜ Fail | |
| VAL-005 | Zero Volume | ⬜ Pass / ⬜ Fail | |
| VAL-006 | Flat Prices | ⬜ Pass / ⬜ Fail | |

### Summary Statistics

- **Total Tests**: 29
- **Passed**: _____
- **Failed**: _____
- **Skipped**: _____
- **Pass Rate**: _____%

### Critical Issues Found

1. _____________________________________________
2. _____________________________________________
3. _____________________________________________

### Recommendations

_____________________________________________
_____________________________________________
_____________________________________________

### Sign-off

**Tester Signature**: _____________  
**Date**: _____________

---

## Appendix: Quick Test Commands

### Run All Unit Tests
```bash
cd build
ctest --output-on-failure
```

### Run Specific Test Suite
```bash
./tests/test_data
./tests/test_strategy
./tests/test_backtesting
```

### Run with Verbose Output
```bash
./tests/test_data --gtest_verbose
```

### Run Single Test
```bash
./tests/test_data --gtest_filter=DataHandlerTest.LoadValidCSV
```

### Memory Check (if valgrind available)
```bash
valgrind --leak-check=full ./trading_optimizer ../data/ENEL.MI.csv
```

### Performance Profiling (if gprof available)
```bash
# Compile with -pg flag
cmake -DCMAKE_CXX_FLAGS=-pg ..
make
./trading_optimizer ../data/ENEL.MI.csv
gprof trading_optimizer gmon.out > analysis.txt
```

---

**End of Test Plan**
