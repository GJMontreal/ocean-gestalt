# Projects Settings
cmake_minimum_required (VERSION 3.1)
project (ocean-gestalt)

# The main executable
add_executable(ocean-gestalt
  src/Application.cpp
  src/Application.cpp
  src/MyApplication.cpp
  src/MyApplication.cpp
  src/glError.hpp
  src/glError.cpp
  src/main.cpp
  src/Shader.hpp
  src/Shader.cpp
  src/Wave.cpp
  src/Mesh.cpp
  src/Model.cpp
  src/Ocean.cpp
)

set_property(TARGET ocean-gestalt PROPERTY CXX_STANDARD 11)
target_compile_options(ocean-gestalt PRIVATE -Wall)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)

target_link_libraries(ocean-gestalt
  PRIVATE glfw
  PRIVATE libglew_static
  PRIVATE glm
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/asset.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/asset.hpp
)
target_include_directories(ocean-gestalt
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
)
