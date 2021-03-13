#pragma once

#include "gfx.h"

namespace sprint {

class Texture {

    class Loader {
    public:
        explicit Loader(const std::istream& in);
        explicit Loader(const std::string& path);
        ~Loader();

        Loader(const Loader& loader) = delete;
        Loader& operator=(const Loader& loader) = delete;

        Loader(Loader&& loader) noexcept;
        Loader& operator=(Loader&& loader) noexcept;

        explicit operator bool() const { return data_; }

        [[nodiscard]] int get_height() const { return height_; }
        [[nodiscard]] int get_width() const { return width_; }
        [[nodiscard]] int get_channels_num() const { return channels_; }

        [[nodiscard]] const uint8_t* get_data() const { return data_; }
    private:
        uint8_t* data_;
        int width_, height_, channels_;
    };

public:
    Texture(void* data, uint32_t width, uint32_t height, gfx::TextureFormat::Enum, gfx::TextureWrap = {}, gfx::TextureFilter = {}, gfx::TextureFlags::Type = gfx::TextureFlags::None);
    Texture(const Texture&) = delete;
    Texture(Texture&&) noexcept;
    ~Texture();

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) noexcept;

    [[nodiscard]] uint32_t get_width() const { return width_; }
    [[nodiscard]] uint32_t get_height() const { return height_; }

    static std::unique_ptr<Texture> Load(const std::istream& in);
    static std::unique_ptr<Texture> Load(const std::string& path);
    [[nodiscard]] gfx::texture_handle get_handle() const;

private:
    Texture(gfx::MemoryPtr ptr, uint32_t width, uint32_t height, gfx::TextureFormat::Enum, gfx::TextureWrap = {}, gfx::TextureFilter = {}, gfx::TextureFlags::Type = gfx::TextureFlags::None);

private:
    gfx::texture_handle handle_;
    uint32_t width_, height_;
};

}

