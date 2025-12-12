# Projects Settings
cmake_minimum_required (VERSION 3.5...4.0)
cmake_policy(VERSION 4.0)
project (ocean-gestalt)

enable_testing()
include(CTest)
# cspell: disable

add_subdirectory(src/core)

# We need to come up with a better way to specify this path
if (EMSCRIPTEN)
  target_include_directories(app-core BEFORE PRIVATE
  "/Users/gjones/emsdk/upstream/emscripten/cache/ports/contrib.glfw3/include"
  )
endif()

add_subdirectory(src/api)

if (NOT EMSCRIPTEN)
  add_subdirectory(src/rest)
  add_subdirectory(tests)
endif()

# The main executable
add_executable(ocean-gestalt
  src/main.cpp
)

target_compile_features(ocean-gestalt PRIVATE cxx_std_17)
target_compile_options(ocean-gestalt PRIVATE 
  $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Weffc++ -Wuninitialized>
  )

set_property(TARGET ocean-gestalt PROPERTY CXX_STANDARD 17)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

if (EMSCRIPTEN)
  target_link_libraries(ocean-gestalt PUBLIC "-s USE_WEBGL2=1")
  target_link_libraries(ocean-gestalt PUBLIC "-s WASM=1")

  target_link_libraries(ocean-gestalt PUBLIC "-s ASSERTIONS=1")
 
  target_link_libraries(ocean-gestalt PUBLIC
    "-s INITIAL_MEMORY=64MB"
    "-s ALLOW_MEMORY_GROWTH=1"
  )

  target_compile_options(ocean-gestalt PUBLIC "--use-port=contrib.glfw3")
  target_link_options(ocean-gestalt PUBLIC "--use-port=contrib.glfw3")

  target_compile_options(ocean-gestalt PRIVATE 
  -fexceptions
   )

  target_link_options(ocean-gestalt PRIVATE 
  -fexceptions
  -sFETCH=1
  )
 
  target_link_libraries(ocean-gestalt
    PRIVATE app-core
    PRIVATE app-api
  )

  target_link_options(ocean-gestalt PRIVATE
    "-sEXPORTED_FUNCTIONS+=['_onPageHidden','_onPageVisible']"
    "-sEXPORTED_RUNTIME_METHODS=['ccall']"
  )

  # Copy the index.html file.
  foreach(file 
   "index.html" 
   "run_webassembly.py" 
   "keypress.js"
   "manifest.json"
   )
    configure_file(src/${file} ${file})
  endforeach(file)

  file(GLOB SHADER_FILES "data/shader/webgl/*")
  foreach(file ${SHADER_FILES})
    get_filename_component(fname "${file}" NAME)
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR}/data/shader/webgl/${fname})
  endforeach()

  file(GLOB TEXTURE_FILES "data/shader/texture/*")
  foreach(file ${TEXTURE_FILES})
    get_filename_component(fname "${file}" NAME)
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR}/data/shader/texture/${fname} COPYONLY)
  endforeach()

  file(GLOB FONT_FILES "data/font/*")
  foreach(file ${FONT_FILES})
    get_filename_component(fname "${file}" NAME)
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR}/data/font/${fname} COPYONLY)
  endforeach()

  file(GLOB CONFIG_FILES "data/config/*.json")
  foreach(file ${CONFIG_FILES})
    get_filename_component(fname "${file}" NAME)
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR}/data/config/${fname})
  endforeach(file)

  foreach(file
    "github-mark-white.png"
    "favicon.ico"
    "thumbnail.jpg"
    "apple-touch-icon.png"
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
  
  add_library(civetweb STATIC
    lib/civetweb/src/civetweb.c
    lib/civetweb/src/CivetServer.cpp
  )

  target_include_directories(civetweb PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/civetweb/include
  )

  target_compile_definitions(civetweb PRIVATE
    CIVETWEB_ENABLE_CXX
    NO_SSL
  )

  target_link_libraries(ocean-gestalt
    PRIVATE app-core
    PRIVATE app-rest
    PRIVATE app-api
  )

  target_include_directories(ocean-gestalt 
    PRIVATE lib/glew/include
    PRIVATE lib/glfw/include
  )
endif()

add_subdirectory(lib/glm)
target_link_libraries(ocean-gestalt PRIVATE glm)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(ocean-gestalt PRIVATE nlohmann_json::nlohmann_json)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/asset.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/core/asset.hpp
)

target_include_directories(ocean-gestalt
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/core/include
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src/core/
)

