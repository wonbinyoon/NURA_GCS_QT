# Parser Rules

## 1. Isolation
- Fully isolated in `parser/` directory.

## 2. Dependencies
- Depend only on `IProtocolParser`.
- No external dependency on concrete parser.

## 3. State Management
- Stateless or explicitly documented state.
- No global/static state.

## 4. Output
- Output MUST be `DataFrame`.
