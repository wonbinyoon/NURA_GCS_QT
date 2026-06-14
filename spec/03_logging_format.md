# Logging Format

## 1. File Structure
File MUST be structured as:
1. `[FileHeader]`
2. `[DataFrame x N]`
3. `[Optional Index]`

## 2. FileHeader Requirements
FileHeader MUST include:
- Magic identifier
- Version
- `frame_size` (160 bytes)
- `frame_count`
- `data_offset`
- `optional index_offset`

## 3. Storage Rules
- FileHeader MUST be placed at the beginning of file
- DataFrame region MUST be contiguous
- Access MUST use: `offset = data_offset + index * 160`
- Logging MUST be append-only
- Header fields (count, timestamps) MUST be finalized on close

## 4. Data Integrity Rules
- DataFrame MUST be self-contained
- No pointer or dynamic memory allowed
- Corrupted frames MUST NOT crash system
- CRC MAY be validated during replay (optional)
