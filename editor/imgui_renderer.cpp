#include "imgui_renderer.h"
#include "GL.h"

namespace sprint {

void ImGuiRenderer::BeginFrame(sprint::Window *window, float delta_time) {
    imgui_impl::NewFrame(window, delta_time);
    gui::NewFrame();
}

void ImGuiRenderer::EndFrame() {
    gui::Render();
    Render(gui::GetDrawData());
}

ImGuiRenderer::~ImGuiRenderer() {
    imgui_impl::Shutdown();
}

ImGuiRenderer::ImGuiRenderer() {
    gui::CreateContext();

    imgui_impl::Init();
}

void SetupRenderState() {

}

void ImGuiRenderer::Render(ImDrawData *draw_data) {

//    gl::CreateVertexBuffer((void*) cmd_list->VtxBuffer.Data,
//                           cmd_list->VtxBuffer.Size,
//                           {
//                               {gl::AttributeType::POSITION,  gl::AttributeFormat::Vec3 },
//                               {gl::AttributeType::TEXCOORD0, gl::AttributeFormat::Vec2 }
//                           },
//                           true
//    );
//    gl::CreateIndexBuffer((void*) cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];


    }
}

}
