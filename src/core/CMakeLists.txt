# core/CMakeLists.txt
cmake_minimum_required (VERSION 3.5...4.0)
cmake_policy(VERSION 4.0)

# cspell: disable

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
  SurfAudio.cpp
  FoamRenderPass.cpp
  Sphere.cpp
  Identifier.cpp
  Drawable.cpp
  Buoy.cpp
  GerstnerWave.cpp
  TextRenderer.cpp
  Skybox.cpp
  HeightMapGenerator.cpp
  KeyBindings.cpp
)

set_property(TARGET app-core PROPERTY CXX_STANDARD 17)
target_compile_features(app-core PRIVATE cxx_std_17)
target_compile_options(app-core PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

target_link_libraries(app-core
  PRIVATE glfw
  PRIVATE glm
  PRIVATE nlohmann_json::nlohmann_json
  PRIVATE app-api
)

if (NOT EMSCRIPTEN)
  target_link_libraries(app-core
  PRIVATE libglew_static
  "-framework CoreAudio"
  "-framework AudioToolbox"
  "-framework AVFoundation"
  )
endif()

target_include_directories(app-core 
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_BINARY_DIR}/src/core
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  PUBLIC ${CMAKE_SOURCE_DIR}/lib/stb
  PUBLIC ${CMAKE_SOURCE_DIR}/lib/miniaudio
)
