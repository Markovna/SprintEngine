#pragma once

#include <string>
#include "GL.h"

namespace Sprint {

class Texture {

    class Loader {
    public:
        explicit Loader(const std::string& path);
        ~Loader();

        Loader(const Loader& loader) = delete;
        Loader& operator=(const Loader& loader) = delete;

        Loader(Loader&& loader) noexcept;
        Loader& operator=(Loader&& loader) noexcept;

        explicit operator bool() const { return m_Data; }

        int GetHeight() const { return m_Height; }
        int GetWidth() const { return m_Width; }
        int GetChannelsNum() const { return m_Channels; }

        const unsigned char* GetData() const { return m_Data; }
    private:
        unsigned char* m_Data;
        int m_Width, m_Height, m_Channels;
    };

public:
    static std::shared_ptr<Texture> Load(const std::string& path);
    void Bind(uint32_t slot = 0) const;
    ~Texture();
private:
    Texture(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels);

    GL::TextureHandle m_Handle;
};

}

