#pragma once

#include <string>

namespace Sprint {

namespace GL {

constexpr static uint16_t MAX_ATTRIBUTES_COUNT = 16;

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

    AttributeFormat(GLType type, int size) noexcept : Type(type), Size(size) {}

    GLType Type;
    uint8_t Size;
};

struct Attribute {
    Attribute(AttributeType::Enum type, AttributeFormat format, bool normalized = false) :
            Type(type), Format(format), Normalized(normalized) {}

    Attribute() : Attribute{AttributeType::POSITION, AttributeFormat::Float, false} {}

    AttributeType::Enum Type;
    AttributeFormat Format;
    bool Normalized;
};

class VertexLayout {
public:
    struct Item { Attribute Attribute; size_t Offset = 0; };
    typedef const Item* Iterator;
public:
    VertexLayout(std::initializer_list<Attribute> attributes);

    inline const Item& operator[](size_t idx) const {
        assert(idx >= 0 && idx < m_Size);
        return m_Attributes[idx];
    }

    inline Iterator begin() const { return std::begin(m_Attributes); }
    inline Iterator end() const { return std::begin(m_Attributes) + m_Size; }

    inline uint32_t GetStride() const { return m_Stride; }
    inline uint32_t GetSize() const { return m_Size; }

private:
    Item m_Attributes[MAX_ATTRIBUTES_COUNT];
    uint32_t m_Stride = 0;
    uint32_t m_Size = 0;
};

}

}
