#include "DataHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace trading {

DataHandler::DataHandler() {}

DataHandler::~DataHandler() {}

bool DataHandler::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    candles_.clear();
    std::string line;
    
    // Skip header line
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        Candle candle;
        if (parseLine(line, candle)) {
            candles_.push_back(candle);
        }
    }
    
    file.close();
    
    std::cout << "Loaded " << candles_.size() << " candles from " << filename << std::endl;
    return !candles_.empty();
}

const std::vector<Candle>& DataHandler::getCandles() const {
    return candles_;
}

std::vector<Candle> DataHandler::getCandlesInRange(size_t start_idx, size_t end_idx) const {
    if (start_idx >= candles_.size() || end_idx > candles_.size() || start_idx >= end_idx) {
        return std::vector<Candle>();
    }
    
    return std::vector<Candle>(candles_.begin() + start_idx, candles_.begin() + end_idx);
}

size_t DataHandler::size() const {
    return candles_.size();
}

void DataHandler::clear() {
    candles_.clear();
}

double DataHandler::getClosePrice(size_t idx) const {
    if (idx >= candles_.size()) {
        return 0.0;
    }
    return candles_[idx].close;
}

bool DataHandler::parseLine(const std::string& line, Candle& candle) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    
    // Expected format: timestamp,open,high,low,close,volume
    if (tokens.size() < 6) {
        return false;
    }
    
    try {
        candle.timestamp = std::stol(tokens[0]);
        candle.open = std::stod(tokens[1]);
        candle.high = std::stod(tokens[2]);
        candle.low = std::stod(tokens[3]);
        candle.close = std::stod(tokens[4]);
        candle.volume = std::stol(tokens[5]);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing line: " << line << " - " << e.what() << std::endl;
        return false;
    }
}

} // namespace trading
