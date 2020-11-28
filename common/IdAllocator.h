#pragma once

#include <string>
#include <numeric>
#include <bitset>

namespace sprint {

template<size_t Size>
class IdAllocator {
public:
    IdAllocator() noexcept : reserved_count_(0), bitset_() {
        std::iota(std::begin(free_indices_), std::end(free_indices_), 0);
    }

    bool HasFree() {
        return reserved_count_ < Size;
    }

    uint32_t Alloc() {
        bitset_[reserved_count_] = 0;
        return free_indices_[reserved_count_++];
    }

    void Free(uint32_t idx) {
        assert(idx < Size);
        assert(!bitset_[idx]);
        assert(reserved_count_);
        bitset_[idx] = 1;
        free_indices_[--reserved_count_] = idx;
    }
private:
    std::bitset<Size> bitset_;
    uint32_t free_indices_[Size];
    size_t reserved_count_;
};

}