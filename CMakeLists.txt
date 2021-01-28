cmake_minimum_required(VERSION 3.16)
project(SprintEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/spdlog)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glad)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/stb_image)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/imgui)
#add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/assimp)

set(SPRINT_SRCS
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
        common/color.cpp common/color.h
        common/signals/delegate.h
        common/signals/event.cpp common/signals/event.h
        common/signals/function_ref.cpp common/signals/function_ref.h
        common/signals/delegate_tests.cpp common/signals/delegate_tests.h
        common/slot_map.h
        common/time_span.cpp common/time_span.h
        renderer/gfx.cpp renderer/gfx.h
        renderer/vertex_layout.cpp renderer/vertex_layout.h
        renderer/gfx_details.cpp renderer/gfx_details.h
        renderer/renderer_api_gl.cpp renderer/renderer_api_gl.h
        renderer/shader_utils.cpp renderer/shader_utils.h
        renderer/gfx_config.cpp renderer/gfx_config.h
        common/iterator_range.h
        common/rect.cpp common/rect.h
        core/engine.cpp core/engine.h
        common/ecs/ecs.cpp common/ecs/ecs.h
        common/sparse_set.h
        core/input_events.cpp core/input_events.h
        common/semaphore.cpp common/semaphore.h
        debug/profiler.cpp debug/profiler.h
        core/macro.h
        common/sparse_map.h
        core/config.h
        core/scene.cpp core/scene.h
        math/transform.cpp math/transform.h
        math/math_utils.h
        core/components/transform_component.cpp core/components/transform_component.h)

set(EDITOR_SRCS ${SPRINT_SRCS} editor/editor_app.cpp editor/editor_app.h editor/imgui_renderer.cpp editor/imgui_renderer.h editor/editor_gui.h editor/editor_gui.cpp)

add_executable(SprintEngine ${SPRINT_SRCS} core/main.cpp)
add_executable(SprintEngine_Editor ${EDITOR_SRCS} editor/main.cpp)

target_include_directories(SprintEngine PRIVATE core/ renderer/ math/ common/)
target_include_directories(SprintEngine_Editor PRIVATE core/ renderer/ math/ common/)

target_link_libraries(SprintEngine PRIVATE glfw)
target_link_libraries(SprintEngine PRIVATE spdlog)
target_link_libraries(SprintEngine PRIVATE glad)
target_link_libraries(SprintEngine PRIVATE stb_image)
target_link_libraries(SprintEngine PRIVATE imgui)
#target_link_libraries(SprintEngine PRIVATE assimp)

target_link_libraries(SprintEngine_Editor PRIVATE glfw)
target_link_libraries(SprintEngine_Editor PRIVATE spdlog)
target_link_libraries(SprintEngine_Editor PRIVATE glad)
target_link_libraries(SprintEngine_Editor PRIVATE stb_image)
target_link_libraries(SprintEngine_Editor PRIVATE imgui)
#target_link_libraries(SprintEngine PRIVATE assimp)

target_compile_definitions(SprintEngine PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(SprintEngine_Editor PRIVATE GLFW_INCLUDE_NONE)