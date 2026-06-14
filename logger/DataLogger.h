#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <fstream>
#include <cstdint>
#include "../model/DataFrame.h"

// Defined magic bytes "TLM\0"
constexpr uint32_t LOG_MAGIC = 0x004D4C54; // 'T'=0x54, 'L'=0x4C, 'M'=0x4D, '\0'=0x00 in Little Endian

#pragma pack(push, 1)
struct FileHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t frame_size;
    uint64_t frame_count;
    uint64_t data_offset;
    uint64_t index_offset;
};
#pragma pack(pop)

class DataLogger {
public:
    DataLogger();
    ~DataLogger();

    bool open(const std::string& filename);
    void close();

    bool logFrame(const DataFrame& frame);

    bool isOpen() const;
    uint64_t getFrameCount() const;

private:
    std::fstream file_;
    FileHeader header_;
    bool is_open_;
};

#endif // DATALOGGER_H
