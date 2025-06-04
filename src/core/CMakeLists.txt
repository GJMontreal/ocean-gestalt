# core/CMakeLists.txt
cmake_minimum_required (VERSION 3.5)
cmake_policy(VERSION 3.5)

add_library(app-core STATIC
  Application.cpp
  OceanGestalt.cpp
  glError.cpp
  Shader.cpp
  Wave.cpp
  Mesh.cpp
  Model.cpp
  Ocean.cpp
  Configuration.cpp
  Serialization.cpp
  Camera.cpp
  WaveUI.cpp
  WaveGenerator.cpp
  Light.cpp
  Moveable.cpp
  KeyExecutable.cpp
  OceanApi.cpp
)

set_property(TARGET app-core PROPERTY CXX_STANDARD 17)

target_link_libraries(app-core
  PRIVATE glfw
  PRIVATE libglew_static
  PRIVATE glm
  PRIVATE nlohmann_json::nlohmann_json
)

target_include_directories(app-core 
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_BINARY_DIR}/src/core
  # PRIVATE ${CMAKE_SOURCE_DIR}/lib/glew/include
)
