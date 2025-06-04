/**
 * Application.hpp
 * Contributors:
 *      Geoffrey Jones
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */

#include "Application.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLFW/emscripten_glfw3.h>
#include <cmath>
#endif

using namespace std;

bool Application::firstMouse = true;
float Application::lastX;
float Application::lastY;

std::function<void()> registered_loop;
void loop_iteration() {
  registered_loop();
}

Application* currentApplication = NULL;

Application& Application::getInstance() {
  if (currentApplication)
    return *currentApplication;
  else
    throw std::runtime_error("There is no current Application");
}

#define GLFW_PLATFORM_EMSCRIPTEN 0x00060006

Application::Application()
    : state(stateReady), width(1200), height(600), title("Here we are, again.") {
  currentApplication = this;

  cout << "[Info] GLFW initialisation" << endl;
  if (!glfwInit()) {
    throw std::runtime_error("Couldn't init GLFW");
  }

// setting the opengl version
#ifdef __EMSCRIPTEN__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  emscripten::glfw3::SetNextWindowCanvasSelector("#canvas");
#else
  int major = 3;
  int minor = 2;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
  glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
  // create the window
  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Couldn't create a window");
  }
  
  #ifdef __EMSCRIPTEN__
  emscripten::glfw3::MakeCanvasResizable(window, "#canvas-container",nullptr);
  #endif
  
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();

  if (err != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error(string("Could initialize GLFW, error = ") +
                             (const char*)glewGetErrorString(err));
  }

  // get version info
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  cout << "Renderer: " << renderer << endl;
  cout << "OpenGL version supported " << version << endl;

  // opengl configuration
  glEnable(GL_DEPTH_TEST);  // enable depth-testing
  glDepthFunc(GL_LESS);  // depth-testing interprets a smaller value as "closer"

  glEnable(GL_CULL_FACE);

#ifndef __EMSCRIPTEN__
  // vsync
  glfwSwapInterval(1);
#endif

  // bind the callbacks
  glfwSetWindowUserPointer(window, this);
  glfwSetScrollCallback(window, Application::scrollCallback);
  glfwSetCursorPosCallback(window, Application::cursorCallback);
  glfwSetMouseButtonCallback(window, Application::mouseCallback);
  glfwSetFramebufferSizeCallback(window, Application::framebufferSizeCallback);
  glfwSetErrorCallback(Application::errorCallback);
}

GLFWwindow* Application::getWindow() const {
  return window;
}

void Application::exit() {
  state = stateExit;
}

float Application::getFrameDeltaTime() const {
  return deltaTime;
}

float Application::getTime() const {
  return time;
}

void Application::run() {
  state = stateRun;

  // Make the window's context current
  glfwMakeContextCurrent(window);

  setUIDelegate();
  time = (float)glfwGetTime();
  registered_loop = [&]() {
    // compute new time and delta time
    auto t = (float)glfwGetTime();
    deltaTime = t - time;
    time = t;

    processInput(window, deltaTime);

    // execute the frame code
    loop();

    // Swap Front and Back buffers (double buffering)
    glfwSwapBuffers(window);

    // Pool and process events
    glfwPollEvents();
  };
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop_iteration, 0, 1);
#else
  while (state == stateRun) {
    loop_iteration();
  }
  glfwTerminate();
#endif
}

void Application::loop() {
  cout << "[INFO] : loop" << endl;
}

void Application::setUIDelegate() {
  cout << "[INFO] : setUIDelegate" << endl;
}

int Application::getWidth() const {
  return width;
}

int Application::getHeight() const{
  return height;
}

float Application::getWindowRatio() const{
  return float(width) / float(height);
}

std::shared_ptr<Camera> Application::getCamera() {
  return camera;
}

std::shared_ptr<Light> Application::getLight() {
  return light;
}

void Application::framebufferSizeCallback(GLFWwindow* window, int w, int h){
  glViewport(0, 0, w, h);
  Application::getInstance().width = w;
  Application::getInstance().height = h;
}

void Application::scrollCallback(GLFWwindow* window,
                                 double,
                                 double yoffset) {
  auto app = (Application*)glfwGetWindowUserPointer(window);
  auto camera = app->getCamera();
  camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Application::cursorCallback(GLFWwindow* window,
                                double xposIn,
                                double yposIn) {
  // Ignore if the mouse button is not pressed
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);  

  if(state == GLFW_RELEASE){
    return;
  }

  auto app = (Application*)glfwGetWindowUserPointer(window);
  auto camera = app->getCamera();
  auto light = app->getLight();

  auto xpos = static_cast<float>(xposIn);
  auto ypos = static_cast<float>(yposIn);
  
  if (Application::firstMouse) {
    lastX = xpos;
    lastY = ypos;
    Application::firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos;  // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera->ProcessMouseMovement(xoffset, yoffset);
  light->moveForward = camera->moveForward;
  light->moveRight = camera->moveRight;
}

// Reset the starting position on a mouse or touch up
void Application::mouseCallback(GLFWwindow *window, int, int state, int){
  if(state == 0){
    Application::firstMouse = true;
  }
}

void Application::errorCallback(int error_code, const char *description){
  cout << "GLFW error: " << error_code << " " << description << endl;
}

// ---------------------------------------------------------------------------------------------------------
// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
void Application::processInput(GLFWwindow*, float){
}


