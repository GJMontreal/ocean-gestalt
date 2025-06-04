# core/CMakeLists.txt
cmake_minimum_required (VERSION 3.5)
cmake_policy(VERSION 3.5)

add_library(rest-api STATIC
  ${CMAKE_SOURCE_DIR}/lib/civetweb/src/civetweb.c
  # REST-API.cpp
)

set_property(TARGET app-core PROPERTY CXX_STANDARD 17)

target_include_directories(rest-api PUBLIC
 ${CMAKE_SOURCE_DIR}/lib/civetweb/include
)

target_compile_definitions(rest-api PRIVATE
  CIVETWEB_ENABLE_CXX
  NO_SSL
)
