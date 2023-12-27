cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

include(cmake/Vcpkg.cmake) # needs to happen before project()

project(
    "chess-with-ai"
    VERSION 2.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB PRJ_HEADERS
    src/*.h
)

file(GLOB PRJ_SOURCES
    src/*.cpp
)

# add library targets here
set(PRJ_LIBRARIES 
    fmt::fmt
	SDL2::SDL2
	SDL2_image::SDL2_image
	imgui::imgui
    OpenGL::GL
)

# add dependency find_package calls and similar here
find_package(fmt CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} ${PRJ_HEADERS} ${PRJ_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PRJ_LIBRARIES})
target_compile_features(chess-with-ai PRIVATE cxx_std_17)

