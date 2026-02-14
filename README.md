# C++ Trading Strategy Optimizer

This project is a C++ application that uses a combination of Genetic Algorithms (GA) and TensorFlow to discover and optimize trading strategies for stocks on the Borsa Italiana (Italian Stock Exchange).

## Features

- **Genetic Algorithm**: Optimizes trading strategy parameters (e.g., moving average periods, RSI levels) to maximize profitability.
- **TensorFlow Integration**: Incorporates a neural network model (currently a mock implementation) to provide predictive signals, which are weighted into the overall strategy.
- **Backtesting Engine**: Simulates trading strategies against historical data to evaluate performance using key metrics like total return, Sharpe ratio, and max drawdown.
- **Data Acquisition**: Includes a Python script to fetch historical stock data from Yahoo Finance.
- **Modular Architecture**: Built with a clean, modular structure using CMake for easy maintenance and extension.

## Project Structure

```
trading-optimizer-cpp/
├── CMakeLists.txt
├── README.md
├── data/                # Stores downloaded .csv stock data
├── scripts/
│   └── get_data.py      # Python script to fetch data
├── src/
│   ├── main.cpp         # Main application entry point
│   ├── backtesting/     # Backtesting engine
│   ├── data/            # Data loading and handling
│   ├── ga/              # Genetic algorithm implementation
│   ├── nn/              # TensorFlow model interface
│   └── strategy/        # Trading strategy definition
└── tests/
    └── (Unit test files for each module)
```

## How to Use

### 1. Fetch Data

Use the provided Python script to download historical data for an Italian stock. The stock symbol must end with `.MI`.

```bash
cd scripts
python3 get_data.py ENEL.MI 2y
```

This will save a `ENEL.MI.csv` file in the `data/` directory.

### 2. Build the Project

The project uses CMake. It is recommended to build it in a clean environment (e.g., using Docker) to avoid dependency issues.

```bash
mkdir build
cd build
cmake ..
make
```

### 3. Run the Optimizer

Once built, run the optimizer from the `build` directory, passing the path to the data file.

```bash
./trading_optimizer ../data/ENEL.MI.csv
```

### Disclaimer

This is a proof-of-concept project and should not be used for real financial trading without extensive validation and understanding of the risks involved. The provided models and strategies are for educational and research purposes only.
