
cmake_minimum_required(VERSION 2.8.9)
project (Mandelbrot)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Mandelbrot Main.cpp)

target_link_libraries(Mandelbrot ${SDL2_LIBRARIES})


