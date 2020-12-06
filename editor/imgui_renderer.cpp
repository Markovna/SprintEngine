#include <gfx.h>
#include <Application.h>
#include "imgui_renderer.h"

namespace sprint {

static Window::Cursor::Type GetCursorType(ImGuiMouseCursor cursor) {
    using CursorsMap = std::array<Window::Cursor::Type, Window::Cursor::Count>;
    const static CursorsMap cursors_map = [](){
        CursorsMap map;
        map[ImGuiMouseCursor_Arrow] = Window::Cursor::Arrow;
        map[ImGuiMouseCursor_ResizeAll] = Window::Cursor::Resize_All;
        map[ImGuiMouseCursor_TextInput] = Window::Cursor::Text_Input;
        map[ImGuiMouseCursor_ResizeNS] = Window::Cursor::Resize_Vertical;
        map[ImGuiMouseCursor_ResizeEW] = Window::Cursor::Resize_Horizontal;
        map[ImGuiMouseCursor_ResizeNESW] = Window::Cursor::Resize_Bottom_Left_Top_Right;
        map[ImGuiMouseCursor_ResizeNWSE] = Window::Cursor::Resize_Top_Left_Bottom_Right;
        map[ImGuiMouseCursor_Hand] = Window::Cursor::Hand;
        map[ImGuiMouseCursor_NotAllowed] = Window::Cursor::Not_Allowed;
        return map;
    }();
    return cursors_map[cursor];
}

static void SetupKeyMap(ImGuiIO &io) {
    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = key::Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = key::Left;
    io.KeyMap[ImGuiKey_RightArrow] = key::Right;
    io.KeyMap[ImGuiKey_UpArrow] = key::Up;
    io.KeyMap[ImGuiKey_DownArrow] = key::Down;
    io.KeyMap[ImGuiKey_PageUp] = key::PageUp;
    io.KeyMap[ImGuiKey_PageDown] = key::PageDown;
    io.KeyMap[ImGuiKey_Home] = key::Home;
    io.KeyMap[ImGuiKey_End] = key::End;
    io.KeyMap[ImGuiKey_Insert] = key::Insert;
    io.KeyMap[ImGuiKey_Delete] = key::Delete;
    io.KeyMap[ImGuiKey_Backspace] = key::Backspace;
    io.KeyMap[ImGuiKey_Space] = key::Space;
    io.KeyMap[ImGuiKey_Enter] = key::Enter;
    io.KeyMap[ImGuiKey_Escape] = key::Escape;
    io.KeyMap[ImGuiKey_KeyPadEnter] = key::KPEnter;
    io.KeyMap[ImGuiKey_A] = key::A;
    io.KeyMap[ImGuiKey_C] = key::C;
    io.KeyMap[ImGuiKey_V] = key::V;
    io.KeyMap[ImGuiKey_X] = key::X;
    io.KeyMap[ImGuiKey_Y] = key::Y;
    io.KeyMap[ImGuiKey_Z] = key::Z;
}

void OnKeyPressed(KeyPressEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[event.key_code] = true;
    io.KeyCtrl = event.control;
    io.KeyShift = event.shift;
    io.KeyAlt = event.alt;
    io.KeySuper = event.super;
}

void OnKeyReleased(KeyReleaseEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[event.key_code] = false;
    io.KeyCtrl = event.control;
    io.KeyShift = event.shift;
    io.KeyAlt = event.alt;
    io.KeySuper = event.super;
}

void OnMouseDown(MouseDownEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.mouse_code] = true;
}

void OnMouseUp(MouseUpEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.mouse_code] = false;
}

void OnMouseMove(MouseMoveEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos.x = e.position.x;
    io.MousePos.y = e.position.y;

    io.MousePos.x *= io.DisplayFramebufferScale.x;
    io.MousePos.y *= io.DisplayFramebufferScale.y;
}

void OnScroll(ScrollEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += e.offset.y;
}

void OnTextInput(TextEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    if(e.unicode > 0 && e.unicode < 0x10000) {
        io.AddInputCharacter(e.unicode);
    }
}

ImGuiRenderer::~ImGuiRenderer() {
    Application::OnKeyPress.disconnect(OnKeyPressed);
    Application::OnKeyRelease.disconnect(OnKeyReleased);
    Application::OnMouseDown.disconnect(OnMouseDown);
    Application::OnMouseMove.disconnect(OnMouseMove);
    Application::OnMouseUp.disconnect(OnMouseUp);
    Application::OnScroll.disconnect(OnScroll);
    Application::OnTextInput.disconnect(OnTextInput);
}

