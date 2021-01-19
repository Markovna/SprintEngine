#include <gfx.h>
#include <Application.h>
#include "imgui_renderer.h"
#include "input_events.h"

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
    input_events::OnKeyPress.disconnect(OnKeyPressed);
    input_events::OnKeyRelease.disconnect(OnKeyReleased);
    input_events::OnMouseDown.disconnect(OnMouseDown);
    input_events::OnMouseMove.disconnect(OnMouseMove);
    input_events::OnMouseUp.disconnect(OnMouseUp);
    input_events::OnScroll.disconnect(OnScroll);
    input_events::OnTextInput.disconnect(OnTextInput);
}

ImGuiRenderer::ImGuiRenderer() : context_(RenderContext::Create()) {

    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();

    io.Fonts->TexID = (ImTextureID) &context_.texture;

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // TODO
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    SetupKeyMap(io);

    input_events::OnKeyPress.connect(OnKeyPressed);
    input_events::OnKeyRelease.connect(OnKeyReleased);
    input_events::OnMouseDown.connect(OnMouseDown);
    input_events::OnMouseMove.connect(OnMouseMove);
    input_events::OnMouseUp.connect(OnMouseUp);
    input_events::OnScroll.connect(OnScroll);
    input_events::OnTextInput.connect(OnTextInput);

    gfx::SetView(1, Matrix::Identity);
    gfx::SetClear(1, gfx::ClearFlag::Color | gfx::ClearFlag::Depth);
    gfx::SetClearColor(1, Color(0.1, 0.1, 0.1, 1.0));
}

void ImGuiRenderer::BeginFrame(sprint::Window *window) {
    ImGuiIO& io = gui::GetIO();

    if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
        static ImGuiMouseCursor last_cursor_type = gui::GetMouseCursor();
        auto cursor = GetCursorType(gui::GetMouseCursor());
        if (last_cursor_type != gui::GetMouseCursor()) {
            window->SetMouseCursor(cursor);
        }

        last_cursor_type = gui::GetMouseCursor();
    }

    Vec2Int resolution = window->get_resolution();
    io.DisplaySize = ImVec2(window->get_width(), window->get_height());
    io.DisplayFramebufferScale = { (float) resolution.x / io.DisplaySize.x, (float) resolution.y / io.DisplaySize.y };
    io.DeltaTime = time::delta().AsSeconds();

    gfx::SetProjection(1, Matrix::Ortho(0, io.DisplaySize.x, 0, io.DisplaySize.y,0,1));
    gfx::SetViewRect(1, {0, 0, (uint32_t) resolution.x, (uint32_t) resolution.y });

    gui::NewFrame();
}

void ImGuiRenderer::EndFrame() {
    gui::Render();
    Render(gui::GetDrawData());
}

void ImGuiRenderer::Render(ImDrawData *draw_data) {
    uint32_t vertices_offset = 0;
    uint32_t indices_offset = 0;

    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

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
            if (cmd->UserCallback) {
                cmd->UserCallback(cmd_list, cmd);
            }
            else if (cmd->ElemCount) {
                const std::uint16_t x = (cmd->ClipRect.x - clip_off.x)* clip_scale.x;
                const std::uint16_t y = (cmd->ClipRect.y - clip_off.y) * clip_scale.x;
                const std::uint16_t width  = (cmd->ClipRect.z - clip_off.x) * clip_scale.x - x;
                const std::uint16_t height = (cmd->ClipRect.w - clip_off.y) * clip_scale.x - y;

                auto texture = static_cast<Texture*>(cmd->TextureId);
                gfx::SetScissor({ x, y, width, height });
                gfx::SetOptions(gfx::DrawConfig::Option::NONE);
                gfx::SetUniform(context_.shader.get_handle(), "Texture", texture->get_handle(), 0);
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
