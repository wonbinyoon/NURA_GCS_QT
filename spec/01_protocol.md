# Rocket Telemetry Protocol

## 1. Frame Structure
**Frame size**: 25 bytes

| Byte | Field | Type |
| :--- | :--- | :--- |
| 0–1 | Sync {0xA5, 0xEC} | uint8[2] |
| 2–3 | Timestamp | uint16 |
| 4–9 | Position (NED) | int16[3] |
| 10–11 | Velocity Z | int16 |
| 12–13 | Acc Z (0.01 m/s²) | int16 |
| 14–21 | Quaternion (1/30000) | int16[4] |
| 22 | Bitfield (FSM, Sensor, Ejection) | uint8 |
| 23 | Bitfield (RX seq, TX seq) | uint8 |
| 24 | CRC8 | uint8 |

## 2. Timestamp Handling (MANDATORY)
- **Type**: 16-bit counter
- **Unit**: (2^16 / 168,000,000) s
- **Wrap-around**: Every 65536 ticks
- **Overflow Handling**:
    - `if (previous_timestamp - current_timestamp) > 32768 → overflow`
    - Threshold = half range (2^15)
    - Prevents false overflow due to packet loss
- **Extended Time**:
    - `extended_time = wrap_count * 65536 + current_timestamp`
    - Increment `wrap_count` on valid overflow only
    - All downstream systems must use extended timestamp
    - Must be deterministic and tested

## 3. Bitfield Definitions
### Byte 22
- `[0:2]` FSM
- `[3:5]` Sensor
- `[6:7]` Ejection

### Byte 23
- `[0:3]` RX seq
- `[4:7]` TX seq

## 4. Sequence Handling (MANDATORY)
- 4-bit sequence fields MUST be expanded to extended counters
- **Overflow Handling**:
    - `if (previous_seq - current_seq) > 8 → overflow`
    - Threshold = half range (2^3)
- **Maintain Extended Sequence Counters**:
    - `extended_rx = rx_wrap * 16 + current_rx`
    - `extended_tx = tx_wrap * 16 + current_tx`
    - `rx_wrap` and `tx_wrap` increment on valid overflow only
    - Must tolerate packet loss (threshold-based)

## 5. Extended Sequence (DataFrame Requirement)
- DataFrame MUST store extended sequence values (at least 16-bit recommended)
- Raw 4-bit values may be optionally stored for debugging
- Future protocol must support increased sequence width (≥8 bit recommended)
