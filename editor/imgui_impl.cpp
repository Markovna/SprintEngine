#include <Shader.h>
#include <VertexLayout.h>
#include <Texture.h>
#include <Application.h>
#include "imgui_impl.h"

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

static std::unique_ptr<Shader> shader_;
static std::unique_ptr<Texture> texture_;

void SetupFonts(ImGuiIO& io) {

    // Build texture atlas
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    texture_ = std::make_unique<Texture>(pixels, width, height, 4);
    io.Fonts->TexID = (ImTextureID)(intptr_t) texture_->get_handle().ID;
}

void SetupGraphics(ImGuiIO& io) {
    shader_ = std::make_unique<Shader>(Shader::Load(
        "assets/shaders/GUIShader.shader",
        {
            gl::AttributeType::POSITION, gl::AttributeType::TEXCOORD0, gl::AttributeType::COLOR0
        }));
}

void SetupKeyMap(ImGuiIO &io) {
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
    io.KeyMap[ImGuiKey_Escape] = key::Space;
    io.KeyMap[ImGuiKey_KeyPadEnter] = key::KPEnter;
    io.KeyMap[ImGuiKey_A] = key::A;
    io.KeyMap[ImGuiKey_C] = key::C;
    io.KeyMap[ImGuiKey_V] = key::V;
    io.KeyMap[ImGuiKey_X] = key::X;
    io.KeyMap[ImGuiKey_Y] = key::Y;
    io.KeyMap[ImGuiKey_Z] = key::Z;
}

void CheckKeyModifiers(ImGuiIO& io) {
    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[key::LeftControl] || io.KeysDown[key::RightControl];
    io.KeyShift = io.KeysDown[key::LeftShift] || io.KeysDown[key::RightShift];
    io.KeyAlt = io.KeysDown[key::LeftAlt] || io.KeysDown[key::RightAlt];
#ifdef _WIN32
    io.KeySuper = false;
#else
    io.KeySuper = io.KeysDown[key::LeftSuper] || io.KeysDown[key::RightSuper];
#endif
}

void OnKeyPressed(KeyEvent& key_event) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key_event.KeyCode] = true;
    CheckKeyModifiers(io);
}

void OnKeyReleased(KeyEvent& key_event) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key_event.KeyCode] = false;
    CheckKeyModifiers(io);
}

void imgui_impl::Init() {

    ImGuiIO &io = ImGui::GetIO();

    SetupFonts(io);
    SetupGraphics(io);

    ImGui::StyleColorsDark();

    // Setup back-end capabilities flags
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    SetupKeyMap(io);

    Application::OnKeyPress.connect(OnKeyPressed);
    Application::OnKeyRelease.connect(OnKeyReleased);
}

void imgui_impl::Shutdown() {
    Application::OnKeyPress.disconnect(OnKeyPressed);
    Application::OnKeyRelease.disconnect(OnKeyReleased);
}

void imgui_impl::NewFrame(Window* const window, float delta_time) {
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
    io.DeltaTime = delta_time;



}

}