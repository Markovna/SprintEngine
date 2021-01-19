#pragma once

#include <imgui.h>
#include "Log.h"
#include "Shader.h"
#include "Texture.h"
#include "KeyCode.h"

namespace sprint {

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
        gfx::VertexBufferHandle vb_handle;
        gfx::IndexBufferHandle ib_handle;
        ImGuiContext* imgui_context;

        RenderContext(Shader shader, Texture texture, ImGuiContext* context) noexcept :
            shader(std::move(shader)),
            texture(std::move(texture)),
            imgui_context(context) {

            vb_handle = gfx::CreateVertexBuffer(gfx::MakeRef(nullptr, sizeof(buffer.vertices)), 0, {
                { gfx::Attribute::Binding::POSITION, gfx::Attribute::Format::Vec2 },
                { gfx::Attribute::Binding::TEXCOORD0, gfx::Attribute::Format::Vec2 } ,
                { gfx::Attribute::Binding::COLOR0, gfx::Attribute::Format::Vec4Byte, true }
            });

            ib_handle = gfx::CreateIndexBuffer(gfx::MakeRef(nullptr, sizeof(buffer.indexes)), 0);
        }

        RenderContext(RenderContext&& other) noexcept :
            shader(std::move(other.shader)),
            texture(std::move(other.texture)),
            imgui_context(other.imgui_context),
            vb_handle(other.vb_handle),
            ib_handle(other.ib_handle) {
            other.ib_handle = gfx::IndexBufferHandle::Invalid;
            other.vb_handle = gfx::VertexBufferHandle::Invalid;
            other.imgui_context = nullptr;
        }

        ~RenderContext() {
            if (imgui_context) {
                ImGuiIO &io = ImGui::GetIO();
                io.Fonts->TexID = nullptr;

                gui::DestroyContext(imgui_context);

                if (vb_handle.IsValid())
                    gfx::Destroy(vb_handle);

                if (ib_handle.IsValid())
                    gfx::Destroy(ib_handle);
            }
        }

        static RenderContext Create() {
            ImGuiContext* context = gui::CreateContext();

            ImGuiIO &io = ImGui::GetIO();

            // Build texture atlas
            unsigned char* pixels;
            int width, height;
            // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small)
            // because it is more likely to be compatible with user's existing shaders. If
            // your ImTextureId represent a higher-level concept than just a GL texture id,
            // consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            Texture texture(pixels, width, height, 4);

            Shader shader = Shader::Load(
                "assets/shaders/GUIShader.shader",
                {
                    gfx::Attribute::Binding::POSITION, gfx::Attribute::Binding::TEXCOORD0, gfx::Attribute::Binding::COLOR0
                });

            return { std::move(shader), std::move(texture), context };
        }
    };

public:
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

