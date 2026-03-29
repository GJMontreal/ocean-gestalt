/**
 * OceanGestalt.hpp skeleton
 * Contributors:
 *      * Geoffrey Jones
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#pragma once

#include "Application.hpp"
#include "Configuration.hpp"
#include "AppContextInterface.hpp"
#include "InputProcessor.hpp"
#include "KeyExecutable.hpp"
#include "OceanGestaltInterface.hpp"
#include "Model.hpp"
#include "WaveUI.hpp"
#include "Moveable.hpp"
#include "KeyExecutable.hpp"
#include "SurfAudio.hpp"

#include "FPSCounter.hpp"
#include "ReflectionPass.hpp"
#include "ShadowPass.hpp"

using std::unique_ptr;
using std::string;
using std::vector;

using glm::mat4;

class OceanGestalt;

using Action = std::function<void(OceanGestalt*)>;

class KeyBindings {
  public:
  std::vector<std::pair<int, Action>> getKeyBindings() const;
};

using OptionalMoveable = std::optional<std::shared_ptr<MoveableInterface>>;
using OptionalDrawable = std::optional<std::shared_ptr<Drawable>>;
  
struct SceneElement{
  std::string name;
  OptionalDrawable drawable;
  OptionalMoveable moveable;
};

static const std::vector<std::pair<int, Movement>> movementKeys{
    {GLFW_KEY_W, Movement::FORWARD}, {GLFW_KEY_S, Movement::BACKWARD},
    {GLFW_KEY_A, Movement::LEFT},    {GLFW_KEY_D, Movement::RIGHT},
    {GLFW_KEY_SPACE, Movement::UP},  {GLFW_KEY_LEFT_SHIFT, Movement::DOWN}};

class OceanGestalt final : public OceanGestaltInterface,
                     public Application,
                     public Updatable,
                     public KeyExecutable {
 friend KeyBindings;
 public:
  OceanGestalt();

  void toggleSimulation();

  AppContextInterface& getContext() override { return *configuration; };
  void doOnReady(const std::function<void()>& callback) final;
  void onRender(const std::function<void(float)>& callback) final;
  void pauseSimulation(bool) override;
  void muteAudio(bool);
  bool getIsRunning() const { return isRunning;};
  
 protected:
  void loop() override;
  void processInput(GLFWwindow *window, float deltaTime) override;
  void runOnce() override;
 
 private:
  // for restoring the window from fullscreen
  int windowXPos = 0;
  int windowYPos = 0;
  int restorationWindowWidth = 0;
  int restorationWindowHeight = 0;

  // shader matrix uniform
  glm::mat4 projection = mat4(1.0);
  glm::mat4 view = mat4(1.0);

  GLuint uboMatrices;

  shared_ptr<Configuration> configuration;
  
  std::vector<SceneElement> sceneElements;
  std::vector<SceneElement>::iterator currentElement;

  double elapsedTime=0;
  double lastTime = 0.0;
  bool isRunning = true;

  bool shouldRenderText = false;
  double textVisibleTime = 0;

  FPSCounter fps;

  KeyBindings keyBindings;

  void toggleNormalDisplay();
  void toggleWireframe();
  void toggleMesh();
  void toggleDrawTriangles();
  void toggleDrawLines();
  void dumpUniforms();
  void toggleFloatingCamera();
  void selectNextElement();
  void toggleFullscreen(GLFWwindow*);
  void toggleDisplayText();

  void initUniformBuffers();
  void setUniformBuffers(mat4& projection, mat4& view) const;
  void loadUniforms();
  void generateUniforms();

  void buildScene();
  void renderText();
  
  std::vector<std::function<void()>> onReadyCallbacks;
  std::vector<std::function<void(double time)>> renderThreadCallbacks;

  std::shared_ptr<SurfAudio> surfAudio;
  std::unique_ptr<ReflectionPass> reflectionPass;
  std::unique_ptr<ShadowPass> shadowPass;
};
