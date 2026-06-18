#ifndef REPLAYSTORAGE_H
#define REPLAYSTORAGE_H

#include <string>
#include <cstdint>
#include <cstddef>
#include "../model/DataFrame.h"
#include "../logger/DataLogger.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

class ReplayStorage {
public:
    ReplayStorage();
    ~ReplayStorage();

    bool open(const std::string& filename);
    void close();

    bool isOpen() const;
    uint64_t getFrameCount() const;

    // Zero-copy access in O(1)
    const DataFrame* getFrame(uint64_t index) const;

    // Binary search for timestamp
    const DataFrame* seekByTimestamp(uint32_t timestamp) const;

private:
#ifdef _WIN32
    HANDLE file_handle_;
    HANDLE mapping_handle_;
    const void* mapped_data_;
#else
    int fd_;
    void* mapped_data_;
#endif
    size_t mapped_size_;

    const FileHeader* header_;
    const DataFrame* frames_;
};

#endif // REPLAYSTORAGE_H
