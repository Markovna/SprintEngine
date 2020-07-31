cmake_minimum_required(VERSION 3.16)
project(SprintEngine)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(SprintEngine main.cpp)

target_link_libraries(SprintEngine ${SDL2_LIBRARIES})
# target_link_libraries(SprintEngine ${Boost_LIBRARIES})
