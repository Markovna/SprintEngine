cmake_minimum_required(VERSION 3.16)
project(SprintEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(thirdparty/spdlog spdlog)
add_subdirectory(thirdparty/glfw glfw)
add_subdirectory(thirdparty/glad glad)
add_subdirectory(thirdparty/stb_image stb_image)
add_subdirectory(thirdparty/imgui imgui)

set(SOURCE_DIR src)

add_subdirectory(${SOURCE_DIR}/reflection/generator)

set(ENGINE_SRC
#        ${SOURCE_DIR}/sprintpch.h
        ${SOURCE_DIR}/core/log.cpp ${SOURCE_DIR}/core/log.h
        ${SOURCE_DIR}/core/window_event.h
        ${SOURCE_DIR}/core/application.cpp ${SOURCE_DIR}/core/application.h
        ${SOURCE_DIR}/core/window.cpp ${SOURCE_DIR}/core/window.h
        ${SOURCE_DIR}/renderer/shader.cpp ${SOURCE_DIR}/renderer/shader.h
        ${SOURCE_DIR}/math/vector.cpp ${SOURCE_DIR}/math/vector.h
        ${SOURCE_DIR}/renderer/texture.cpp ${SOURCE_DIR}/renderer/texture.h
        ${SOURCE_DIR}/math/matrix.cpp ${SOURCE_DIR}/math/matrix.h
        ${SOURCE_DIR}/math/quat.cpp ${SOURCE_DIR}/math/quat.h
        ${SOURCE_DIR}/core/key_codes.h
        ${SOURCE_DIR}/core/mouse_codes.h
        ${SOURCE_DIR}/common/color.cpp ${SOURCE_DIR}/common/color.h
        ${SOURCE_DIR}/common/signals/delegate.h
        ${SOURCE_DIR}/common/signals/event.cpp ${SOURCE_DIR}/common/signals/event.h
        ${SOURCE_DIR}/common/signals/delegate_tests.cpp ${SOURCE_DIR}/common/signals/delegate_tests.h
        ${SOURCE_DIR}/common/slot_map.h
        ${SOURCE_DIR}/common/time_span.cpp ${SOURCE_DIR}/common/time_span.h
        ${SOURCE_DIR}/renderer/gfx.cpp ${SOURCE_DIR}/renderer/gfx.h
        ${SOURCE_DIR}/renderer/vertex_layout.cpp ${SOURCE_DIR}/renderer/vertex_layout.h
        ${SOURCE_DIR}/renderer/gfx_details.cpp ${SOURCE_DIR}/renderer/gfx_details.h
        ${SOURCE_DIR}/renderer/renderer_api_gl.cpp ${SOURCE_DIR}/renderer/renderer_api_gl.h
        ${SOURCE_DIR}/renderer/shader_utils.cpp ${SOURCE_DIR}/renderer/shader_utils.h
        ${SOURCE_DIR}/renderer/gfx_config.cpp ${SOURCE_DIR}/renderer/gfx_config.h
        ${SOURCE_DIR}/common/iterator_range.h
        ${SOURCE_DIR}/common/rect_int.cpp ${SOURCE_DIR}/common/rect_int.h
        ${SOURCE_DIR}/core/engine.cpp ${SOURCE_DIR}/core/engine.h
        ${SOURCE_DIR}/common/ecs/ecs.cpp ${SOURCE_DIR}/common/ecs/ecs.h
        ${SOURCE_DIR}/common/sparse_set.h
        ${SOURCE_DIR}/core/input_events.cpp ${SOURCE_DIR}/core/input_events.h
        ${SOURCE_DIR}/common/semaphore.cpp ${SOURCE_DIR}/common/semaphore.h
        ${SOURCE_DIR}/debug/profiler.cpp ${SOURCE_DIR}/debug/profiler.h
        ${SOURCE_DIR}/core/macro.h
        ${SOURCE_DIR}/common/sparse_map.h
        ${SOURCE_DIR}/core/scene.cpp ${SOURCE_DIR}/core/scene.h
        ${SOURCE_DIR}/math/transform.cpp ${SOURCE_DIR}/math/transform.h
        ${SOURCE_DIR}/math/math_utils.h
        ${SOURCE_DIR}/core/components/transform_component.cpp ${SOURCE_DIR}/core/components/transform_component.h
        ${SOURCE_DIR}/core/components/mesh_renderer.cpp ${SOURCE_DIR}/core/components/mesh_renderer.h
        ${SOURCE_DIR}/core/components/camera.cpp ${SOURCE_DIR}/core/components/camera.h
        ${SOURCE_DIR}/common/int_set.cpp ${SOURCE_DIR}/common/int_set.h
        ${SOURCE_DIR}/core/renderer.cpp ${SOURCE_DIR}/core/renderer.h
        ${SOURCE_DIR}/common/rect.cpp ${SOURCE_DIR}/common/rect.h
        ${SOURCE_DIR}/renderer/render_texture.cpp ${SOURCE_DIR}/renderer/render_texture.h
        ${SOURCE_DIR}/core/resources.cpp ${SOURCE_DIR}/core/resources.h src/core/serializer.cpp src/core/serializer.h
        ${SOURCE_DIR}/common/json.hpp
        src/renderer/texture_import.cpp src/renderer/texture_import.h src/core/entity.cpp src/core/entity.h)

set(EDITOR_SRC
        ${SOURCE_DIR}/editor/editor_app.cpp ${SOURCE_DIR}/editor/editor_app.h
        ${SOURCE_DIR}/editor/gui/imgui_renderer.cpp ${SOURCE_DIR}/editor/gui/imgui_renderer.h
        ${SOURCE_DIR}/editor/gui/editor_gui.h ${SOURCE_DIR}/editor/gui/editor_gui.cpp
        ${SOURCE_DIR}/editor/gui/scene_graph_gui.cpp ${SOURCE_DIR}/editor/gui/scene_graph_gui.h
        ${SOURCE_DIR}/editor/gui/gui.h ${SOURCE_DIR}/editor/gui/gui.cpp
        ${SOURCE_DIR}/editor/gui/properties_gui.cpp ${SOURCE_DIR}/editor/gui/properties_gui.h
        ${SOURCE_DIR}/editor/gui/scene_view_gui.cpp ${SOURCE_DIR}/editor/gui/scene_view_gui.h
        ${SOURCE_DIR}/editor/gui/scene_view_gui.cpp ${SOURCE_DIR}/editor/gui/scene_view_gui.h
        ${SOURCE_DIR}/editor/gui/game_view_gui.cpp ${SOURCE_DIR}/editor/gui/game_view_gui.h
        ${SOURCE_DIR}/editor/gui/components_gui.cpp ${SOURCE_DIR}/editor/gui/components_gui.h src/editor/common/guid.cpp src/editor/common/guid.h)

set(REFLECTION_SRC
        ${SOURCE_DIR}/reflection/meta_runtime.h
        ${SOURCE_DIR}/reflection/meta_module.h ${SOURCE_DIR}/reflection/meta_module.cpp
        ${SOURCE_DIR}/reflection/meta_registry.cpp ${SOURCE_DIR}/reflection/meta_registry.h
        ${SOURCE_DIR}/reflection/type_info.cpp ${SOURCE_DIR}/reflection/type_info.h
        ${SOURCE_DIR}/reflection/type_ids.cpp ${SOURCE_DIR}/reflection/type_ids.h
        ${SOURCE_DIR}/reflection/field.cpp ${SOURCE_DIR}/reflection/field.h
        ${SOURCE_DIR}/reflection/type.cpp ${SOURCE_DIR}/reflection/type.h
        ${SOURCE_DIR}/reflection/reference.cpp ${SOURCE_DIR}/reflection/reference.h
        ${SOURCE_DIR}/reflection/method.cpp ${SOURCE_DIR}/reflection/method.h
        ${SOURCE_DIR}/reflection/object.cpp ${SOURCE_DIR}/reflection/object.h)

set(INCLUDE_DIRECTORIES
        ${SOURCE_DIR}/
        )

set(LIBRARIES
        glfw
        spdlog
        glad
        stb_image
        imgui
        )

# ----------------------------------------------------------
# ---------------------- SprintEngine ----------------------
# ----------------------------------------------------------

add_executable(SprintEngine ${SOURCE_DIR}/core/main.cpp ${ENGINE_SRC})

target_precompile_headers(SprintEngine PRIVATE ${SOURCE_DIR}/sprintpch.h)
target_include_directories(SprintEngine PRIVATE ${INCLUDE_DIRECTORIES})
target_link_libraries(SprintEngine PRIVATE ${LIBRARIES})
target_compile_definitions(SprintEngine PRIVATE GLFW_INCLUDE_NONE)

# ----------------------------------------------------------
# ---------------------- SprintEditor ----------------------
# ----------------------------------------------------------

add_executable(SprintEditor ${SOURCE_DIR}/editor/main.cpp ${ENGINE_SRC} ${REFLECTION_SRC} ${EDITOR_SRC})

set(ALL_INCLUDES $<TARGET_PROPERTY:SprintEditor,INCLUDE_DIRECTORIES>)
set(ALL_DEFINES $<TARGET_PROPERTY:SprintEditor,COMPILE_DEFINITIONS>)

add_custom_target(gen_meta_module
        COMMAND ${CMAKE_COMMAND}
        ARGS
            -DINPUT="${ENGINE_SRC}"
            -DIMPLICIT_INCLUDES="${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
            -DINCLUDES="${ALL_INCLUDES}"
            -DDEFINES="${ALL_DEFINES}"
            -DOUTPUT=${PROJECT_SOURCE_DIR}/${SOURCE_DIR}/reflection/meta_module.h
            -DGEN=$<TARGET_FILE:meta_generator>
            -P ${CMAKE_SOURCE_DIR}/cmake/RunCppParser.cmake
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        )

target_precompile_headers(SprintEditor PRIVATE ${SOURCE_DIR}/sprintpch.h)
target_include_directories(SprintEditor PRIVATE ${INCLUDE_DIRECTORIES})
target_link_libraries(SprintEditor PRIVATE ${LIBRARIES})
target_compile_definitions(SprintEditor PRIVATE GLFW_INCLUDE_NONE)

# ----------------------------------------------------------
# ------------------------ Sandbox -------------------------
# ----------------------------------------------------------

set(SANDBOX_PATH ${PROJECT_SOURCE_DIR}/sandbox/)

add_custom_target(SandboxEditor
        COMMAND SprintEditor
            ${SANDBOX_PATH}
        )

add_custom_target(Sandbox
        COMMAND SprintEngine
        WORKING_DIRECTORY "${SANDBOX_PATH}"
        )

# ----------------------------------------------------------
# ----------------------------------------------------------
# ----------------------------------------------------------

