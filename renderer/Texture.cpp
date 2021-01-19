#include "Texture.h"

#include <memory>
#include <utility>

#include "stb_image.h"
#include "Log.h"

namespace sprint {

Texture::Loader::Loader(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    data_ = stbi_load(path.c_str(), &width_, &height_, &channels_, 0);
}

Texture::Loader::~Loader() {
    if (data_) {
        stbi_image_free(data_);
        data_ = nullptr;
    }
}

Texture::Loader::Loader(Loader&& loader) noexcept {
    data_ = loader.data_;
    width_ = loader.width_;
    height_ = loader.width_;
    channels_ = loader.channels_;

    loader.data_ = nullptr;
    loader.width_ = 0;
    loader.height_ = 0;
    loader.channels_ = 0;
}

Texture::Loader& Texture::Loader::operator=(Loader&& loader) noexcept {
    if (data_ == loader.data_) return *this;

    data_ = loader.data_;
    width_ = loader.width_;
    height_ = loader.width_;
    channels_ = loader.channels_;

    loader.data_ = nullptr;
    loader.width_ = 0;
    loader.height_ = 0;
    loader.channels_ = 0;
    return *this;
}

std::unique_ptr<Texture> Texture::Load(const std::string& path) {
    Loader loader(path);
    if (loader) {
        return std::make_unique<Texture>(
                loader.get_data(),
                loader.get_width(),
                loader.get_height(),
                loader.get_channels_num());
    }

    log::core::Error("Failed to load texture {0}", path);
    return nullptr;
}

// TODO
static gfx::TextureFormat::Enum ToFormat(uint32_t channels) {
    if (channels == 1) return gfx::TextureFormat::R8;
    if (channels == 3) return gfx::TextureFormat::RGB8;
    return gfx::TextureFormat::RGBA8;
}

Texture::Texture(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) :
    handle_(gfx::CreateTexture(
        width,
        height,
        ToFormat(channels),
        gfx::Copy(data, sizeof(uint8_t) * width * height * channels))),
        width_(width), height_(height)
{}

Texture::~Texture() {
    if (handle_.IsValid())
        gfx::Destroy(handle_);
}

gfx::TextureHandle Texture::get_handle() const {
    return handle_;
}

Texture::Texture(Texture&& other) noexcept {
    std::swap(handle_, other.handle_);
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
}

Texture &Texture::operator=(Texture && other) noexcept {
    std::swap(handle_, other.handle_);
    return *this;
}

Texture::Texture(gfx::MemoryPtr ptr,
                 uint32_t width, uint32_t height,
                 gfx::TextureFormat::Enum format,
                 gfx::TextureWrap wrap,
                 gfx::TextureFilter filter,
                 gfx::TextureFlags::Type flags)
     : handle_(gfx::CreateTexture(width, height, format, wrap, filter, flags, std::move(ptr))),
       width_(width), height_(height) {

}

}