# Backend API Specification

This document defines the public interfaces for integrating the backend telemetry processing modules. It strictly adheres to the definitions of `DataFrame`, the telemetry protocol, and the backend architecture.

---

## Data Flow Pipeline

The standard flow of data through the backend system follows this pipeline:

**Simulator → Parser → Logger → Replay**

*   **Simulator Output**: `std::vector<uint8_t>` (passed by value to Parser).
*   **Parser Output**: `std::vector<DataFrame>` (passed by value, managed by UI/Model).
*   **Logger Input**: `const DataFrame&` (passed by const reference, writes continuously).
*   **Replay Output**: `const DataFrame*` (zero-copy pointer directly into `mmap` region, managed by `ReplayStorage`).

---

# TelemetrySimulator

## Responsibility
Generates deterministic, protocol-compliant binary telemetry byte streams simulating hardware serial input. It provides reliable and reproducible test data without requiring physical hardware.

## Public API
```cpp
class TelemetrySimulator {
public:
    TelemetrySimulator(uint32_t seed = 42, double output_rate_hz = 50.0);
    std::vector<uint8_t> generateFrame();
    void setSeed(uint32_t seed);
    void setOutputRate(double hz);
    double getOutputRate() const;
};
```

## Threading
*   Not inherently thread-safe.
*   Should be run on a dedicated background or simulation thread.
*   Generating a frame must not block the main thread.

## Notes
*   Stateful deterministic RNG relies on `setSeed` to reset monotonic variables.
*   Output bytes are exactly matched to the 25-byte protocol specification.

---

# IProtocolParser

## Responsibility
Abstract interface defining the contract for converting raw binary streams into valid `DataFrame` structs. This ensures decoupling between byte ingestion logic and specific protocol logic.

## Public API
```cpp
class IProtocolParser {
public:
    virtual ~IProtocolParser() = default;
    virtual std::vector<DataFrame> parse(const std::vector<uint8_t>& data) = 0;
};
```

## Threading
*   Expected to be accessed by the thread owning the serial or simulation source.
*   Thread-agnostic interface, implementation dependent.

## Notes
*   Parsers own their own internal buffering and state tracking.

---

# RocketTelemetryParser

## Responsibility
Concrete implementation of `IProtocolParser` tailored specifically to the 25-byte Rocket Telemetry Protocol. Responsible for extracting values, computing CRC-8, managing overflow behavior thresholds, and scaling raw integer units into standard float values.

## Public API
```cpp
class RocketTelemetryParser : public IProtocolParser {
public:
    RocketTelemetryParser();
    ~RocketTelemetryParser() override = default;
    std::vector<DataFrame> parse(const std::vector<uint8_t>& data) override;
};
```

## Threading
*   Must be instantiated and utilized by a single processing thread.
*   Requires external synchronization if `parse` is called from multiple threads.

## Notes
*   Safely buffers incomplete chunks and silently drops corrupted CRC packets.
*   Outputs fully expanded and monotonic timestamps/sequences into the returned `DataFrame`.

---

# DataLogger

## Responsibility
Writes sequential streams of `DataFrame` structs into binary disk logs. Supports robust append-only logging behavior alongside metadata tracking via `FileHeader`.

## Public API
```cpp
class DataLogger {
public:
    DataLogger();
    ~DataLogger();

    bool open(const std::string& filename);
    void close();
    bool logFrame(const DataFrame& frame);

    bool isOpen() const;
    uint64_t getFrameCount() const;
};
```

## Threading
*   File I/O operations are inherently blocking.
*   Must be executed within a background logging thread to avoid main UI thread latency.
*   Callers should queue `DataFrame` elements safely before transferring them to the logging thread.

## Notes
*   Writes are completely raw; no data conversion occurs within the logger.

---

# ReplayStorage

## Responsibility
Provides O(1), zero-copy access to previously recorded binary log files by leveraging POSIX memory mapping (`mmap`). Facilitates instant binary searching over continuous `DataFrame` arrays.

## Public API
```cpp
class ReplayStorage {
public:
    ReplayStorage();
    ~ReplayStorage();

    bool open(const std::string& filename);
    void close();

    bool isOpen() const;
    uint64_t getFrameCount() const;

    const DataFrame* getFrame(uint64_t index) const;
    const DataFrame* seekByTimestamp(uint32_t timestamp) const;
};
```

## Threading
*   Read-only operations (`getFrame`, `seekByTimestamp`) are thread-safe and lock-free across multiple threads after successful `open()`.
*   `open()` and `close()` must not overlap with active reads.

## Notes
*   Ownership of the `const DataFrame*` array remains fully managed by `ReplayStorage` lifecycle.
*   If a pointer is extracted, the caller must not access it if the storage is closed.

---

# Integration Example (Pseudo-code)

```cpp
// 1. Setup Backend Modules
TelemetrySimulator simulator(42, 50.0);
RocketTelemetryParser parser;
DataLogger logger;

if (!logger.open("flight_log.bin")) {
    handleError("Cannot start logger");
}

// 2. Data Pipeline Execution (Running in Background Thread)
while (isRunning) {
    // Generate raw byte stream from simulator (or read from QSerialPort)
    std::vector<uint8_t> raw_bytes = simulator.generateFrame();

    // Process arbitrary streams into complete frames
    std::vector<DataFrame> decoded_frames = parser.parse(raw_bytes);

    // Log outputs and process for UI
    for (const DataFrame& frame : decoded_frames) {
        logger.logFrame(frame);

        // Dispatch copy to UI or analysis models safely
        uiModelQueue.push(frame);
    }
}

// Finalize writes and log headers correctly
logger.close();

// 3. Replay Flow (Analysis / Charting)
ReplayStorage replay;
if (replay.open("flight_log.bin")) {
    // Fast O(1) zero copy
    const DataFrame* early_frame = replay.getFrame(10);

    // Find frame dynamically by timestamp target using binary search
    const DataFrame* specific_frame = replay.seekByTimestamp(65536 + 100);
}
```
