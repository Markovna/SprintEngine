#include "Texture.h"

#include "stb_image.h"
#include "Log.h"

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

Texture::Texture(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) :
    m_Handle(GL::CreateTexture(data, width, height, channels)) {
}

void Texture::Bind(uint32_t slot) const {
    GL::Bind(m_Handle, slot);
}

Texture::~Texture() {
    GL::Destroy(m_Handle);
}

}