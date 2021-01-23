# Install script for directory: /Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibassimp5.0.1-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/assimp-5.0" TYPE FILE FILES
    "/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/assimp-config.cmake"
    "/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/assimp-config-version.cmake"
    "/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/assimpTargets.cmake"
    "/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/assimpTargets-debug.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibassimp5.0.1-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/assimp.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/contrib/cmake_install.cmake")
  include("/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/code/cmake_install.cmake")
  include("/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/tools/assimp_cmd/cmake_install.cmake")
  include("/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/test/cmake_install.cmake")

endif()

