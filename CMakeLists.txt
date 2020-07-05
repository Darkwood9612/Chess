cmake_minimum_required(VERSION 3.17)

project(testTask CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_REVISION 3d2a7ca33e9997051ea221c30ccd4835989879ad)

# Download vcpkg dependencies
include(cmake/pmm.cmake)
set(ports sdl2:x64-windows sdl2-ttf:x64-windows sdl2-image:x64-windows imgui[bindings]:x64-windows opengl:x64-windows)

pmm(VCPKG
  REVISION ${VCPKG_REVISION}
  REQUIRES ${ports}
)

file(GLOB src "${CMAKE_CURRENT_SOURCE_DIR}/src/*" "${CMAKE_CURRENT_SOURCE_DIR}/.clang-format")
add_executable(test_task ${src})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT test_task)

find_package(SDL2 REQUIRED)
target_link_libraries(test_task PRIVATE SDL2::SDL2-static)

find_package(SDL2-ttf REQUIRED)
target_link_libraries(test_task PRIVATE SDL2::SDL2_ttf)

find_package(SDL2-image REQUIRED)
target_link_libraries(test_task PRIVATE SDL2::SDL2_image)

find_package(imgui REQUIRED)
target_link_libraries(test_task PRIVATE imgui::imgui)

find_package(OpenGL REQUIRED)
target_link_libraries(test_task PRIVATE OpenGL::GL)

find_path(IMGUI_BINDINGS_INCLUDE_DIR NAMES imgui_impl_sdl.h PATH_SUFFIXES bindings)
target_include_directories(test_task PUBLIC ${IMGUI_BINDINGS_INCLUDE_DIR})

add_custom_target(copy_files ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/image $<TARGET_FILE_DIR:test_task>/image
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/engine $<TARGET_FILE_DIR:test_task>/engine
)