ImGuiRenderer::ImGuiRenderer() : context_(RenderContext::Create()) {

    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    // Setup back-end capabilities flags
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    SetupKeyMap(io);

    Application::OnKeyPress.connect(OnKeyPressed);
    Application::OnKeyRelease.connect(OnKeyReleased);
    Application::OnMouseDown.connect(OnMouseDown);
    Application::OnMouseMove.connect(OnMouseMove);
    Application::OnMouseUp.connect(OnMouseUp);
    Application::OnScroll.connect(OnScroll);
    Application::OnTextInput.connect(OnTextInput);

    gfx::SetView(1, Matrix::Identity);
    gfx::SetClear(1, gfx::ClearFlag::Color | gfx::ClearFlag::Depth);
    gfx::SetClearColor(1, Color(0.1, 0.1, 0.1, 1.0));
}

void ImGuiRenderer::BeginFrame(sprint::Window *window, float delta_time) {
    ImGuiIO& io = gui::GetIO();

    if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
        static ImGuiMouseCursor last_cursor_type = gui::GetMouseCursor();
        auto cursor = GetCursorType(gui::GetMouseCursor());
        if (last_cursor_type != gui::GetMouseCursor()) {
            window->SetMouseCursor(cursor);
        }

        last_cursor_type = gui::GetMouseCursor();
    }

    io.DisplaySize = ImVec2(window->get_width(), window->get_height());
    io.DisplayFramebufferScale = {1.0,1.0};
    io.DeltaTime = delta_time;

    gfx::SetProjection(1, Matrix::Ortho(0, io.DisplaySize.x, 0, io.DisplaySize.y,0,1));

    gui::NewFrame();
}

void ImGuiRenderer::EndFrame() {
    gui::Render();
    Render(gui::GetDrawData());
}

void ImGuiRenderer::Render(ImDrawData *draw_data) {

    uint32_t vertices_offset = 0;
    uint32_t indices_offset = 0;

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        uint32_t num_vertices = cmd_list->VtxBuffer.size();
        uint32_t num_indices = cmd_list->IdxBuffer.size();

        gfx::UpdateVertexBuffer(
            context_.vb_handle,
            gfx::MakeRef((void*)cmd_list->VtxBuffer.begin(), num_vertices * sizeof(ImDrawVert)),
            vertices_offset * sizeof(ImDrawVert)
            );
        gfx::UpdateIndexBuffer(
            context_.ib_handle,
            gfx::MakeRef((void*)cmd_list->IdxBuffer.begin(), num_indices * sizeof(ImDrawIdx)),
            indices_offset * sizeof(ImDrawIdx)
            );

        std::uint32_t offset = 0;
        for (const ImDrawCmd *cmd = cmd_list->CmdBuffer.begin(), *cmdEnd = cmd_list->CmdBuffer.end();
             cmd != cmdEnd;
             ++cmd)
        {
            if(cmd->UserCallback != nullptr)
            {
                cmd->UserCallback(cmd_list, cmd);
            }
            else if(0 != cmd->ElemCount)
            {
                const std::uint16_t x = std::uint16_t(std::max(cmd->ClipRect.x, 0.0f));
                const std::uint16_t y = std::uint16_t(std::max(cmd->ClipRect.y, 0.0f));
                const std::uint16_t width = std::uint16_t(std::min(cmd->ClipRect.z, 65535.0f) - x);
                const std::uint16_t height = std::uint16_t(std::min(cmd->ClipRect.w, 65535.0f) - y);

//                auto tex_id = context_.texture.get_handle();
//                if (cmd->TextureId) {
//                }

                gfx::SetScissor({ x, y, width, height });
                gfx::SetOptions(gfx::DrawConfig::Option::NONE);
                gfx::SetUniform(context_.shader.get_handle(), "Texture", context_.texture.get_handle(), 0);
                gfx::SetBuffer(context_.vb_handle, vertices_offset, num_vertices);
                gfx::SetBuffer(context_.ib_handle, indices_offset + offset, cmd->ElemCount);
                gfx::Render(1, context_.shader.get_handle());
            }

            offset += cmd->ElemCount;
        }

        vertices_offset += num_vertices;
        indices_offset += num_indices;

    }


}

}
