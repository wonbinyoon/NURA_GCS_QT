#include "TelemetrySimulator.h"

TelemetrySimulator::TelemetrySimulator(uint32_t seed, double output_rate_hz)
    : current_seed_(seed), output_rate_hz_(output_rate_hz),
      timestamp_ticks_(0), exact_ticks_(0.0), rx_seq_(0), tx_seq_(0), rng_(seed) {
}

void TelemetrySimulator::setSeed(uint32_t seed) {
    current_seed_ = seed;
    rng_.seed(seed);
    // Reset state for deterministic behavior from seed
    timestamp_ticks_ = 0;
    exact_ticks_ = 0.0;
    rx_seq_ = 0;
    tx_seq_ = 0;
}

void TelemetrySimulator::setOutputRate(double hz) {
    if (hz > 0.0) {
        output_rate_hz_ = hz;
    }
}

double TelemetrySimulator::getOutputRate() const {
    return output_rate_hz_;
}

uint8_t TelemetrySimulator::calculateCRC8(const std::vector<uint8_t>& data, size_t length) const {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
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

std::vector<uint8_t> TelemetrySimulator::generateFrame() {
    std::vector<uint8_t> frame(25, 0);

    // Bytes 0-1: Sync
    frame[0] = 0xA5;
    frame[1] = 0xEC;

    // Bytes 2-3: Timestamp (16-bit)
    double ticks_per_sec = 168000000.0 / 65536.0;
    double ticks_per_frame = ticks_per_sec / output_rate_hz_;

    exact_ticks_ += ticks_per_frame;
    timestamp_ticks_ = static_cast<uint16_t>(exact_ticks_);

    // Little Endian packing
    frame[2] = timestamp_ticks_ & 0xFF;
    frame[3] = (timestamp_ticks_ >> 8) & 0xFF;

    // Bytes 4-9: Position NED (int16[3])
    std::uniform_int_distribution<int16_t> pos_dist(-1000, 1000);
    int16_t pos_n = pos_dist(rng_);
    int16_t pos_e = pos_dist(rng_);
    int16_t pos_d = pos_dist(rng_);

    frame[4] = pos_n & 0xFF;
    frame[5] = (pos_n >> 8) & 0xFF;
    frame[6] = pos_e & 0xFF;
    frame[7] = (pos_e >> 8) & 0xFF;
    frame[8] = pos_d & 0xFF;
    frame[9] = (pos_d >> 8) & 0xFF;

    // Bytes 10-11: Velocity Z (int16)
    int16_t vel_z = pos_dist(rng_);
    frame[10] = vel_z & 0xFF;
    frame[11] = (vel_z >> 8) & 0xFF;

    // Bytes 12-13: Accel Z (int16)
    int16_t acc_z = pos_dist(rng_);
    frame[12] = acc_z & 0xFF;
    frame[13] = (acc_z >> 8) & 0xFF;

    // Bytes 14-21: Quaternion (int16[4])
    std::uniform_int_distribution<int16_t> quat_dist(-30000, 30000);
    for (int i = 0; i < 4; ++i) {
        int16_t q = quat_dist(rng_);
        frame[14 + i*2] = q & 0xFF;
        frame[15 + i*2] = (q >> 8) & 0xFF;
    }

    // Byte 22: Bitfield (FSM, Sensor, Ejection)
    uint8_t fsm = rng_() % 8;
    uint8_t sensor = rng_() % 8;
    uint8_t ejection = rng_() % 4;
    frame[22] = (fsm & 0x07) | ((sensor & 0x07) << 3) | ((ejection & 0x03) << 6);

    // Byte 23: Bitfield (RX seq, TX seq)
    // Both are 4-bit, [0:3] RX, [4:7] TX
    frame[23] = (rx_seq_ & 0x0F) | ((tx_seq_ & 0x0F) << 4);

    rx_seq_ = (rx_seq_ + 1) & 0x0F;
    tx_seq_ = (tx_seq_ + 1) & 0x0F;

    // Byte 24: CRC8
    frame[24] = calculateCRC8(frame, 24);

    return frame;
}
