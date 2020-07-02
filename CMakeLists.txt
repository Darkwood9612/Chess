cmake_minimum_required(VERSION 3.17)

project(testTask CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_REVISION f29a191d0afccc3ed6f481283d6d15e0186096ae)

# Download vcpkg dependencies
include(cmake/pmm.cmake)
set(ports sdl2:x64-windows sdl2-ttf:x64-windows sdl2-image:x64-windows imgui:x64-windows)

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


add_custom_target(copy_files ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/image $<TARGET_FILE_DIR:test_task>/image
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/font $<TARGET_FILE_DIR:test_task>/font
)