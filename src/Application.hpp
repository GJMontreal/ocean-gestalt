/**
 * Application.cpp
 * Contributors:
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */

#ifndef OPENGL_CMAKE_SKELETON_APPLICATION_HPP
#define OPENGL_CMAKE_SKELETON_APPLICATION_HPP

#include "Camera.hpp"
#include <string>
#include <map>
#include <memory>
#include <GLFW/glfw3.h>

struct GLFWwindow;

/// Application class:
/// * init OpenGL
/// * provide:
///   * getWidth()
///   * getHeight()
///   * getFrameDeltaTime()
///   * getWindowRatio()
///   * windowDimensionChanged()
/// * let the user define the "loop" function.
class Application {
 public:
  Application();
  virtual ~Application() = default;

  static Application& getInstance();

  // get the window id
  GLFWwindow* getWindow() const;

  // window control
  void exit();

  // delta time between frame and time from beginning
  float getFrameDeltaTime() const;
  float getTime() const;

  // application run
  void run();

  // GLFW callbacks
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
  // Application informations
  //
  int getWidth();
  int getHeight();
  float getWindowRatio();
  bool windowDimensionChanged();
  std::shared_ptr<Camera> getCamera();

 private:
  enum State { stateReady, stateRun, stateExit };

  State state;

  Application& operator=(const Application&) { return *this; }

  GLFWwindow* window;

  // Time:
  float time;
  float deltaTime;

  // Dimensions:
  int width;
  int height;
  bool dimensionChanged;
  void detectWindowDimensionChange();
  void processInput(GLFWwindow *window);
  void dumpCameraMatrices();
  // Simulation:
  bool running = false;

 protected:
  Application(const Application&){};

  std::string title;

  std::shared_ptr<Camera> camera;

  virtual void loop();
  virtual void toggleNormalDisplay();
  virtual void toggleSimulation();
  virtual void toggleWireframe();

  std::map<char, int> keyPressState;
};

#endif /* end of include guard: OPENGL_CMAKE_SKELETON_APPLICATION_HPP */
