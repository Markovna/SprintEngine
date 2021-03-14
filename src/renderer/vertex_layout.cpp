#include "vertex_layout.h"
#include "gfx.h"

namespace sprint {

namespace gfx {

const Attribute::Format Attribute::Format::Byte(Attribute::Type::BYTE, 1);
const Attribute::Format Attribute::Format::Vec4Byte(Attribute::Type::BYTE, 4);
const Attribute::Format Attribute::Format::Int(Attribute::Type::INT, 1);
const Attribute::Format Attribute::Format::Uint(Attribute::Type::UINT, 1);
const Attribute::Format Attribute::Format::Float(Attribute::Type::FLOAT, 1);
const Attribute::Format Attribute::Format::Vec2(Attribute::Type::FLOAT, 2);
const Attribute::Format Attribute::Format::Vec3(Attribute::Type::FLOAT, 3);
const Attribute::Format Attribute::Format::Vec4(Attribute::Type::FLOAT, 4);

static uint32_t SizeOf(const Attribute::Format& type) {
    switch (type.type) {
        case Attribute::Type::FLOAT: return type.size * sizeof(float);
        case Attribute::Type::UINT: return type.size * sizeof(uint32_t);
        case Attribute::Type::INT: return type.size * sizeof(int32_t);
        case Attribute::Type::BYTE: return type.size * sizeof(char);
    }
}

VertexLayout::VertexLayout(std::initializer_list<Attribute> attributes) {
    assert(attributes.size() <= static_config::kAttributesCapacity);

    for (const auto& attr : attributes) {
        attributes_[size_].attribute = attr;
        attributes_[size_].offset = stride_;
        stride_ += SizeOf(attr.format);
        size_++;
    }
}

bool Attribute::Binding::TryParse(std::string_view str, Attribute::Binding::Enum &type) {
    #define DEFINE_BINDING(__binding) { #__binding, __binding }
    static std::unordered_map<std::string, Attribute::Binding::Enum> types_map = {
        DEFINE_BINDING(POSITION),
        DEFINE_BINDING(NORMAL),
        DEFINE_BINDING(COLOR0),
        DEFINE_BINDING(COLOR1),
        DEFINE_BINDING(TANGENT),
        DEFINE_BINDING(TEXCOORD0),
        DEFINE_BINDING(TEXCOORD1),
        DEFINE_BINDING(TEXCOORD2),
        DEFINE_BINDING(TEXCOORD3),
        DEFINE_BINDING(TEXCOORD4),
        DEFINE_BINDING(TEXCOORD5)
    };
    #undef DEFINE_BINDING

    static std::string search_str;
    search_str = str;
    if (auto it = types_map.find(search_str); it != types_map.end()) {
        type = it->second;
        return true;
    }

    return false;
}

}

}