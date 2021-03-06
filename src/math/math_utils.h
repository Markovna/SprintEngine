#pragma once

#include <limits>
#include <cmath>

namespace sprint::math {

constexpr float PI = M_PI;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 1.0f / DEG_TO_RAD;

template <class T>
bool IsNaN(const T& value) { return isnan(value); }

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
inline Approximately(T x, T y, int ulp = 1) {
    T diff = x - y;
    return std::fabs(diff) <= std::numeric_limits<T>::epsilon() * std::fabs(x+y) * ulp
        // unless the result is subnormal
        || std::fabs(diff) < std::numeric_limits<T>::min();
}

}