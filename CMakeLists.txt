cmake_minimum_required(VERSION 3.16)
project(SprintEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/spdlog)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glad)

add_executable(SprintEngine core/main.cpp core/Log.cpp core/Log.h core/Event.cpp core/Event.h core/Application.cpp core/Application.h)

target_link_libraries(SprintEngine PRIVATE glfw)
target_link_libraries(SprintEngine PRIVATE spdlog)
target_link_libraries(SprintEngine PRIVATE glad)
