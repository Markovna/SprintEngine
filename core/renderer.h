#pragma once

#include <rect.h>
#include "gfx.h"
#include "components/camera.h"

namespace sprint {

class Scene;

class Renderer {
public:
    static std::unique_ptr<Renderer> Create(Scene&);

    explicit Renderer(Scene& scene) : scene_(scene) {}
    void Render(Rect viewport, gfx::framebuf_handle = gfx::framebuf_handle::null, Camera::Kind camera_kind = Camera::Game);

private:
    Scene& scene_;
};

}