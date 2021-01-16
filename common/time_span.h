#pragma once

#include <chrono>

namespace sprint {

class TimeSpan {
private:
    using seconds_t = std::chrono::duration<float, std::chrono::seconds::period>;
    using milliseconds_t = std::chrono::milliseconds;
    using microseconds_t = std::chrono::microseconds;

    static constexpr const float kSecToMicrosec = 1000000.0f;
    static constexpr const float kMillisecToMicrosec = 1000.0f;

public:
    float AsSeconds() const {
        return std::chrono::duration_cast<seconds_t>(microseconds_).count();
    }
    int32_t AsMilliseconds() const {
        return std::chrono::duration_cast<milliseconds_t>(microseconds_).count();
    }
    int64_t AsMicroseconds() const {
        return microseconds_.count();
    }
    static TimeSpan Seconds(float seconds) {
        return TimeSpan(microseconds_t(int64_t(seconds * kSecToMicrosec)));
    }
    static TimeSpan Milliseconds(int32_t milliseconds) {
        return TimeSpan(microseconds_t(int64_t(milliseconds * kMillisecToMicrosec)));
    }
    static TimeSpan Microseconds(int64_t microseconds) {
        return TimeSpan(microseconds_t(int64_t(microseconds)));
    }

private:
    microseconds_t microseconds_;

public:
    explicit TimeSpan(microseconds_t microseconds) noexcept : microseconds_(microseconds) {}
    TimeSpan() noexcept : microseconds_(0) {}
};

class Clock {
private:
    using time_point_t = std::chrono::steady_clock::time_point;
public:
    Clock() noexcept : time_point_(std::chrono::steady_clock::now()) {}

    TimeSpan Restart() {
        TimeSpan time = GetTime();
        time_point_ = std::chrono::steady_clock::now();
        return time;
    }

    TimeSpan GetTime() const {
        using namespace std::chrono;
        return TimeSpan(duration_cast<microseconds>(steady_clock::now() - time_point_));
    }
private:
    time_point_t time_point_;
};

}
