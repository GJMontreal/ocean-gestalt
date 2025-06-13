cmake_minimum_required (VERSION 3.5...4.0)
cmake_policy(VERSION 4.0)

# cspell: disable

add_library(app-api STATIC
  clangd_headers.cpp
  OceanApi.cpp
  PathHandler.cpp
  UniformState.cpp
)

set_property(TARGET app-api PROPERTY CXX_STANDARD 17)
target_compile_features(app-api PRIVATE cxx_std_17)
target_compile_options(app-api PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

target_link_libraries(app-api
  PRIVATE libglew_static
  PRIVATE glm
)

target_include_directories(app-api
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../core/include
  PUBLIC ${CMAKE_BINARY_DIR}/src/core
)
