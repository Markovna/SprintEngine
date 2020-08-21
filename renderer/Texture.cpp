#include "Texture.h"

Texture::Loader::Loader(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    int nrChannels;
    m_Data = stbi_load(path.c_str(), &m_Width, &m_Height, &nrChannels, 0);
}

Texture::Loader::~Loader() {
    if (m_Data)
        stbi_image_free(m_Data);
}

Texture::Loader::Loader(Loader&& loader) {
    m_Data = loader.m_Data;
    m_Width = loader.m_Width;
    m_Height = loader.m_Width;

    loader.m_Data = nullptr;
    loader.m_Width = 0;
    loader.m_Height = 0;
}

Texture::Loader& Texture::Loader::operator=(Loader&& loader) {
    if (m_Data == loader.m_Data) return *this;

    m_Data = loader.m_Data;
    m_Width = loader.m_Width;
    m_Height = loader.m_Width;

    loader.m_Data = nullptr;
    loader.m_Width = 0;
    loader.m_Height = 0;
    return *this;
}

std::shared_ptr<Texture> Texture::Load(const std::string& path) {
    if (Loader loader(path); loader) {
        return std::shared_ptr<Texture>(
                new Texture(loader.GetData(),
                            loader.GetWidth(),
                            loader.GetHeight()));
    }

    Log::CoreError("Failed to load texture {0}", path);
    return nullptr;
}

Texture::Texture(const unsigned char *data, unsigned int width, unsigned int height) {
    glGenTextures(1, &m_GLTextureID);
    glBindTexture(GL_TEXTURE_2D, m_GLTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_GLTextureID);
}