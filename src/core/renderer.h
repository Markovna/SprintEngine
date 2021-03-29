#pragma once

#include "common/rect.h"
#include "renderer/gfx.h"
#include "components/camera.h"
#include "scene.h"

namespace sprint {

class World;

class Renderer {
public:
    static std::unique_ptr<Renderer> Create(World&);

    explicit Renderer(World& scene) : scene_(scene) {}
    void Render(Rect viewport, gfx::framebuf_handle = gfx::framebuf_handle::null, Camera::Kind camera_kind = Camera::Game);

private:
    World& scene_;
};

}