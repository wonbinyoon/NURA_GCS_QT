# Simulator

## 1. Requirements
- Generate valid frames.
- Follow protocol exactly.
- Configurable rate.
- Flight sequence support.

## 2. Determinism
- Fixed seed required.
- Same seed → same output.

## 3. Integration
Integration pipeline:
`[Simulator / Serial]` → `[Parser]` → `[Model]` → `[UI / Logger]`
- Use the same pipeline only.
