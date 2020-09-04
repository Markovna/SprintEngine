#include "VertexLayout.h"

namespace Sprint {

namespace GL {

const AttributeFormat AttributeFormat::Float(GLType::FLOAT, 1);
const AttributeFormat AttributeFormat::Vec2(GLType::FLOAT, 2);
const AttributeFormat AttributeFormat::Vec3(GLType::FLOAT, 3);
const AttributeFormat AttributeFormat::Vec4(GLType::FLOAT, 4);

static uint32_t SizeOf(const AttributeFormat& type) {
    switch (type.Type) {
        case GLType::FLOAT: return type.Size * sizeof(float);
    }
}

VertexLayout::VertexLayout(std::initializer_list<Attribute> attributes) {
    assert(attributes.size() <= MAX_ATTRIBUTES_COUNT);

    for (const auto& attr : attributes) {
        m_Attributes[m_Size].Attribute = attr;
        m_Attributes[m_Size].Offset = m_Stride;
        m_Stride += SizeOf(attr.Format);
        m_Size++;
    }
}

}

}