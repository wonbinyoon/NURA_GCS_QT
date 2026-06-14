#include "ReplayStorage.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

ReplayStorage::ReplayStorage()
    : fd_(-1), mapped_data_(MAP_FAILED), mapped_size_(0), header_(nullptr), frames_(nullptr) {
}

ReplayStorage::~ReplayStorage() {
    close();
}

bool ReplayStorage::open(const std::string& filename) {
    if (isOpen()) {
        close();
    }

    fd_ = ::open(filename.c_str(), O_RDONLY);
    if (fd_ < 0) {
        return false;
    }

    struct stat sb;
    if (fstat(fd_, &sb) == -1) {
        close();
        return false;
    }

    mapped_size_ = sb.st_size;
    if (mapped_size_ < sizeof(FileHeader)) {
        close();
        return false;
    }

    mapped_data_ = mmap(nullptr, mapped_size_, PROT_READ, MAP_PRIVATE, fd_, 0);
    if (mapped_data_ == MAP_FAILED) {
        close();
        return false;
    }

    // Cast mapped data
    const uint8_t* byte_ptr = static_cast<const uint8_t*>(mapped_data_);
    header_ = reinterpret_cast<const FileHeader*>(byte_ptr);

    // Validate header
    if (header_->magic != LOG_MAGIC || header_->frame_size != sizeof(DataFrame)) {
        close();
        return false;
    }

    // Ensure size encompasses data
    size_t required_size = header_->data_offset + (header_->frame_count * header_->frame_size);
    if (mapped_size_ < required_size) {
        close();
        return false;
    }

    frames_ = reinterpret_cast<const DataFrame*>(byte_ptr + header_->data_offset);
    return true;
}

void ReplayStorage::close() {
    if (mapped_data_ != MAP_FAILED) {
        munmap(mapped_data_, mapped_size_);
        mapped_data_ = MAP_FAILED;
    }
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
    mapped_size_ = 0;
    header_ = nullptr;
    frames_ = nullptr;
}

bool ReplayStorage::isOpen() const {
    return mapped_data_ != MAP_FAILED;
}

uint64_t ReplayStorage::getFrameCount() const {
    if (!isOpen() || !header_) return 0;
    return header_->frame_count;
}

const DataFrame* ReplayStorage::getFrame(uint64_t index) const {
    if (!isOpen() || !header_ || !frames_) return nullptr;

    if (index >= header_->frame_count) {
        return nullptr;
    }

    // Zero-copy array access
    return &frames_[index];
}

const DataFrame* ReplayStorage::seekByTimestamp(uint32_t timestamp) const {
    if (!isOpen() || !header_ || !frames_ || header_->frame_count == 0) {
        return nullptr;
    }

    // Binary search over monotonic extended timestamp
    uint64_t left = 0;
    uint64_t right = header_->frame_count - 1;
    const DataFrame* best_match = nullptr;

    while (left <= right) {
        uint64_t mid = left + (right - left) / 2;
        const DataFrame* current = &frames_[mid];

        if (current->timestamp_ext == timestamp) {
            return current;
        } else if (current->timestamp_ext < timestamp) {
            // Target is greater, so this could be the closest previous match
            // or we just haven't found it yet. We'll search right.
            best_match = current;
            left = mid + 1;
        } else {
            // Target is smaller
            if (mid == 0) break; // Avoid underflow
            right = mid - 1;
        }
    }

    return best_match;
}
