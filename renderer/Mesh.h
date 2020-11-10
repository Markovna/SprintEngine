#pragma once
#include "Vector.h"
#include "color.h"

namespace sprint {

constexpr static size_t kMaxNumberOfTexturecoords = 8;

class Mesh {
private:
    struct Face {
        uint32_t indices[3] = {0, 0, 0 };
    };

private:
    using VerticesArray = std::vector<Vec3>;
    using NormalsArray = std::vector<Vec3>;
    using TextureCoordsArray = std::vector<Vec2[kMaxNumberOfTexturecoords]>;
    using ColorsArray = std::vector<Color>;
    using FacesArray = std::vector<Face>;

public:
    Mesh() = default;

private:
    size_t             vertices_count_ = 0;
    VerticesArray      vertices_;
    NormalsArray       normals_;
    TextureCoordsArray texture_coords_;
    FacesArray         faces_;
};

}