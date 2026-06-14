# AppController

## Responsibility
- Central coordinator for all UI updates
- Receives DataFrame from backend
- Routes data to UI components
- Manages mode (LIVE vs REPLAY)

## Rules
- MUST be the only module connecting backend and UI
- MUST use thread-safe queue
- MUST NOT block UI thread

## Data Flow Contract (MANDATORY)
`DataFrame` →
- PlotPanel
- 3DView
- OrientationPanel
- MapView
- StatusPanel

- `DataFrame` MUST be passed as `const&`
- No ownership transfer allowed

## Mode Switching Rules
- **Modes**: LIVE (Serial / Simulator), REPLAY
- **Rules**:
    - ONLY one mode active at a time
    - Switching MUST stop previous mode
    - All buffers MUST be cleared
- **Constraints**:
    - Replay MUST NOT run during Serial connection
    - Serial MUST NOT connect during replay
