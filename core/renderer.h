#pragma once

#include "gfx.h"

namespace sprint {

class Scene;

class Renderer {
public:
    static std::unique_ptr<Renderer> Create(Scene&, gfx::framebuf_handle = gfx::framebuf_handle::null);

    explicit Renderer(Scene& scene, gfx::framebuf_handle fb_handle)
        : scene_(scene), fb_handle_(fb_handle)
    {}
    void Render();

private:
    Scene& scene_;
    gfx::framebuf_handle fb_handle_;
};

}