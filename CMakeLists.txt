cmake_minimum_required(VERSION 3.16)
project(SprintEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/spdlog)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glfw)

add_executable(SprintEngine main.cpp Log.cpp Log.h)

target_link_libraries(SprintEngine glfw)
target_link_libraries(SprintEngine spdlog)
