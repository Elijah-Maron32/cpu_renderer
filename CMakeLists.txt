cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(ARM3Drenderer)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE RENDERER_HEADERS "include/*.hpp")
file(GLOB_RECURSE RENDERER_SOURCES "source/*.cpp")

add_executable(ARM3Drenderer ${RENDERER_HEADERS} ${RENDERER_SOURCES})


target_include_directories(ARM3Drenderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${SDL2_INCLUDE_DIRS}")
target_link_libraries(ARM3Drenderer PUBLIC
    ${SDL2_LIBRARIES}
)
