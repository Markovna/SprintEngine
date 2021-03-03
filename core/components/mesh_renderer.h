#pragma once

#include "gfx.h"
#include "meta_runtime.h"
#include "shader.h"
#include "texture.h"

namespace sprint {

//TODO
Shader* GetShader();

Texture* GetMainTexture();

Texture* GetSecondTexture();

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

    Texture* GetMainTexture() const {
        return ::sprint::GetMainTexture();
    }

    Texture* GetSecondTexture() const {
        return ::sprint::GetSecondTexture();
    }

    Shader* GetShader() const {
        return ::sprint::GetShader();
    }

private:
//    SERIALIZABLE float float_value = 10.0f;
//    SERIALIZABLE int int_value = 42;
//    SERIALIZABLE bool bool_value = false;
};

}