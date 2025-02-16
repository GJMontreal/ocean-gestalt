# Projects Settings
cmake_minimum_required (VERSION 3.5)
project (ocean-gestalt)

# The main executable
add_executable(ocean-gestalt
  src/Application.cpp
  src/OceanGestalt.cpp
  src/glError.cpp
  src/main.cpp
  src/Shader.hpp
  src/Shader.cpp
  src/Wave.cpp
  src/Mesh.cpp
  src/Model.cpp
  src/Ocean.cpp
  src/Configuration.cpp
  src/Serialization.cpp
  src/Camera.cpp
  src/WaveUI.cpp
  src/WaveGenerator.cpp
  src/Light.cpp
  src/Moveable.cpp
  src/KeyExecutable.cpp
)

set_property(TARGET ocean-gestalt PROPERTY CXX_STANDARD 17)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)


if (EMSCRIPTEN)
  target_link_libraries(ocean-gestalt PUBLIC "-s MIN_WEBGL_VERSION=2")
  target_link_libraries(ocean-gestalt PUBLIC "-s WASM=1")

  target_compile_options(ocean-gestalt PUBLIC "--use-port=contrib.glfw3")
  target_link_options(ocean-gestalt PUBLIC "--use-port=contrib.glfw3")

  target_compile_options(ocean-gestalt PRIVATE -fexceptions )
  target_link_options(ocean-gestalt PRIVATE -fexceptions)


  # Copy the index.html file.
  foreach(file 
   "index.html" 
   "run_webassembly.py" 
   "keypress.js"
   "manifest.json"
   )
    configure_file(src/${file} ${file})
  endforeach(file)

  foreach(file 
   "data/shader/webgl/shader.frag"
   "data/shader/webgl/simple.frag" 
   "data/shader/webgl/gerstner.vert"
   "data/config/environment.json"
   "data/config/shader.json"
   "data/config/generator.json"
  )
  
    configure_file(${file} ${file})
  endforeach(file)

  foreach(file
    "github-mark-white.png"
    "favicon.ico"
    "thumbnail.jpg"
  )
  configure_file(${file} ${file} COPYONLY)
  endforeach(file)

  # Allow the /data directory to be fetched by the web browser.
  set_property(TARGET ocean-gestalt APPEND_STRING PROPERTY LINK_FLAGS
    "--preload-file ${CMAKE_CURRENT_BINARY_DIR}/data/@/"
  )


else() # Not Web

  add_definitions(-DGLEW_STATIC)
  add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
  add_subdirectory(lib/glew EXCLUDE_FROM_ALL)

  target_link_libraries(ocean-gestalt
  PRIVATE glfw
  PRIVATE libglew_static
  )
endif()

add_subdirectory(lib/glm)
target_link_libraries(ocean-gestalt PRIVATE glm)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(ocean-gestalt PRIVATE nlohmann_json::nlohmann_json)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/asset.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/asset.hpp
)
target_include_directories(ocean-gestalt
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
)
