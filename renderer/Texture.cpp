#include "Texture.h"

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
        return std::unique_ptr<Texture>(
                new Texture(loader.get_data(),
                            loader.get_width(),
                            loader.get_height(),
                            loader.get_channels_num()));
    }

    log::core::Error("Failed to load texture {0}", path);
    return nullptr;
}

Texture::Texture(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) :
    handle_(gfx::CreateTexture(gfx::Copy(data, sizeof(uint8_t) * width * height * channels), width, height, channels)) {
}

Texture::~Texture() {
    if (handle_.IsValid())
        gfx::Destroy(handle_);
}
gfx::TextureHandle Texture::get_handle() const {
    return handle_;
}

Texture::Texture(Texture&& other) noexcept {
    std::swap(handle_, other.handle_);
}

Texture &Texture::operator=(Texture && other) noexcept {
    std::swap(handle_, other.handle_);
    return *this;
}

}