#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <string>
#include <vector>
#include <map>

namespace trading {

struct Candle {
    long timestamp;
    double open;
    double high;
    double low;
    double close;
    long volume;
    
    Candle() : timestamp(0), open(0.0), high(0.0), low(0.0), close(0.0), volume(0) {}
    
    Candle(long ts, double o, double h, double l, double c, long v)
        : timestamp(ts), open(o), high(h), low(l), close(c), volume(v) {}
};

class DataHandler {
public:
    DataHandler();
    ~DataHandler();
    
    // Load data from CSV file
    bool loadFromCSV(const std::string& filename);
    
    // Get all candles
    const std::vector<Candle>& getCandles() const;
    
    // Get candles in a specific time range
    std::vector<Candle> getCandlesInRange(size_t start_idx, size_t end_idx) const;
    
    // Get the number of candles
    size_t size() const;
    
    // Clear all data
    void clear();
    
    // Get price at specific index
    double getClosePrice(size_t idx) const;
    
private:
    std::vector<Candle> candles_;
    
    // Helper function to parse a CSV line
    bool parseLine(const std::string& line, Candle& candle);
};

} // namespace trading

#endif // DATA_HANDLER_H
