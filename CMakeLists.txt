cmake_minimum_required(VERSION 3.30)
project(mandelbrot C)
set(CMAKE_C_STANDARD 11)



add_executable(mandelbrot src/main.c src/glad.c)

include_directories(include)
include_directories(lib)

add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mandelbrot glfw)