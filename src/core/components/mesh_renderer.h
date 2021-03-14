#pragma once

#include "renderer/gfx.h"
#include "reflection/meta_runtime.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "core/resources.h"

namespace sprint {

float* GetVertices();
uint32_t* GetIndices(size_t& count);

gfx::vertexbuf_handle GetVertexBuf();
gfx::indexbuf_handle GetIndexBuf();

gfx::uniform_handle GetMainTextureUniform();
gfx::uniform_handle GetSecondTextureUniform();
gfx::uniform_handle GetColorUniform();


class SERIALIZED MeshRenderer {
public:
//TODO:
    gfx::uniform_handle GetMainTextureUniform() const {
        return ::sprint::GetMainTextureUniform();
    }

    gfx::uniform_handle GetSecondTextureUniform() const {
        return ::sprint::GetSecondTextureUniform();
    }

    gfx::uniform_handle GetColorUniform() const {
        return ::sprint::GetColorUniform();
    }

    gfx::vertexbuf_handle GetVertexBuf() const {
        return ::sprint::GetVertexBuf();
    }

    gfx::indexbuf_handle GetIndexBuf() const {
        return ::sprint::GetIndexBuf();
    }

    [[nodiscard]] const Texture* GetMainTexture() const {
        return main_texture_.Get();
    }

    [[nodiscard]] const Texture* GetSecondTexture() const {
        return second_texture_.Get();
    }

    [[nodiscard]] const Shader* GetShader() const {
        return shader_.Get();
    }

    MeshRenderer() = default;
    ~MeshRenderer() = default;

    MeshRenderer(const MeshRenderer& o) = default;
    MeshRenderer(MeshRenderer&& o) noexcept = default;

    MeshRenderer& operator=(const MeshRenderer&) = default;
    MeshRenderer& operator=(MeshRenderer&&) noexcept = default;

private:
    resources::ResourceHandle<Texture> main_texture_ = resources::Load<Texture>("assets/textures/container.jpg");
    resources::ResourceHandle<Texture> second_texture_ = resources::Load<Texture>("assets/textures/seal.png");
    resources::ResourceHandle<Shader> shader_ = resources::Load<Shader>("assets/shaders/TestShader.shader");
};

}