cmake_minimum_required(VERSION 3.5)

project(demo)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(
  ${GLFW_INCLUDE_DIRS}
  ${GLEW_INCLUDE_PATH}
  ${GLM_INCLUDE_PATH}
)
file(
  CREATE_LINK "${PROJECT_SOURCE_DIR}/src/images"
  "${CMAKE_CURRENT_BINARY_DIR}/images" SYMBOLIC
)
file(
  CREATE_LINK "${PROJECT_SOURCE_DIR}/src/shaders"
  "${CMAKE_CURRENT_BINARY_DIR}/shaders" SYMBOLIC
)
set(GCC_COVERAGE_LINK_FLAGS "-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++14")
set(source_dir "${PROJECT_SOURCE_DIR}/src/")
file(GLOB source_files "${source_dir}/*.cpp" "${source_dir}/*.c" "${source_dir}/imgui/*.cpp" "${source_dir}/imgui/*.h")

add_executable(demo ${source_files})

target_link_libraries(demo ${GLFW_LIBRARIES})
