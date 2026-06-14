#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <cstdint>
#include <type_traits>

// Layout Version: 1.0

struct alignas(16) DataFrame {
  uint32_t timestamp_ext;
  uint32_t reserved0;

  uint16_t timestamp_raw;
  uint8_t fsm_state;
  uint8_t sensor_state;
  uint8_t ejection_type;

  // Extended sequence (monotonic)
  uint32_t rx_seq_ext;
  uint32_t tx_seq_ext;

  // Raw sequence (for debugging)
  uint8_t rx_seq_raw;
  uint8_t tx_seq_raw;
  uint8_t reserved1[2];

  float position_ned[3];
  float velocity_ned[3];
  float acceleration[3];

  float gyro[3];
  float quat[4];

  double latitude;
  double longitude;
  float altitude;
  float reserved2;

  float mag[3];
  float pressure;
  float temperature;
  float reserved3;

  uint8_t padding[16];
};

static_assert(sizeof(DataFrame) == 160, "DataFrame size MUST be exactly 160 bytes");
static_assert(std::is_standard_layout<DataFrame>::value && std::is_trivial<DataFrame>::value, "DataFrame MUST be a POD type");

#endif // DATAFRAME_H
