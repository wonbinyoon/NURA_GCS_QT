# System Rules

## 1. Project Goal
Receive binary data via Serial, then:
- Log to file
- Visualize in real-time
- Support replay and analysis

## 2. Technology
- **Language**: C++17+
- **Framework**: Qt 6, QSerialPort
- **Build System**: CMake
- **Storage**:
    - Binary logging (parsed data format, REQUIRED)
    - Memory-mapped replay (mmap or equivalent)
- **Plotting**: Qt Charts or QCustomPlot

## 3. Architecture
- `parser/`
- `model/`
- `serial/`
- `simulator/`
- `logger/`
- `ui/`

## 4. Testing
- Use GoogleTest
- Loop: implement → test → fix → pass
- Deterministic input required
- No UI/Serial dependency

## 5. Execution Rules
- One module at a time
- Must compile
- Tests must pass

## 6. Done Criteria
- Works without hardware
- Parser fully tested
- Deterministic simulation
- No memory growth
- ≥30 FPS visualization
