# core/CMakeLists.txt


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
)

set_property(TARGET app-core PROPERTY CXX_STANDARD 17)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glfw ${CMAKE_BINARY_DIR}/lib/glfw)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glew ${CMAKE_BINARY_DIR}/lib/glew)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glm ${CMAKE_BINARY_DIR}/lib/glm)
target_compile_definitions(app-core PRIVATE GLEW_STATIC)

target_link_libraries(app-core
  PRIVATE glfw
  PRIVATE libglew_static
  PRIVATE glm
  PRIVATE nlohmann_json::nlohmann_json
)

target_include_directories(app-core 
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/src/core
)
