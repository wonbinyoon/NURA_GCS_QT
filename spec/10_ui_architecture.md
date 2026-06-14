# UI Architecture

This document defines the UI architecture, modules, data flow, and execution rules for the frontend layer.

## Goal
- Visualize telemetry data in real-time and replay
- Provide control over Serial and Replay
- Maintain strict separation from backend logic
- Ensure deterministic, high-performance rendering

## 1. Architecture Overview

```
[Serial / Simulator]
        ↓
      Parser
        ↓
     DataFrame
       /   \
   Logger   UI
```

### Rules
- UI MUST NOT access raw bytes
- UI MUST consume DataFrame only
- UI MUST NOT perform parsing or scaling
- All updates MUST be asynchronous

## 2. Top-Level Layout

```
+--------------------------------------------------+
| Menu / Toolbar                                   |
+----------------------+---------------------------+
| Control Panel        | Visualization Area        |
|                      |                           |
|                      |                           |
+----------------------+---------------------------+
| Status / Console                                 |
+--------------------------------------------------+
```

### Rules
- Control panel MUST include Serial and Replay controls
- Visualization area MUST host graphs, 3D view, and map
- Console MUST display logs and system events

## 3. Secondary Modules

### 3.1 StatusPanel
- **Displays**: FSM state, Altitude, Velocity, Acceleration, GPS
- **Rules**:
    - MUST update on every UI frame
    - MUST highlight abnormal states
    - MUST be low-latency

### 3.2 ConsolePanel
- **Features**: System logs, Parser logs
- **Rules**:
    - MUST NOT block UI thread
    - MUST support scrolling and buffering

## 4. Operational Rules

### 4.1 Data Handling Rules
- UI MUST receive DataFrame only
- No transformation or parsing in UI layer
- Data MUST be buffered in thread-safe queue

### 4.2 Threading Model
```
Worker Thread (Parser / Replay)
        ↓
   Thread-safe Queue
        ↓
UI Thread (Qt)
```
- **Rules**:
    - MUST use `Qt::QueuedConnection`
    - UI thread MUST NOT perform heavy computation

### 4.3 UI Update Rules (CRITICAL)
- UI MUST be driven by timer (NOT data arrival)
- Update interval: 16–33 ms (30–60 FPS)
- Latest DataFrame MUST be used per frame
- Intermediate frames MAY be dropped
- Rendering MUST NOT depend on data rate

### 4.4 Performance Rules
- UI MUST sustain ≥30 FPS
- Plot updates MUST be decoupled from input rate
- Buffer size MUST be bounded

### 4.5 Visual Feedback Rules
- Connection status MUST be visible
- Errors MUST be clearly indicated
- Critical values MUST be highlighted

### 4.6 Data Integrity Rules
- UI MUST tolerate missing data
- UI MUST NOT crash on invalid DataFrame
- Invalid values MUST be ignored or flagged

## 5. Future Extensions
- Sensor dashboards
- Advanced filtering
- Multi-device support

## 6. Definition of Done
- Real-time visualization functional
- Replay functional with seeking
- UI runs smoothly at ≥30 FPS
- No UI freezes or blocking
- Data displayed correctly and consistently
