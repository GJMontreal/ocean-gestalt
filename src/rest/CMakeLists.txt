# rest/CMakeLists.txt
cmake_minimum_required (VERSION 3.5...4.0)
cmake_policy(VERSION 4.0)

# cspell: disable

add_library(app-rest STATIC
  RestServer.cpp
  PauseHandler.cpp
  PathHandler.cpp
  UniformHandler.cpp
)

set_property(TARGET app-rest PROPERTY CXX_STANDARD 17)
target_compile_features(app-rest PRIVATE cxx_std_17)
target_compile_options(app-rest PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

target_include_directories(app-rest
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR}/src/api/
)

target_link_libraries(app-rest
  PUBLIC civetweb
  PRIVATE nlohmann_json::nlohmann_json
)
