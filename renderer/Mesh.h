#pragma once
#include "Vector.h"
#include "Color.h"

namespace Sprint {

constexpr static size_t MAX_NUMBER_OF_TEXTURECOORDS = 8;

class Mesh {
private:
    struct Face {
        uint32_t Indices[3] = { 0, 0, 0 };
    };

private:
    using VerticesArray = std::vector<Vec3>;
    using NormalsArray = std::vector<Vec3>;
    using TextureCoordsArray = std::vector<Vec2[MAX_NUMBER_OF_TEXTURECOORDS]>;
    using ColorsArray = std::vector<Color>;
    using FacesArray = std::vector<Face>;

public:
    Mesh() = default;

private:
    size_t             m_VerticesCount = 0;
    VerticesArray      m_Vertices;
    NormalsArray       m_Normals;
    TextureCoordsArray m_TextureCoords;
    FacesArray         m_Faces;
};

}