#include "VertexLayout.h"

namespace sprint {

namespace gl {

const AttributeFormat AttributeFormat::Float(GLType::FLOAT, 1);
const AttributeFormat AttributeFormat::Vec2(GLType::FLOAT, 2);
const AttributeFormat AttributeFormat::Vec3(GLType::FLOAT, 3);
const AttributeFormat AttributeFormat::Vec4(GLType::FLOAT, 4);

static uint32_t SizeOf(const AttributeFormat& type) {
    switch (type.type) {
        case GLType::FLOAT: return type.size * sizeof(float);
    }
}

VertexLayout::VertexLayout(std::initializer_list<Attribute> attributes) {
    assert(attributes.size() <= kMaxAttributesCount);

    for (const auto& attr : attributes) {
      attributes_[size_].attribute = attr;
      attributes_[size_].offset = stride_;
      stride_ += SizeOf(attr.format);
        size_++;
    }
}

}

}