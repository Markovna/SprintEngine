#pragma once

#include <rect.h>
#include "gfx.h"

namespace sprint {

class Scene;

class Renderer {
public:
    static std::unique_ptr<Renderer> Create(Scene&);

    explicit Renderer(Scene& scene)
        : scene_(scene)
    {}
    void Render(Rect viewport, gfx::framebuf_handle = gfx::framebuf_handle::null);

private:
    Scene& scene_;
};

}