cmake_minimum_required(VERSION 3.16)
project(SprintEngine)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/spdlog)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glad)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/stb_image)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/imgui)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/assimp)

add_executable(SprintEngine
        core/main.cpp
        core/Log.cpp core/Log.h
        core/WindowEvent.h
        core/Application.cpp core/Application.h
        core/Window.cpp core/Window.h
        renderer/Shader.cpp renderer/Shader.h
        math/Vector.cpp math/Vector.h
        renderer/Texture.cpp renderer/Texture.h
        math/Matrix.cpp math/Matrix.h
        math/Quat.cpp math/Quat.h
        core/KeyCode.h
        core/MouseCodes.h
        renderer/Camera.cpp renderer/Camera.h
        core/ModelLoader.cpp core/ModelLoader.h
        renderer/Mesh.cpp renderer/Mesh.h
        common/color.cpp common/color.h
        editor/imgui_renderer.cpp editor/imgui_renderer.h
        renderer/GL.cpp renderer/GL.h
        common/IdAllocator.h
        renderer/VertexLayout.cpp renderer/VertexLayout.h
        core/InputSystem.cpp core/InputSystem.h
        common/signals/delegate.h
        common/signals/event.cpp common/signals/event.h
        common/signals/function_ref.cpp common/signals/function_ref.h
        common/signals/delegate_tests.cpp common/signals/delegate_tests.h
        common/slot_map.h
        common/clock.cpp common/clock.h
        renderer/gfx.cpp renderer/gfx.h
        renderer/vertex_layout.cpp renderer/vertex_layout.h
        renderer/gfx_details.cpp renderer/gfx_details.h
        renderer/renderer_api_gl.cpp renderer/renderer_api_gl.h
        renderer/shader_utils.cpp renderer/shader_utils.h renderer/gfx_config.cpp renderer/gfx_config.h common/iterator_range.h common/rect.cpp common/rect.h)

target_include_directories(SprintEngine PRIVATE core/ renderer/ math/ common/)

target_link_libraries(SprintEngine PRIVATE glfw)
target_link_libraries(SprintEngine PRIVATE spdlog)
target_link_libraries(SprintEngine PRIVATE glad)
target_link_libraries(SprintEngine PRIVATE stb_image)
target_link_libraries(SprintEngine PRIVATE imgui)
target_link_libraries(SprintEngine PRIVATE assimp)

target_compile_definitions(SprintEngine PRIVATE GLFW_INCLUDE_NONE)