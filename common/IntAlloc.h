#pragma once

#include <string>
#include <numeric>

namespace Sprint {

template<size_t N, size_t INVALID_ID = N>
class IntAlloc {
public:
    IntAlloc() noexcept : m_ReservedCount(0) {
        std::iota(std::begin(m_FreeIndices), std::end(m_FreeIndices), 0);
        std::fill(std::begin(m_IsFree), std::end(m_IsFree), false);
    }

    uint32_t Alloc() {
        if (m_ReservedCount == N) return INVALID_ID;
        m_IsFree[m_ReservedCount] = false;
        return m_FreeIndices[m_ReservedCount++];
    }

    void Free(uint32_t idx) {
        assert(idx < N);
        assert(!m_IsFree[idx]);
        assert(m_ReservedCount);
        m_IsFree[idx] = true;
        m_FreeIndices[--m_ReservedCount] = idx;
    }
private:
    bool m_IsFree[N];
    uint32_t m_FreeIndices[N];
    size_t m_ReservedCount;
};

}