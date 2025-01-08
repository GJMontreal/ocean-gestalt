/**
 * Application.cpp
 * Contributors:
 *      * Geoffrey Jones
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */

#ifndef OPENGL_CMAKE_SKELETON_APPLICATION_HPP
#define OPENGL_CMAKE_SKELETON_APPLICATION_HPP

#include "Camera.hpp"
#include "InputProcessor.hpp"
#include "Light.hpp"

#include <GLFW/glfw3.h>

#include <string>
#include <memory>


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
class Application: public InputProcessor {
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
  int getWidth() const;
  int getHeight() const;
  float getWindowRatio() const;
  bool windowDimensionChanged() const;

  std::shared_ptr<Camera> getCamera();
  std::shared_ptr<Light> getLight();

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

  // Simulation:
  bool running = false;

 protected:
  Application(const Application&){};

  std::string title;

  std::shared_ptr<Camera> camera;
  std::shared_ptr<Light> light;

  virtual void loop();
  void processInput(GLFWwindow *window, float deltaTime) override;
};

#endif /* end of include guard: OPENGL_CMAKE_SKELETON_APPLICATION_HPP */
