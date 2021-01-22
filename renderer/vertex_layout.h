#pragma once

#include <string>
//#include "gfx.h"
#include "gfx_config.h"

namespace sprint {

namespace gfx {

struct Attribute {

    struct Type {
        enum Enum : uint8_t { FLOAT = 0, INT, UINT, BYTE };
    };

    struct Binding {
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

        constexpr static const uint32_t Count = 11;
    };

    struct BindingPack {
        using Mask = uint16_t;

        BindingPack(std::initializer_list<Binding::Enum> list) {
            uint16_t location = 0;
            for (Binding::Enum bnd : list) {
                mask |= (1u << bnd);
                locations[bnd] = location++;
            }
        }

        BindingPack() : mask(0) {}

        Mask mask = 0;
        uint16_t locations[Binding::Count];
    };

    struct Format {
        const static Format Byte;
        const static Format Int;
        const static Format Uint;
        const static Format Float;
        const static Format Vec2;
        const static Format Vec3;
        const static Format Vec4;
        const static Format Vec4Byte;

        Format(Type::Enum type, int size) noexcept : type(type), size(size) {}

        Type::Enum type;
        uint8_t size;
    };

    Attribute(Binding::Enum binding, Format format, bool normalized = false) :
        binding(binding), format(format), normalized(normalized) {}

    Attribute() : Attribute{Binding::POSITION, Format::Float, false} {}

    Binding::Enum binding;
    Format format;
    bool normalized;
};

class VertexLayout {
public:
    struct Item { Attribute attribute; size_t offset = 0; };
    typedef const Item* Iterator;
public:
    VertexLayout(std::initializer_list<Attribute> attributes);
    VertexLayout(const VertexLayout&) = default;

    inline const Item& operator[](size_t idx) const {
        assert(idx >= 0 && idx < size_);
        return attributes_[idx];
    }

    [[nodiscard]] inline Iterator begin() const { return std::begin(attributes_); }
    [[nodiscard]] inline Iterator end() const { return std::begin(attributes_) + size_; }

    [[nodiscard]] inline uint32_t get_stride() const { return stride_; }
    [[nodiscard]] inline uint32_t get_size() const { return size_; }

private:
    Item attributes_[static_config::kAttributesCapacity];
    uint32_t stride_ = 0;
    uint32_t size_ = 0;
};

}

}

