#include "RocketTelemetryParser.h"
#include <cstring>

RocketTelemetryParser::RocketTelemetryParser()
    : has_prev_timestamp_(false), prev_timestamp_(0), timestamp_wrap_count_(0),
      has_prev_rx_seq_(false), prev_rx_seq_(0), rx_seq_wrap_count_(0),
      has_prev_tx_seq_(false), prev_tx_seq_(0), tx_seq_wrap_count_(0) {
}

uint8_t RocketTelemetryParser::calculateCRC8(const std::vector<uint8_t>& data, size_t offset, size_t length) const {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[offset + i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

std::vector<DataFrame> RocketTelemetryParser::parse(const std::vector<uint8_t>& data) {
    std::vector<DataFrame> parsed_frames;

    // Append new data to buffer
    buffer_.insert(buffer_.end(), data.begin(), data.end());

    while (buffer_.size() >= 25) {
        // Search for sync bytes
        if (buffer_[0] == 0xA5 && buffer_[1] == 0xEC) {
            // Found sync, check CRC
            std::vector<uint8_t> frame_data(buffer_.begin(), buffer_.begin() + 25);
            uint8_t expected_crc = calculateCRC8(frame_data, 0, 24);
            uint8_t actual_crc = buffer_[24];

            if (expected_crc == actual_crc) {
                // Valid frame
                parsed_frames.push_back(decodeFrame(frame_data, 0));

                // Consume the 25 bytes
                buffer_.erase(buffer_.begin(), buffer_.begin() + 25);
            } else {
                // CRC mismatch, drop the first sync byte and continue searching
                buffer_.pop_front();
            }
        } else {
            // Not a sync byte, pop
            buffer_.pop_front();
        }
    }

    return parsed_frames;
}

DataFrame RocketTelemetryParser::decodeFrame(const std::vector<uint8_t>& frame_data, size_t offset) {
    DataFrame df;
    std::memset(&df, 0, sizeof(DataFrame));

    // 2-3 Timestamp
    uint16_t current_timestamp = frame_data[offset + 2] | (static_cast<uint16_t>(frame_data[offset + 3]) << 8);
    df.timestamp_raw = current_timestamp;

    if (has_prev_timestamp_) {
        // Overflow threshold logic: prev - current > 32768
        // Note: use int32_t to avoid underflow bugs in subtraction
        int32_t diff = static_cast<int32_t>(prev_timestamp_) - static_cast<int32_t>(current_timestamp);
        if (diff > 32768) {
            timestamp_wrap_count_++;
        }
    } else {
        has_prev_timestamp_ = true;
    }
    prev_timestamp_ = current_timestamp;
    df.timestamp_ext = (timestamp_wrap_count_ * 65536) + current_timestamp;

    // 4-9 Position NED
    int16_t pos_n = frame_data[offset + 4] | (static_cast<int16_t>(frame_data[offset + 5]) << 8);
    int16_t pos_e = frame_data[offset + 6] | (static_cast<int16_t>(frame_data[offset + 7]) << 8);
    int16_t pos_d = frame_data[offset + 8] | (static_cast<int16_t>(frame_data[offset + 9]) << 8);
    df.position_ned[0] = static_cast<float>(pos_n);
    df.position_ned[1] = static_cast<float>(pos_e);
    df.position_ned[2] = static_cast<float>(pos_d);

    // 10-11 Velocity Z
    int16_t vel_z = frame_data[offset + 10] | (static_cast<int16_t>(frame_data[offset + 11]) << 8);
    df.velocity_ned[0] = 0.0f; // Not provided in spec
    df.velocity_ned[1] = 0.0f; // Not provided
    df.velocity_ned[2] = static_cast<float>(vel_z);

    // 12-13 Acc Z
    int16_t acc_z = frame_data[offset + 12] | (static_cast<int16_t>(frame_data[offset + 13]) << 8);
    df.acceleration[0] = 0.0f;
    df.acceleration[1] = 0.0f;
    df.acceleration[2] = static_cast<float>(acc_z) * 0.01f;

    // 14-21 Quaternion
    for (int i = 0; i < 4; ++i) {
        int16_t q_raw = frame_data[offset + 14 + i*2] | (static_cast<int16_t>(frame_data[offset + 15 + i*2]) << 8);
        df.quat[i] = static_cast<float>(q_raw) / 30000.0f;
    }

    // 22 Bitfield
    uint8_t byte22 = frame_data[offset + 22];
    df.fsm_state = byte22 & 0x07;
    df.sensor_state = (byte22 >> 3) & 0x07;
    df.ejection_type = (byte22 >> 6) & 0x03;

    // 23 Bitfield (RX/TX seq)
    uint8_t byte23 = frame_data[offset + 23];
    uint8_t current_rx_seq = byte23 & 0x0F;
    uint8_t current_tx_seq = (byte23 >> 4) & 0x0F;

    df.rx_seq_raw = current_rx_seq;
    df.tx_seq_raw = current_tx_seq;

    // Rx Overflow logic
    if (has_prev_rx_seq_) {
        int32_t diff = static_cast<int32_t>(prev_rx_seq_) - static_cast<int32_t>(current_rx_seq);
        if (diff > 8) {
            rx_seq_wrap_count_++;
        }
    } else {
        has_prev_rx_seq_ = true;
    }
    prev_rx_seq_ = current_rx_seq;
    df.rx_seq_ext = (rx_seq_wrap_count_ * 16) + current_rx_seq;

    // Tx Overflow logic
    if (has_prev_tx_seq_) {
        int32_t diff = static_cast<int32_t>(prev_tx_seq_) - static_cast<int32_t>(current_tx_seq);
        if (diff > 8) {
            tx_seq_wrap_count_++;
        }
    } else {
        has_prev_tx_seq_ = true;
    }
    prev_tx_seq_ = current_tx_seq;
    df.tx_seq_ext = (tx_seq_wrap_count_ * 16) + current_tx_seq;

    return df;
}
