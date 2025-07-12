/**
 * OceanGestalt.hpp skeleton
 * Contributors:
 *      * Geoffrey Jones
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#ifndef __OCEAN_GESTALT_HPP
#define __OCEAN_GESTALT_HPP

#include "Application.hpp"
#include "Configuration.hpp"
#include "AppContextInterface.hpp"
#include "InputProcessor.hpp"
#include "KeyExecutable.hpp"
#include "OceanGestaltInterface.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "WaveUI.hpp"
#include "Moveable.hpp"
#include "KeyExecutable.hpp"
#include "SurfAudio.hpp"

#include "FPSCounter.hpp"
#include "FrameLimiter.hpp"

#include <iterator>

using std::unique_ptr;
using std::string;
using std::vector;

using glm::mat4;

class OceanGestalt : public OceanGestaltInterface,
                     public Application,
                     public Updatable,
                     public KeyExecutable {
 public:
  OceanGestalt();

  void toggleSimulation();

  AppContextInterface& getContext() override { return *configuration; };
  void doOnReady(const std::function<void()>& callback) override;
  void onRender(const std::function<void()>& callback) override;
  void pauseSimulation(bool) override;

 protected:
  void loop() override;
  void processInput(GLFWwindow *window, float deltaTime) override;
  void runOnce() override;
 
 private:
  // for restoring the window from fullscreen
  int windowXPos;
  int windowYPos;
  int windowWidth;
  int windowHeight;

  // shader matrix uniform
  glm::mat4 projection = mat4(1.0);
  glm::mat4 view = mat4(1.0);

  GLuint uboMatrices;

  shared_ptr<Configuration> configuration;
  std::vector<shared_ptr<MoveableInterface>>::iterator currentMoveable;
  unique_ptr<WaveUI> waveUI;
  vector<Model*> models;

  vector<std::shared_ptr<Drawable>> drawables;
  vector<std::shared_ptr<MoveableInterface>> moveables;

  float elapsedTime=0;
  double lastTime;
  bool isRunning = true;

  bool floatingCamera = true;
  FPSCounter fps;

  void toggleNormalDisplay();

  void toggleWireframe();
  void toggleMesh();
  void toggleDrawTriangles();
  void toggleDrawLines();
  void dumpUniforms();

  void toggleFullscreen(GLFWwindow*);

  void initUniformBuffers();
  void setUniformBuffers(mat4& projection, mat4& view) const;
  void loadUniforms();
  
  std::vector<std::function<void()>> onReadyCallbacks;
  std::vector<std::function<void()>> renderThreadCallbacks;

  std::shared_ptr<SurfAudio> surfAudio;
};

#endif
