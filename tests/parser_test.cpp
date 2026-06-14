#include <gtest/gtest.h>
#include "../parser/RocketTelemetryParser.h"
#include <vector>

// Helper to calculate CRC8 for testing
uint8_t calculateCRC8(const std::vector<uint8_t>& data, size_t length) {
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

// Helper to construct a valid base frame
std::vector<uint8_t> createBaseFrame() {
    std::vector<uint8_t> f(25, 0);
    f[0] = 0xA5;
    f[1] = 0xEC;

    // Default values
    f[2] = 0; f[3] = 0; // ts
    // pos
    f[4] = 0; f[5] = 0;
    f[6] = 0; f[7] = 0;
    f[8] = 0; f[9] = 0;
    // vel z
    f[10] = 0; f[11] = 0;
    // acc z
    f[12] = 0; f[13] = 0;
    // quat
    f[14] = 0; f[15] = 0;
    f[16] = 0; f[17] = 0;
    f[18] = 0; f[19] = 0;
    f[20] = 0; f[21] = 0;
    // bitfields
    f[22] = 0;
    f[23] = 0;

    f[24] = calculateCRC8(f, 24);
    return f;
}

TEST(ParserTest, CorrectFrameParsing) {
    RocketTelemetryParser parser;
    std::vector<uint8_t> f = createBaseFrame();

    // Modify some values to verify conversions
    f[2] = 100 & 0xFF; f[3] = (100 >> 8) & 0xFF; // ts = 100

    int16_t acc_z = 981; // 9.81 m/s^2
    f[12] = acc_z & 0xFF; f[13] = (acc_z >> 8) & 0xFF;

    int16_t q0 = 30000; // 1.0f
    int16_t q1 = 15000; // 0.5f
    f[14] = q0 & 0xFF; f[15] = (q0 >> 8) & 0xFF;
    f[16] = q1 & 0xFF; f[17] = (q1 >> 8) & 0xFF;

    // bitfield 22: FSM=2, Sensor=5, Ejection=1
    f[22] = (2 & 0x07) | ((5 & 0x07) << 3) | ((1 & 0x03) << 6);

    f[24] = calculateCRC8(f, 24);

    auto frames = parser.parse(f);
    ASSERT_EQ(frames.size(), 1);

    EXPECT_EQ(frames[0].timestamp_raw, 100);
    EXPECT_FLOAT_EQ(frames[0].acceleration[2], 9.81f);
    EXPECT_FLOAT_EQ(frames[0].quat[0], 1.0f);
    EXPECT_FLOAT_EQ(frames[0].quat[1], 0.5f);

    EXPECT_EQ(frames[0].fsm_state, 2);
    EXPECT_EQ(frames[0].sensor_state, 5);
    EXPECT_EQ(frames[0].ejection_type, 1);
}

TEST(ParserTest, TimestampOverflow) {
    RocketTelemetryParser parser;

    std::vector<uint8_t> f1 = createBaseFrame();
    f1[2] = 65000 & 0xFF; f1[3] = (65000 >> 8) & 0xFF;
    f1[24] = calculateCRC8(f1, 24);

    auto r1 = parser.parse(f1);
    ASSERT_EQ(r1.size(), 1);
    EXPECT_EQ(r1[0].timestamp_ext, 65000);

    std::vector<uint8_t> f2 = createBaseFrame();
    f2[2] = 100 & 0xFF; f2[3] = (100 >> 8) & 0xFF;
    f2[24] = calculateCRC8(f2, 24);

    auto r2 = parser.parse(f2);
    ASSERT_EQ(r2.size(), 1);
    EXPECT_EQ(r2[0].timestamp_ext, 65536 + 100);
}

TEST(ParserTest, SequenceOverflow) {
    RocketTelemetryParser parser;

    std::vector<uint8_t> f1 = createBaseFrame();
    // rx = 14, tx = 15
    f1[23] = (14 & 0x0F) | ((15 & 0x0F) << 4);
    f1[24] = calculateCRC8(f1, 24);

    auto r1 = parser.parse(f1);
    ASSERT_EQ(r1.size(), 1);
    EXPECT_EQ(r1[0].rx_seq_ext, 14);
    EXPECT_EQ(r1[0].tx_seq_ext, 15);

    std::vector<uint8_t> f2 = createBaseFrame();
    // rx = 2, tx = 1
    f2[23] = (2 & 0x0F) | ((1 & 0x0F) << 4);
    f2[24] = calculateCRC8(f2, 24);

    auto r2 = parser.parse(f2);
    ASSERT_EQ(r2.size(), 1);
    EXPECT_EQ(r2[0].rx_seq_ext, 16 + 2);
    EXPECT_EQ(r2[0].tx_seq_ext, 16 + 1);
}

TEST(ParserTest, MalformedInput) {
    RocketTelemetryParser parser;

    // Corrupt CRC
    std::vector<uint8_t> f1 = createBaseFrame();
    f1[24] ^= 0x01;
    auto r1 = parser.parse(f1);
    EXPECT_EQ(r1.size(), 0);

    // Bad sync bytes then good frame
    std::vector<uint8_t> stream = {0x00, 0xA5, 0x01, 0xEC};
    std::vector<uint8_t> good_f = createBaseFrame();
    stream.insert(stream.end(), good_f.begin(), good_f.end());

    auto r2 = parser.parse(stream);
    EXPECT_EQ(r2.size(), 1);
}

TEST(ParserTest, PartialFrames) {
    RocketTelemetryParser parser;
    std::vector<uint8_t> f = createBaseFrame();

    std::vector<uint8_t> part1(f.begin(), f.begin() + 10);
    std::vector<uint8_t> part2(f.begin() + 10, f.end());

    auto r1 = parser.parse(part1);
    EXPECT_EQ(r1.size(), 0); // Not enough data

    auto r2 = parser.parse(part2);
    EXPECT_EQ(r2.size(), 1); // Now completes the frame
}
