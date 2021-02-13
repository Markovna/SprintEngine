cmake_minimum_required(VERSION 3.16)
project(cpp_parser)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM 6.0 REQUIRED)

set(CLANG_LIBS clang)
set(HEADER_FILES cpp_parser.hpp inja.hpp)
set(SOURCE_FILES main.cpp)

add_executable(cpp_parser ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(cpp_parser PRIVATE ./)

# add LLVM includes
target_include_directories(cpp_parser PUBLIC ${LLVM_INCLUDE_DIRS})
target_link_directories(cpp_parser PUBLIC ${LLVM_LIBRARY_DIRS})
target_link_libraries(cpp_parser PUBLIC ${CLANG_LIBS})