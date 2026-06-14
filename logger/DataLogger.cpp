#include "DataLogger.h"
#include <cstring>
#include <iostream>

DataLogger::DataLogger() : is_open_(false) {
    std::memset(&header_, 0, sizeof(FileHeader));
}

DataLogger::~DataLogger() {
    close();
}

bool DataLogger::open(const std::string& filename) {
    if (is_open_) {
        close();
    }

    // Try to open existing file first for appending
    file_.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    if (file_.is_open()) {
        // Read header
        file_.seekg(0, std::ios::beg);
        file_.read(reinterpret_cast<char*>(&header_), sizeof(FileHeader));

        if (header_.magic != LOG_MAGIC || header_.frame_size != sizeof(DataFrame)) {
            // Invalid file or version mismatch, close and fail
            file_.close();
            return false;
        }

        // Move to the end of data section to append new frames
        // Assuming append only happens sequentially at the end of frames
        file_.seekp(header_.data_offset + (header_.frame_count * header_.frame_size), std::ios::beg);
        is_open_ = true;
        return true;
    }

    // File doesn't exist or couldn't be opened, create a new one
    file_.clear();
    file_.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file_.is_open()) {
        return false;
    }

    // Initialize header for new file
    header_.magic = LOG_MAGIC;
    header_.version = 1;
    header_.frame_size = sizeof(DataFrame);
    header_.frame_count = 0;
    header_.data_offset = sizeof(FileHeader);
    header_.index_offset = 0; // Not used for now

    // Write header
    file_.seekp(0, std::ios::beg);
    file_.write(reinterpret_cast<const char*>(&header_), sizeof(FileHeader));

    // Close and reopen in in/out mode to allow both reading/updating header and appending data
    file_.close();
    file_.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_.is_open()) {
        return false;
    }

    file_.seekp(header_.data_offset, std::ios::beg);
    is_open_ = true;
    return true;
}

void DataLogger::close() {
    if (is_open_ && file_.is_open()) {
        // Finalize header by updating frame_count
        file_.seekp(0, std::ios::beg);
        file_.write(reinterpret_cast<const char*>(&header_), sizeof(FileHeader));
        file_.close();
    }
    is_open_ = false;
}

bool DataLogger::logFrame(const DataFrame& frame) {
    if (!is_open_) {
        return false;
    }

    // Write frame at current position
    file_.write(reinterpret_cast<const char*>(&frame), sizeof(DataFrame));

    if (file_.good()) {
        header_.frame_count++;
        return true;
    }

    return false;
}

bool DataLogger::isOpen() const {
    return is_open_;
}

uint64_t DataLogger::getFrameCount() const {
    return header_.frame_count;
}
