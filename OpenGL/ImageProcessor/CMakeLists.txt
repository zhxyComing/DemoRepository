cmake_minimum_required(VERSION 3.17)
project(ImageProcessor)

set(CMAKE_CXX_STANDARD 14)

# 检查环境变量 需要在CLion-Preferences-CMake中配置 环境变量中均是我们将要使用的OpenGL库
if (NOT DEFINED ENV{GLFW_HOME})
    message(FATAL_ERROR "found no env named GLFW_HOME")
endif ()
if (NOT DEFINED ENV{GLAD_HOME})
    message(FATAL_ERROR "found no env named GLAD_HOME")
endif ()
if (NOT DEFINED ENV{GLM_HOME})
    message(FATAL_ERROR "found no env named GLM_HOME")
endif ()
#if (NOT DEFINED ENV{SOIL_HOME})
#    message(FATAL_ERROR "found no env named SOIL_HOME")
#endif ()
#if (NOT DEFINED ENV{FREE_TYPE_HOME})
#    message(FATAL_ERROR "found no env named FREE_TYPE_HOME")
#endif ()

set(GLFW_HOME $ENV{GLFW_HOME})
set(GLAD_HOME $ENV{GLAD_HOME})
set(GLM_HOME $ENV{GLM_HOME})
#set(SOIL_HOME $ENV{SOIL_HOME})
#set(ASSIMP_HOME $ENV{ASSIMP_HOME})
#set(FREE_TYPE_HOME $ENV{FREE_TYPE_HOME})

include_directories("${GLFW_HOME}/include")
include_directories("${GLAD_HOME}/include")
include_directories("${GLM_HOME}/glm")  # 数学处理库
#include_directories("${SOIL_HOME}/src")
#include_directories("${ASSIMP_HOME}/include")
#include_directories("${FREE_TYPE_HOME}/include")

# GLFW3 OpenGL 窗口库
add_library(glfw SHARED IMPORTED)
SET_TARGET_PROPERTIES(glfw PROPERTIES IMPORTED_LOCATION "${GLFW_HOME}/lib-macos/libglfw.3.dylib")

# GLAD OpenGL API 库
add_library(glad SHARED "${GLAD_HOME}/src/glad.c")

# SOIL 图像处理库，Demo 中改用 stb_image
#add_library(SOIL_Dylib SHARED IMPORTED) # SOIL.2
#SET_TARGET_PROPERTIES(SOIL_Dylib PROPERTIES IMPORTED_LOCATION "${SOIL_HOME}/build/libSOIL.dylib")

# assimp 模型文件处理库
#add_library(ASSIMP_Dylib SHARED IMPORTED)
#SET_TARGET_PROPERTIES(ASSIMP_Dylib PROPERTIES IMPORTED_LOCATION "${ASSIMP_HOME}/build/code/libassimp.dylib")

# FreeType 字体库
#add_library(FREETYPE_Dylib SHARED IMPORTED)
#SET_TARGET_PROPERTIES(FREETYPE_Dylib PROPERTIES IMPORTED_LOCATION "/usr/local/freetype/lib/libfreetype.dylib")

file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
add_executable(ImageProcessor src/main.cpp ${SRC_LIST})

#target_link_libraries(ImageProcessor
#        SOIL_Dylib ASSIMP_Dylib glfw glad FREETYPE_Dylib )

target_link_libraries(ImageProcessor glfw glad)

