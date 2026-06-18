#include "ReplayStorage.h"

#ifdef _WIN32

ReplayStorage::ReplayStorage()
    : file_handle_(INVALID_HANDLE_VALUE), mapping_handle_(NULL),
      mapped_data_(nullptr), mapped_size_(0), header_(nullptr), frames_(nullptr) {
}

ReplayStorage::~ReplayStorage() {
    close();
}

bool ReplayStorage::open(const std::string& filename) {
    if (isOpen()) {
        close();
    }

    file_handle_ = CreateFileA(
        filename.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (file_handle_ == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(file_handle_, &file_size)) {
        close();
        return false;
    }

    mapped_size_ = static_cast<size_t>(file_size.QuadPart);
    if (mapped_size_ < sizeof(FileHeader)) {
        close();
        return false;
    }

    mapping_handle_ = CreateFileMappingA(
        file_handle_,
        NULL,
        PAGE_READONLY,
        0, 0,
        NULL
    );
    if (mapping_handle_ == NULL) {
        close();
        return false;
    }

    mapped_data_ = MapViewOfFile(
        mapping_handle_,
        FILE_MAP_READ,
        0, 0,
        0
    );
    if (mapped_data_ == nullptr) {
        close();
        return false;
    }

    const uint8_t* byte_ptr = static_cast<const uint8_t*>(mapped_data_);
    header_ = reinterpret_cast<const FileHeader*>(byte_ptr);

    if (header_->magic != LOG_MAGIC || header_->frame_size != sizeof(DataFrame)) {
        close();
        return false;
    }

    size_t required_size = header_->data_offset + (header_->frame_count * header_->frame_size);
    if (mapped_size_ < required_size) {
        close();
        return false;
    }

    frames_ = reinterpret_cast<const DataFrame*>(byte_ptr + header_->data_offset);
    return true;
}

void ReplayStorage::close() {
    if (mapped_data_ != nullptr) {
        UnmapViewOfFile(mapped_data_);
        mapped_data_ = nullptr;
    }
    if (mapping_handle_ != NULL) {
        CloseHandle(mapping_handle_);
        mapping_handle_ = NULL;
    }
    if (file_handle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(file_handle_);
        file_handle_ = INVALID_HANDLE_VALUE;
    }
    mapped_size_ = 0;
    header_ = nullptr;
    frames_ = nullptr;
}

bool ReplayStorage::isOpen() const {
    return mapped_data_ != nullptr;
}

#else // Linux / POSIX

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

    const uint8_t* byte_ptr = static_cast<const uint8_t*>(mapped_data_);
    header_ = reinterpret_cast<const FileHeader*>(byte_ptr);

    if (header_->magic != LOG_MAGIC || header_->frame_size != sizeof(DataFrame)) {
        close();
        return false;
    }

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

#endif // _WIN32

// ─── Platform-independent ────────────────────────────────────────────────────

uint64_t ReplayStorage::getFrameCount() const {
    if (!isOpen() || !header_) return 0;
    return header_->frame_count;
}

const DataFrame* ReplayStorage::getFrame(uint64_t index) const {
    if (!isOpen() || !header_ || !frames_) return nullptr;
    if (index >= header_->frame_count) return nullptr;
    return &frames_[index];
}

const DataFrame* ReplayStorage::seekByTimestamp(uint32_t timestamp) const {
    if (!isOpen() || !header_ || !frames_ || header_->frame_count == 0) {
        return nullptr;
    }

    uint64_t left = 0;
    uint64_t right = header_->frame_count - 1;
    const DataFrame* best_match = nullptr;

    while (left <= right) {
        uint64_t mid = left + (right - left) / 2;
        const DataFrame* current = &frames_[mid];

        if (current->timestamp_ext == timestamp) {
            return current;
        } else if (current->timestamp_ext < timestamp) {
            best_match = current;
            left = mid + 1;
        } else {
            if (mid == 0) break;
            right = mid - 1;
        }
    }

    return best_match;
}
