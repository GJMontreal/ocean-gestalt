#include "OceanGestalt.hpp"

using Action = std::function<void(OceanGestalt*)>;

std::vector<std::pair<int, Action>> KeyBindings::getKeyBindings() const {


  const std::vector<std::pair<int, Action>> keyBindings = {
    {GLFW_KEY_G, [](OceanGestalt* obj) { obj->loadUniforms(); }},
    {GLFW_KEY_R, [](OceanGestalt* obj) { obj->generateUniforms(); }},
    {GLFW_KEY_C, [](OceanGestalt* obj) {obj->selectNextElement(); }},
    {GLFW_KEY_N, [](OceanGestalt* obj) { obj->toggleNormalDisplay(); }},
    {GLFW_KEY_P, [](OceanGestalt* obj) { obj->toggleSimulation(); }},
    {GLFW_KEY_M, [](OceanGestalt* obj) { obj->toggleMesh(); }},
    {GLFW_KEY_J, [](OceanGestalt* obj) { obj->toggleDrawTriangles(); }},
    {GLFW_KEY_K, [](OceanGestalt* obj) { obj->toggleDrawLines(); }},
    {GLFW_KEY_L, [](OceanGestalt* obj) { obj->toggleWireframe(); }},
    {GLFW_KEY_B, [](OceanGestalt* obj) { obj->dumpUniforms(); }},
    {GLFW_KEY_O, [](OceanGestalt* obj) { obj->configuration->save(CONFIGURATION_DIR "/output.json"); }},
    {GLFW_KEY_T, [](OceanGestalt* obj) { obj->toggleFloatingCamera();}},
#ifndef __EMSCRIPTEN__
    {GLFW_KEY_F, [](OceanGestalt* obj) { obj->toggleFullscreen(obj->getWindow()); }},
#endif
  };
  return keyBindings;
}