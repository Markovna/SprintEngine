cmake_minimum_required(VERSION 3.16)
project(meta_generator)

set(CMAKE_CXX_STANDARD 17)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package(LLVM 6.0 REQUIRED)

set(CLANG_LIBS clang)
set(HEADER_FILES cpp_parser.hpp inja.hpp nlohmann/json.hpp)
set(SOURCE_FILES main.cpp)

add_executable(meta_generator ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(meta_generator PRIVATE ./)

# add LLVM includes
target_include_directories(meta_generator PUBLIC ${LLVM_INCLUDE_DIRS})
target_link_directories(meta_generator PUBLIC ${LLVM_LIBRARY_DIRS})
target_link_libraries(meta_generator PUBLIC ${CLANG_LIBS})