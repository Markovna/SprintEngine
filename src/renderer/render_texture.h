#pragma once

#include "gfx.h"
#include "texture.h"

namespace sprint {

class RenderTexture {
public:
    RenderTexture(
        uint32_t width,
        uint32_t height,
        gfx::TextureFormat::Enum,
        gfx::TextureWrap = {},
        gfx::TextureFilter = {},
        gfx::TextureFlags::Type = gfx::TextureFlags::None);

    ~RenderTexture();

    [[nodiscard]] const Texture& get_texture() const { return color_texture_; }
    [[nodiscard]] gfx::framebuf_handle get_handle() const { return fb_handle_; }

private:
    Texture color_texture_;
    gfx::texture_handle depth_tex_handle_;
    gfx::framebuf_handle fb_handle_;
};

}