#ifndef TELEMETRYSIMULATOR_H
#define TELEMETRYSIMULATOR_H

#include <vector>
#include <cstdint>
#include <random>

class TelemetrySimulator {
public:
    TelemetrySimulator(uint32_t seed = 42, double output_rate_hz = 50.0);

    // Generates a single 25-byte frame
    std::vector<uint8_t> generateFrame();

    // Configuration
    void setSeed(uint32_t seed);
    void setOutputRate(double hz);
    double getOutputRate() const;

private:
    uint32_t current_seed_;
    double output_rate_hz_;

    // State
    uint16_t timestamp_ticks_;
    double exact_ticks_;
    uint8_t rx_seq_;
    uint8_t tx_seq_;

    std::mt19937 rng_;

    uint8_t calculateCRC8(const std::vector<uint8_t>& data, size_t length) const;
};

#endif // TELEMETRYSIMULATOR_H
