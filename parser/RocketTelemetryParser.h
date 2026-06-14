#ifndef ROCKETTELEMETRYPARSER_H
#define ROCKETTELEMETRYPARSER_H

#include "IProtocolParser.h"
#include <deque>

class RocketTelemetryParser : public IProtocolParser {
public:
    RocketTelemetryParser();
    ~RocketTelemetryParser() override = default;

    std::vector<DataFrame> parse(const std::vector<uint8_t>& data) override;

private:
    uint8_t calculateCRC8(const std::vector<uint8_t>& data, size_t offset, size_t length) const;
    DataFrame decodeFrame(const std::vector<uint8_t>& frame_data, size_t offset);

    std::deque<uint8_t> buffer_;

    // State for timestamp tracking
    bool has_prev_timestamp_;
    uint16_t prev_timestamp_;
    uint32_t timestamp_wrap_count_;

    // State for sequence tracking
    bool has_prev_rx_seq_;
    uint8_t prev_rx_seq_;
    uint32_t rx_seq_wrap_count_;

    bool has_prev_tx_seq_;
    uint8_t prev_tx_seq_;
    uint32_t tx_seq_wrap_count_;
};

#endif // ROCKETTELEMETRYPARSER_H
