# Data Model (DataFrame)

## 1. DataFrame Structure
```cpp
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
};
```

## 2. Rules
- **Size**: MUST be exactly 160 bytes
- **Layout**: Fixed layout (ABI stable)
- **Versioning**: Layout versioning REQUIRED (file header must include version)
- **Changes**: Any change to DataFrame requires version bump
- **Units**: All units must be physical (no scaling at usage stage)
- **Conversion**: Parser MUST convert raw → float once
- **Monotonicity**: Extended timestamp and sequences MUST be monotonic
- **Memory Safety**: DataFrame layout MUST be mmap-safe (no pointers, no dynamic data)
- **Alignment**: Alignment MUST be 16-byte (`alignas(16)`)
