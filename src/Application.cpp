/**
 * Application.hpp
 * Contributors:
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */

#include "Application.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include "Debug.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <cmath>
#endif

using namespace std;

static bool firstMouse = true;
static float lastX;
static float lastY;

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

Application::Application()
    : state(stateReady), width(1280), height(480), title("Application") {
  currentApplication = this;

  cout << "[Info] GLFW initialisation" << endl;

  // initialize the GLFW library
  if (!glfwInit()) {
    throw std::runtime_error("Couldn't init GLFW");
  }

// setting the opengl version
#ifdef __EMSCRIPTEN__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
  int major = 3;
  int minor = 2;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

  // create the window
  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Couldn't create a window");
  }

  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();

  if (err != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error(string("Could initialize GLEW, error = ") +
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
  // vsync
  // glfwSwapInterval(false);
  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
  // camera = Camera(glm::vec3{15,3.8,15},glm::vec3{-.002,1,-.02},-138,-1.3);


  // bind the callbacks
  glfwSetWindowUserPointer(window, this);
  glfwSetScrollCallback(window, Application::scrollCallback);
  glfwSetCursorPosCallback(window, Application::mouseCallback);
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

  time = glfwGetTime();
  registered_loop = [&]() {
    // compute new time and delta time
    float t = glfwGetTime();
    deltaTime = t - time;
    time = t;

    // detech window related changes
    detectWindowDimensionChange();

    processInput(window);
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

void Application::detectWindowDimensionChange() {
  int w, h;
  glfwGetWindowSize(getWindow(), &w, &h);
  dimensionChanged = (w != width || h != height);
  if (dimensionChanged) {
    width = w;
    height = h;
    glViewport(0, 0, width, height);
  }
}

void Application::loop() {
  cout << "[INFO] : loop" << endl;
}

int Application::getWidth() {
  return width;
}

int Application::getHeight() {
  return height;
}

float Application::getWindowRatio() {
  return float(width) / float(height);
}

bool Application::windowDimensionChanged() {
  return dimensionChanged;
}

Camera* Application::getCamera() {
  return &camera;
}

void Application::scrollCallback(GLFWwindow* window,
                                 double xoffset,
                                 double yoffset) {
  Application* app = (Application*)glfwGetWindowUserPointer(window);
  Camera* camera = app->getCamera();
  camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Application::mouseCallback(GLFWwindow* window,
                                double xposIn,
                                double yposIn) {
  // Ignore if the mouse button is not pressed
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);  
  if(state == GLFW_RELEASE){
    firstMouse = true;
    return;
  }

  Application* app = (Application*)glfwGetWindowUserPointer(window);
  Camera* camera = app->getCamera();
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos;  // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera->ProcessMouseMovement(xoffset, yoffset);
}

// ---------------------------------------------------------------------------------------------------------
// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
void Application::processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera.ProcessKeyboard(UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera.ProcessKeyboard(DOWN, deltaTime);

  // These are problematic because they might get called many times
  // for each; save the state and only call the function when it changes
  // to release
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    if (keyPressState[GLFW_KEY_C] == GLFW_RELEASE) {
      dumpCameraMatrices();
      keyPressState[GLFW_KEY_C] = GLFW_PRESS;
    }
  } else {
    keyPressState[GLFW_KEY_C] = GLFW_RELEASE;
  }
  
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
    if (keyPressState[GLFW_KEY_N] == GLFW_RELEASE) {
      toggleNormalDisplay();
      keyPressState[GLFW_KEY_N] = GLFW_PRESS;
    }
  } else {
    keyPressState[GLFW_KEY_N] = GLFW_RELEASE;
  }

  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    if(keyPressState[GLFW_KEY_R] == GLFW_RELEASE){
      toggleSimulation();
      keyPressState[GLFW_KEY_R] = GLFW_PRESS;
    }
  }else{
    keyPressState[GLFW_KEY_R] = GLFW_RELEASE;
  }

  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    if(keyPressState[GLFW_KEY_L] == GLFW_RELEASE){
      toggleWireframe();
      keyPressState[GLFW_KEY_L] = GLFW_PRESS;
    }
  }else{
    keyPressState[GLFW_KEY_L] = GLFW_RELEASE;
  }
}

void Application::toggleNormalDisplay(){
}

void Application::toggleSimulation(){
}

void Application::toggleWireframe (){
}

void Application::dumpCameraMatrices(){
  dumpVector("Front", camera.Front);
  dumpVector("Position", camera.Position);
  dumpVector("Up", camera.Up);
  std::cout << "Yaw " << camera.Yaw << std::endl;
  std::cout << "Pitch " << camera.Pitch << std::endl;
}

