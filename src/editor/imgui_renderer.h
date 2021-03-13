#pragma once

#include <imgui.h>
#include "common/time_span.h"
#include "core/log.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "core/key_codes.h"
#include "core/window.h"

namespace sprint::editor {

namespace gui {

using namespace ImGui;

}

class ImGuiRenderer {
private:
    struct RenderContext {

        constexpr static const size_t kBufferMaxSize  = 100 * 2048;

        // TODO: buffers won't work properly when renderer become multithreaded
        struct Buffer {
            std::aligned_storage_t<kBufferMaxSize> vertices;
            std::aligned_storage_t<kBufferMaxSize> indexes;
        };

        Buffer buffer;
        Shader shader;
        Texture texture;
        gfx::uniform_handle texture_uniform_handle;
        gfx::vertexbuf_handle vb_handle;
        gfx::indexbuf_handle ib_handle;
        ImGuiContext* imgui_context;
        Timer timer;

        RenderContext(Shader shader_, Texture texture_, ImGuiContext* context_) noexcept :
            shader(std::move(shader_)),
            texture(std::move(texture_)),
            imgui_context(context_),
            timer() {

            vb_handle = gfx::CreateVertexBuffer(gfx::MakeRef(nullptr, sizeof(buffer.vertices)), 0, {
                { gfx::Attribute::Binding::POSITION, gfx::Attribute::Format::Vec2 },
                { gfx::Attribute::Binding::TEXCOORD0, gfx::Attribute::Format::Vec2 } ,
                { gfx::Attribute::Binding::COLOR0, gfx::Attribute::Format::Vec4Byte, true }
            });

            ib_handle = gfx::CreateIndexBuffer(gfx::MakeRef(nullptr, sizeof(buffer.indexes)), 0);
            texture_uniform_handle = gfx::CreateUniform("Texture");
        }

        RenderContext(RenderContext&& other) noexcept :
            shader(std::move(other.shader)),
            texture(std::move(other.texture)),
            imgui_context(other.imgui_context),
            vb_handle(other.vb_handle),
            ib_handle(other.ib_handle),
            texture_uniform_handle(other.texture_uniform_handle),
            timer(other.timer) {
            other.ib_handle = gfx::indexbuf_handle::null;
            other.vb_handle = gfx::vertexbuf_handle::null;
            other.texture_uniform_handle = gfx::uniform_handle::null;
            other.imgui_context = nullptr;
            other.timer = {};
        }

        ~RenderContext() {
            if (imgui_context) {
                ImGuiIO &io = ImGui::GetIO();
                io.Fonts->TexID = nullptr;

                gui::DestroyContext(imgui_context);

                if (vb_handle)
                    gfx::Destroy(vb_handle);

                if (ib_handle)
                    gfx::Destroy(ib_handle);

                if (texture_uniform_handle)
                    gfx::Destroy(texture_uniform_handle);
            }
        }

        static RenderContext Create() {
            ImGuiContext* context = gui::CreateContext();

            ImGuiIO &io = ImGui::GetIO();

            // Build texture atlas
            uint8_t* pixels;
            int width, height;
            // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small)
            // because it is more likely to be compatible with user's existing shaders. If
            // your ImTextureId represent a higher-level concept than just a GL texture id,
            // consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            Texture texture(pixels, width, height, gfx::TextureFormat::RGBA8);

            Shader shader = Shader::Load(
                "assets/shaders/GUIShader.shader",
                {
                    gfx::Attribute::Binding::POSITION, gfx::Attribute::Binding::TEXCOORD0, gfx::Attribute::Binding::COLOR0
                });

            return { std::move(shader), std::move(texture), context };
        }
    };

public:
    static std::unique_ptr<ImGuiRenderer> Create();

    ImGuiRenderer();
    ~ImGuiRenderer();

    void BeginFrame(Window* window);
    void EndFrame();

private:
    void Render(ImDrawData* draw_data);

private:
    RenderContext context_;
};

}

