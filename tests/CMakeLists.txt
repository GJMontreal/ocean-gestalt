cmake_minimum_required (VERSION 3.5...4.0)
cmake_policy(VERSION 4.0)

# Unit test executable
add_executable(unit_tests
  test_main.cpp
  test_uniformhandler.cpp
  test_pathHandler.cpp
  test_uniformAnimator.cpp
  test_uniformValue.cpp
  test_wave.cpp
  test_restPathHandler.cpp
  test_patternMatch.cpp
  test_prop_spin.cpp
)

set_property(TARGET unit_tests PROPERTY CXX_STANDARD 17)
target_compile_features(unit_tests PRIVATE cxx_std_17)
target_compile_options(unit_tests PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

target_include_directories(unit_tests PRIVATE 
  ${CMAKE_SOURCE_DIR}/src/api
  ${CMAKE_SOURCE_DIR}/src/core/include
  ${CMAKE_SOURCE_DIR}/lib/doctest
)

target_link_libraries(unit_tests 
PRIVATE app-rest
PRIVATE app-api
PRIVATE app-core
PRIVATE glm
PRIVATE nlohmann_json::nlohmann_json)


add_test(NAME unit_tests COMMAND unit_tests)
