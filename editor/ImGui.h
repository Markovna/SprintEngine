#pragma once

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "KeyCode.h"

namespace sprint {

class ImGuiImpl {
public:
    void Init() {
        ImGui::StyleColorsDark();

        // Setup back-end capabilities flags
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

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

    void ShutDown() {

    }

    void Render(ImDrawData* draw_data) {

    }
};

class ImGui {
public:
    void BeginFrame(GLFWwindow* window, float delta_time) {

        ::ImGui::NewFrame();
    }

    void EndFrame() {
        ::ImGui::Render();

//        Render(::ImGui::GetDrawData());
    }
};

}

