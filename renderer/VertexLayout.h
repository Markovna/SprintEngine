#pragma once

#include <string>

namespace sprint {

namespace gl {

constexpr static uint16_t kMaxAttributesCount = 16;

enum class GLType : uint8_t { FLOAT };

struct AttributeType {
    enum Enum : uint8_t {
        POSITION  = 0,
        NORMAL    = 1,
        COLOR0    = 2,
        COLOR1    = 3,
        TANGENT   = 4,
        TEXCOORD0 = 5,
        TEXCOORD1 = 6,
        TEXCOORD2 = 7,
        TEXCOORD3 = 8,
        TEXCOORD4 = 9,
        TEXCOORD5 = 10
    };

    constexpr static uint32_t Count = 11;
};

struct AttributeFormat {
    const static AttributeFormat Float;
    const static AttributeFormat Vec2;
    const static AttributeFormat Vec3;
    const static AttributeFormat Vec4;

    AttributeFormat(GLType type, int size) noexcept : type(type), size(size) {}

    GLType type;
    uint8_t size;
};

struct Attribute {
    Attribute(AttributeType::Enum type, AttributeFormat format, bool normalized = false) :
        type(type), format(format), normalized(normalized) {}

    Attribute() : Attribute{AttributeType::POSITION, AttributeFormat::Float, false} {}

    AttributeType::Enum type;
    AttributeFormat format;
    bool normalized;
};

class VertexLayout {
public:
    struct Item { Attribute attribute; size_t offset = 0; };
    typedef const Item* Iterator;
public:
    VertexLayout(std::initializer_list<Attribute> attributes);

    inline const Item& operator[](size_t idx) const {
        assert(idx >= 0 && idx < size_);
        return attributes_[idx];
    }

    inline Iterator begin() const { return std::begin(attributes_); }
    inline Iterator end() const { return std::begin(attributes_) + size_; }

    inline uint32_t get_stride() const { return stride_; }
    inline uint32_t get_size() const { return size_; }

private:
    Item attributes_[kMaxAttributesCount];
    uint32_t stride_ = 0;
    uint32_t size_ = 0;
};

}

}
