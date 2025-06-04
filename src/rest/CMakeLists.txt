# rest/CMakeLists.txt
cmake_minimum_required (VERSION 3.5)
cmake_policy(VERSION 3.5)

add_library(app-rest STATIC
  RestServer.cpp
  PauseHandler.cpp
)

set_property(TARGET app-rest PROPERTY CXX_STANDARD 17)

target_include_directories(app-rest
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(app-rest
  PUBLIC civetweb
  PRIVATE nlohmann_json::nlohmann_json
)
