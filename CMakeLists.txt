cmake_minimum_required(VERSION 3.16)
project(SprintEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

add_subdirectory(thirdparty/spdlog spdlog)
add_subdirectory(thirdparty/glfw glfw)
add_subdirectory(thirdparty/glad glad)
add_subdirectory(thirdparty/stb_image stb_image)
add_subdirectory(thirdparty/imgui imgui)

add_subdirectory(reflection/parser)

set(SPRINT_SRCS
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
        core/components/transform_component.cpp core/components/transform_component.h sprintpch.h)

set(EDITOR_SRCS ${SPRINT_SRCS}
        editor/editor_app.cpp editor/editor_app.h
        editor/imgui_renderer.cpp editor/imgui_renderer.h
        editor/editor_gui.h editor/editor_gui.cpp
        editor/scene_graph_gui.cpp editor/scene_graph_gui.h
        editor/gui.h editor/gui.cpp
        editor/properties_gui.cpp editor/properties_gui.h
        reflection/runtime.cpp reflection/runtime.h
        )

set(INCLUDE_DIRECTORIES core/ renderer/ math/ common/)
set(LIBRARIES glfw spdlog glad stb_image imgui)

# ------------------------------------
# ----------- SprintEngine -----------
# ------------------------------------

add_executable(SprintEngine)

target_precompile_headers(SprintEngine PRIVATE sprintpch.h)
target_include_directories(SprintEngine PRIVATE ${INCLUDE_DIRECTORIES})
target_link_libraries(SprintEngine PRIVATE ${LIBRARIES})
target_compile_definitions(SprintEngine PRIVATE GLFW_INCLUDE_NONE)
target_sources(SprintEngine PUBLIC ${SPRINT_SRCS} core/main.cpp)

# ------------------------------------
# ----------- SprintEditor -----------
# ------------------------------------

add_executable(SprintEditor)

set(PARSER_INPUT renderer_api_gl.h)
set(PARSER_OUT_HPP dummy.hpp)
set(PARSER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/renderer")

set(ALL_INCLUDES $<TARGET_PROPERTY:SprintEditor,INCLUDE_DIRECTORIES>)
set(ALL_DEFINES $<TARGET_PROPERTY:SprintEditor,COMPILE_DEFINITIONS>)
#set(ALL_PCH $<TARGET_PROPERTY:SprintEditor,PRECOMPILE_HEADERS>)

add_custom_target(test_cpp_parse
        COMMAND ${CMAKE_COMMAND}
        ARGS
            -DINPUT=gfx_details.h
            -DIMPLICIT_INCLUDES="${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
            -DINCLUDES="${ALL_INCLUDES}"
            -DDEFINES="${ALL_DEFINES}"
            -DGEN=$<TARGET_FILE:cpp_parser>
            -P ${CMAKE_SOURCE_DIR}/cmake/RunCppParser.cmake
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/renderer"
        )

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_OUT_HPP}
        COMMAND ${CMAKE_COMMAND}
        ARGS
            -DINPUT=${PARSER_INPUT}
            -DIMPLICIT_INCLUDES="${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
            -DINCLUDES="${ALL_INCLUDES}"
            -DDEFINES="${ALL_DEFINES}"
            -DGEN=$<TARGET_FILE:cpp_parser>
            -P ${CMAKE_SOURCE_DIR}/cmake/RunCppParser.cmake
        WORKING_DIRECTORY "${PARSER_WORKING_DIRECTORY}"
)

target_precompile_headers(SprintEditor PRIVATE sprintpch.h)
target_include_directories(SprintEditor PRIVATE ${INCLUDE_DIRECTORIES})
target_link_libraries(SprintEditor PRIVATE ${LIBRARIES})
target_compile_definitions(SprintEditor PRIVATE GLFW_INCLUDE_NONE)
target_sources(SprintEditor PUBLIC ${EDITOR_SRCS} editor/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_OUT_HPP})

#----------------

# TODO
#add_custom_command(TARGET SprintEditor PRE_BUILD
#        DEPENDS "${CMAKE_CURRENT_LIST_DIR}/reflection/parser/main.cpp"
#        COMMAND $<TARGET_FILE:cpp_parser>
#        --sources "reflection/parser/test.hpp"
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#        )

