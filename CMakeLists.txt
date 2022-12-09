cmake_minimum_required(VERSION 3.24)
project(Ray_Marching)

set(CMAKE_CXX_STANDARD 23)

add_executable(Ray_Marching external/glad/src/glad.c src/main.cpp)

include_directories(include)
include_directories(include)
include_directories(external/glad/include)
include_directories(external/glfw/include)
include_directories(external/glm)
include_directories(external/stbi)

target_link_libraries(Ray_Marching LINK_PUBLIC dl)
target_link_libraries(Ray_Marching LINK_PUBLIC glfw)
