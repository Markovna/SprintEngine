#include "render_texture.h"
namespace sprint
{

RenderTexture::RenderTexture(
    uint32_t width,
    uint32_t height,
    sprint::gfx::TextureFormat::Enum format,
    sprint::gfx::TextureWrap wrap,
    sprint::gfx::TextureFilter filter,
    sprint::gfx::TextureFlags::Type flags)
    : color_texture_(nullptr, width, height, format, wrap, filter, flags)
    , depth_tex_handle_(gfx::CreateTexture(width, height, gfx::TextureFormat::D24S8, {}, {}, gfx::TextureFlags::RenderTarget, {}))
    , fb_handle_(gfx::CreateFrameBuffer({color_texture_.get_handle(), depth_tex_handle_}))
{}

RenderTexture::~RenderTexture() {
    gfx::Destroy(depth_tex_handle_);
    gfx::Destroy(fb_handle_);
}

}