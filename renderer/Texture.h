#pragma once

#include <string>

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
private:
    unsigned int m_GLTextureID;
    Texture(const unsigned char *data, unsigned int width, unsigned int height, unsigned int channels);
};

}

