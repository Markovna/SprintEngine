#pragma once

#include <array>

namespace sprint {

class guid {
public:
    guid() = default;

    guid(const guid &) = default;
    guid(guid &&) = default;

    guid &operator=(const guid &) = default;
    guid &operator=(guid &&) = default;

    bool operator==(const guid &) const;
    bool operator!=(const guid &) const;

    explicit operator std::string() const;

    [[nodiscard]] std::string str() const;
    [[nodiscard]] const std::array<uint8_t, 16> &bytes() const;
    [[nodiscard]] bool is_valid() const;

    void swap(guid &);

    static guid generate();
    static guid invalid();

private:
    friend std::ostream &operator<<(std::ostream &, const guid &);
    friend std::istream &operator>>(std::istream &, guid &);
    friend bool operator<(const guid &, const guid &);

private:
    std::array<uint8_t, 16> bytes_ = {0};
};

}
