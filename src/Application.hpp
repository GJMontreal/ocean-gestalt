/**
 * Application.cpp
 * Contributors:
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */

#ifndef OPENGL_CMAKE_SKELETON_APPLICATION_HPP
#define OPENGL_CMAKE_SKELETON_APPLICATION_HPP

#include <camera.h>
#include <string>

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
  Camera *getCamera();

 private:
  enum State { stateReady, stateRun, stateExit };

  Camera *camera;

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
  
  // Simulation:
  bool running = false;
 protected:
  Application(const Application&){};

  std::string title;

  virtual void loop();
};

#endif /* end of include guard: OPENGL_CMAKE_SKELETON_APPLICATION_HPP */
