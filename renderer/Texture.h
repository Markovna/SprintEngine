#pragma once

#include <string>
//#include "GL.h"
#include "gfx.h"

namespace sprint {

class Texture {

    class Loader {
    public:
        explicit Loader(const std::string& path);
        ~Loader();

        Loader(const Loader& loader) = delete;
        Loader& operator=(const Loader& loader) = delete;

        Loader(Loader&& loader) noexcept;
        Loader& operator=(Loader&& loader) noexcept;

        explicit operator bool() const { return data_; }

        int get_height() const { return height_; }
        int get_width() const { return width_; }
        int get_channels_num() const { return channels_; }

        const unsigned char* get_data() const { return data_; }
    private:
        unsigned char* data_;
        int width_, height_, channels_;
    };

public:
    Texture(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels);
    Texture(gfx::MemoryPtr ptr, uint32_t width, uint32_t height, gfx::TextureFormat::Enum, gfx::TextureWrap, gfx::TextureFilter, gfx::TextureFlags::Type);
    Texture(const Texture&) = delete;
    Texture(Texture&&) noexcept;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) noexcept;

    uint32_t get_width() const { return width_; }
    uint32_t get_height() const { return height_; }

    static std::unique_ptr<Texture> Load(const std::string& path);
    ~Texture();
    gfx::texture_handle get_handle() const;

private:
    gfx::texture_handle handle_;
    uint32_t width_, height_;
};

}

