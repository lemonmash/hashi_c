cmake_minimum_required(VERSION 3.27.1)
set(PROJECT_NAME hashi_c)
# set(CMAKE_C_COMPILER Ninja)
project(${PROJECT_NAME} C)
add_executable(${PROJECT_NAME} main.c)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_WAYLAND ON)

add_subdirectory(glad)
add_subdirectory(glfw)

add_library(linmath 
    linmath.h/linmath.h
    )
set_target_properties(linmath PROPERTIES LINKER_LANGUAGE C)
target_include_directories(linmath PRIVATE "linmath.h")

target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/glfw/include"
    "${PROJECT_SOURCE_DIR}/glad/include"
    "${PROJECT_SOURCE_DIR}/linmath.h"
    )

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    glad
    linmath
    )
