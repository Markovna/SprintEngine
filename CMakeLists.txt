cmake_minimum_required(VERSION 3.16)
project(SprintEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(thirdparty/spdlog spdlog)
add_subdirectory(thirdparty/glfw glfw)
add_subdirectory(thirdparty/glad glad)
add_subdirectory(thirdparty/stb_image stb_image)
add_subdirectory(thirdparty/imgui imgui)

add_subdirectory(reflection/generator)

set(ENGINE_SRC
        core/log.cpp core/log.h
        core/window_event.h
        core/application.cpp core/application.h
        core/window.cpp core/window.h
        renderer/shader.cpp renderer/shader.h
        math/vector.cpp math/vector.h
        renderer/texture.cpp renderer/texture.h
        math/matrix.cpp math/matrix.h
        math/quat.cpp math/quat.h
        core/key_codes.h
        core/mouse_codes.h
        common/color.cpp common/color.h
        common/signals/delegate.h
        common/signals/event.cpp common/signals/event.h
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
        common/rect_int.cpp common/rect_int.h
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
        core/components/transform_component.cpp core/components/transform_component.h
        sprintpch.h
        core/components/mesh_renderer.cpp core/components/mesh_renderer.h
        core/components/camera.cpp core/components/camera.h
        common/int_set.cpp common/int_set.h
        core/renderer.cpp core/renderer.h
        common/rect.cpp common/rect.h renderer/render_texture.cpp renderer/render_texture.h)

set(EDITOR_SRC
        editor/editor_app.cpp editor/editor_app.h
        editor/imgui_renderer.cpp editor/imgui_renderer.h
        editor/editor_gui.h editor/editor_gui.cpp
        editor/scene_graph_gui.cpp editor/scene_graph_gui.h
        editor/gui.h editor/gui.cpp
        editor/properties_gui.cpp editor/properties_gui.h
        editor/scene_view_gui.cpp editor/scene_view_gui.h
        editor/game_view_gui.cpp editor/game_view_gui.h
        editor/components_gui.cpp editor/components_gui.h)

set(REFLECTION_SRC
        reflection/meta_module.h
        reflection/meta_module.cpp
        reflection/meta_registry.cpp reflection/meta_registry.h reflection/type_info.cpp reflection/type_info.h reflection/type_ids.cpp reflection/type_ids.h reflection/field.cpp reflection/field.h reflection/type.cpp reflection/type.h reflection/reference.cpp reflection/reference.h reflection/meta_runtime.h reflection/method.cpp reflection/method.h reflection/object.cpp reflection/object.h)

set(INCLUDE_DIRECTORIES
        core/
        renderer/
        math/
        common/
        reflection/
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

add_executable(SprintEngine core/main.cpp ${ENGINE_SRC})

target_precompile_headers(SprintEngine PRIVATE sprintpch.h)
target_include_directories(SprintEngine PRIVATE ${INCLUDE_DIRECTORIES})
target_link_libraries(SprintEngine PRIVATE ${LIBRARIES})
target_compile_definitions(SprintEngine PRIVATE GLFW_INCLUDE_NONE)
#target_sources(SprintEngine PUBLIC ${ENGINE_SRC})

# ----------------------------------------------------------
# ---------------------- SprintEditor ----------------------
# ----------------------------------------------------------

add_executable(SprintEditor editor/main.cpp ${ENGINE_SRC} ${REFLECTION_SRC} ${EDITOR_SRC})

set(ALL_INCLUDES $<TARGET_PROPERTY:SprintEditor,INCLUDE_DIRECTORIES>)
set(ALL_DEFINES $<TARGET_PROPERTY:SprintEditor,COMPILE_DEFINITIONS>)

add_custom_target(gen_meta_module
        COMMAND ${CMAKE_COMMAND}
        ARGS
            -DINPUT="${ENGINE_SRC}"
            -DIMPLICIT_INCLUDES="${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
            -DINCLUDES="${ALL_INCLUDES}"
            -DDEFINES="${ALL_DEFINES}"
            -DOUTPUT=${PROJECT_SOURCE_DIR}/reflection/meta_module.h
            -DGEN=$<TARGET_FILE:meta_generator>
            -P ${CMAKE_SOURCE_DIR}/cmake/RunCppParser.cmake
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        )

set(TEST_META_SRC core/components/transform_component.h)
add_custom_target(test_meta_module
        COMMAND ${CMAKE_COMMAND}
        ARGS
            -DINPUT="${TEST_META_SRC}"
            -DIMPLICIT_INCLUDES="${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
            -DINCLUDES="${ALL_INCLUDES}"
            -DDEFINES="${ALL_DEFINES}"
            -DOUTPUT=${PROJECT_SOURCE_DIR}/reflection/meta_module_test.h
            -DGEN=$<TARGET_FILE:meta_generator>
            -P ${CMAKE_SOURCE_DIR}/cmake/RunCppParser.cmake
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        )

#add_custom_command(
#        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_OUT_HPP}
#        COMMAND ${CMAKE_COMMAND}
#        ARGS
#            -DINPUT=${PARSER_INPUT}
#            -DIMPLICIT_INCLUDES="${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
#            -DINCLUDES="${ALL_INCLUDES}"
#            -DDEFINES="${ALL_DEFINES}"
#            -DGEN=$<TARGET_FILE:meta_generator>
#            -P ${CMAKE_SOURCE_DIR}/cmake/RunCppParser.cmake
#        WORKING_DIRECTORY "${PARSER_WORKING_DIRECTORY}"
#)

target_precompile_headers(SprintEditor PRIVATE sprintpch.h)
target_include_directories(SprintEditor PRIVATE ${INCLUDE_DIRECTORIES})
target_link_libraries(SprintEditor PRIVATE ${LIBRARIES})
target_compile_definitions(SprintEditor PRIVATE GLFW_INCLUDE_NONE)
#target_sources(SprintEditor PUBLIC ${EDITOR_SRC} ${ENGINE_SRC} ${REFLECTION_SRC})

