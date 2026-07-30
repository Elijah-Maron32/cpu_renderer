cmake_minimum_required(VERSION 3.10.0)
project(ARM3Drenderer VERSION 0.1.0 LANGUAGES C CXX)

find_package(SDL2 REQUIRED)

add_executable(ARM3Drenderer main.cpp)

target_link_libraries(ARM3Drenderer PUBLIC
    ${SDL2_LIBRARIES}
)
