#pragma once

#include <string>
#include <numeric>
#include <bitset>

namespace Sprint {

template<size_t N, size_t INVALID_ID = N>
class IntAlloc {
public:
    IntAlloc() noexcept : m_ReservedCount(0), bitset_() {
        std::iota(std::begin(m_FreeIndices), std::end(m_FreeIndices), 0);
    }

    uint32_t Alloc() {
        if (m_ReservedCount == N) return INVALID_ID;
        bitset_[m_ReservedCount] = 0;
        return m_FreeIndices[m_ReservedCount++];
    }

    void Free(uint32_t idx) {
        assert(idx < N);
        assert(!bitset_[idx]);
        assert(m_ReservedCount);
        bitset_[idx] = 1;
        m_FreeIndices[--m_ReservedCount] = idx;
    }
private:
    std::bitset<N> bitset_;
    uint32_t m_FreeIndices[N];
    size_t m_ReservedCount;
};

}