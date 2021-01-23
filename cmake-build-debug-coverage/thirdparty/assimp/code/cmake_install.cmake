# Install script for directory: /Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/lib/libassimpd.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/anim.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/aabb.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/ai_assert.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/camera.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/color4.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/color4.inl"
    "/Users/kamila/CLionProjects/SprintEngine/cmake-build-debug-coverage/thirdparty/assimp/code/../include/assimp/config.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/ColladaMetaData.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/commonMetaData.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/defs.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Defines.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/cfileio.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/light.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/material.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/material.inl"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/matrix3x3.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/matrix3x3.inl"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/matrix4x4.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/matrix4x4.inl"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/mesh.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/pbrmaterial.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/postprocess.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/quaternion.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/quaternion.inl"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/scene.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/metadata.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/texture.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/types.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/vector2.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/vector2.inl"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/vector3.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/vector3.inl"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/version.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/cimport.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/importerdesc.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Importer.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/IOStream.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/IOSystem.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Logger.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/LogStream.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/NullLogger.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/cexport.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Exporter.hpp"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/DefaultIOStream.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/DefaultIOSystem.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/SceneCombiner.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/fast_atof.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/qnan.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/BaseImporter.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Hash.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/ParsingUtils.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/StreamReader.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/StreamWriter.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/StringComparison.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/StringUtils.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/SGSpatialSort.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/GenericProperty.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/SpatialSort.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/SmallVector.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/SmoothingGroups.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/SmoothingGroups.inl"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/StandardShapes.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/RemoveComments.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Subdivision.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Vertex.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/LineSplitter.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/TinyFormatter.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Profiler.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/LogAux.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Bitmap.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/XMLTools.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/IOStreamBuffer.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/CreateAnimMesh.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/irrXMLWrapper.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/BlobIOSystem.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/MathFunctions.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Exceptional.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/ByteSwapper.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/Users/kamila/CLionProjects/SprintEngine/thirdparty/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

