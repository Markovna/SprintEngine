#pragma once

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "KeyCode.h"


namespace Sprint {

class ImGuiRenderer {
public:
    void Init() {
        ImGui::StyleColorsDark();

        // Setup back-end capabilities flags
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
        io.KeyMap[ImGuiKey_Tab] = Key::Tab;
        io.KeyMap[ImGuiKey_LeftArrow] = Key::Left;
        io.KeyMap[ImGuiKey_RightArrow] = Key::Right;
        io.KeyMap[ImGuiKey_UpArrow] = Key::Up;
        io.KeyMap[ImGuiKey_DownArrow] = Key::Down;
        io.KeyMap[ImGuiKey_PageUp] = Key::PageUp;
        io.KeyMap[ImGuiKey_PageDown] = Key::PageDown;
        io.KeyMap[ImGuiKey_Home] = Key::Home;
        io.KeyMap[ImGuiKey_End] = Key::End;
        io.KeyMap[ImGuiKey_Insert] = Key::Insert;
        io.KeyMap[ImGuiKey_Delete] = Key::Delete;
        io.KeyMap[ImGuiKey_Backspace] = Key::Backspace;
        io.KeyMap[ImGuiKey_Space] = Key::Space;
        io.KeyMap[ImGuiKey_Enter] = Key::Enter;
        io.KeyMap[ImGuiKey_Escape] = Key::Space;
        io.KeyMap[ImGuiKey_KeyPadEnter] = Key::KPEnter;
        io.KeyMap[ImGuiKey_A] = Key::A;
        io.KeyMap[ImGuiKey_C] = Key::C;
        io.KeyMap[ImGuiKey_V] = Key::V;
        io.KeyMap[ImGuiKey_X] = Key::X;
        io.KeyMap[ImGuiKey_Y] = Key::Y;
        io.KeyMap[ImGuiKey_Z] = Key::Z;
    }

    void ShutDown() {

    }

    void Render(ImDrawData* draw_data) {

    }
};

class ImGui {
public:
    void BeginFrame(GLFWwindow* window, float deltaTime) {

        ::ImGui::NewFrame();
    }

    void EndFrame() {
        ::ImGui::Render();

//        Render(::ImGui::GetDrawData());
    }
};

}

