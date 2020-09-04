#include "Texture.h"

#include "stb_image.h"
#include "Log.h"

#include <glad/glad.h>

namespace Sprint {

Texture::Loader::Loader(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    m_Data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
}

Texture::Loader::~Loader() {
    if (m_Data)
        stbi_image_free(m_Data);
}

Texture::Loader::Loader(Loader&& loader) noexcept {
    m_Data = loader.m_Data;
    m_Width = loader.m_Width;
    m_Height = loader.m_Width;
    m_Channels = loader.m_Channels;

    loader.m_Data = nullptr;
    loader.m_Width = 0;
    loader.m_Height = 0;
    loader.m_Channels = 0;
}

Texture::Loader& Texture::Loader::operator=(Loader&& loader) noexcept {
    if (m_Data == loader.m_Data) return *this;

    m_Data = loader.m_Data;
    m_Width = loader.m_Width;
    m_Height = loader.m_Width;
    m_Channels = loader.m_Channels;

    loader.m_Data = nullptr;
    loader.m_Width = 0;
    loader.m_Height = 0;
    loader.m_Channels = 0;
    return *this;
}

std::shared_ptr<Texture> Texture::Load(const std::string& path) {
    Loader loader(path);
    if (loader) {
        return std::shared_ptr<Texture>(
                new Texture(loader.GetData(),
                            loader.GetWidth(),
                            loader.GetHeight(),
                            loader.GetChannelsNum()));
    }

    Log::CoreError("Failed to load texture {0}", path);
    return nullptr;
}

static GLenum GetFormat(unsigned int channels) {
    GLenum format = 0;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    return format;
}

Texture::Texture(const unsigned char *data, unsigned int width, unsigned int height, unsigned int channels) {
    glGenTextures(1, &m_GLTextureID);
    glBindTexture(GL_TEXTURE_2D, m_GLTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GetFormat(channels);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_GLTextureID);
}

}