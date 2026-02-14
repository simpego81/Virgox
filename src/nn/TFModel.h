#ifndef TF_MODEL_H
#define TF_MODEL_H

#include <string>
#include <vector>

namespace trading {

// Simplified TensorFlow model interface
// In a real implementation, this would use TensorFlow C++ API
// For this demo, we'll implement a simple mock predictor
class TFModel {
public:
    TFModel();
    ~TFModel();
    
    // Load a pre-trained model from file
    bool loadModel(const std::string& model_path);
    
    // Make prediction on input features
    // Returns a value between -1.0 (strong sell) and 1.0 (strong buy)
    double predict(const std::vector<double>& features);
    
    // Check if model is loaded
    bool isLoaded() const;
    
private:
    bool is_loaded_;
    
    // Simple moving average prediction (mock implementation)
    double simplePrediction(const std::vector<double>& features);
};

} // namespace trading

#endif // TF_MODEL_H
