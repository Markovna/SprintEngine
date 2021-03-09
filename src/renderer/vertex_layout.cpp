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

}

}