cmake_minimum_required (VERSION 3.5)
cmake_policy(VERSION 3.5)

add_library(app-api STATIC
  OceanApi.cpp
)

set_property(TARGET app-api PROPERTY CXX_STANDARD 17)

target_link_libraries(app-api
  PRIVATE libglew_static
  PRIVATE glm
)

target_include_directories(app-api
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../core/include
)
