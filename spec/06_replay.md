# Replay

## 1. Rules
- Replay MUST use memory mapping (mmap or equivalent).
- No data copy allowed during replay.
- Frame access MUST be O(1).
- Timestamp MUST be monotonic (extended timestamp).
- Sequence MUST be monotonic (extended sequence).

## 2. Seek Implementation
Seek by timestamp MUST be implemented via:
- Binary search on DataFrame array **OR**
- Optional index table

## 3. Optional Index Table
- If present, maps `timestamp` → `frame index`.
- Stored after DataFrame region.
