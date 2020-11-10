#pragma once

#include <string>
#include <numeric>
#include <bitset>

namespace sprint {

template<size_t N>
class id_alloc {
public:
    id_alloc() noexcept : reserved_count_(0), bitset_() {
        std::iota(std::begin(free_indices_), std::end(free_indices_), 0);
    }

    bool HasFree() {
        return reserved_count_ < N;
    }

    uint32_t Alloc() {
        bitset_[reserved_count_] = 0;
        return free_indices_[reserved_count_++];
    }

    void Free(uint32_t idx) {
        assert(idx < N);
        assert(!bitset_[idx]);
        assert(reserved_count_);
        bitset_[idx] = 1;
        free_indices_[--reserved_count_] = idx;
    }
private:
    std::bitset<N> bitset_;
    uint32_t free_indices_[N];
    size_t reserved_count_;
};

}