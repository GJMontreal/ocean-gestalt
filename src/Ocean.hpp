#ifndef OCEAN_HPP
#define OCEAN_HPP

// #include <IOKit/hid/IOHIDUsageTables.h> // where did this come from
#include "Model.hpp"
#include "Configuration.hpp"
#include "Wave.hpp"
#include "Camera.hpp"

#include <glm/glm.hpp>

using vec3 = glm::vec3;

class Ocean : public Model {
 public:
  Ocean(std::shared_ptr<Configuration> configuration);

  ~Ocean() override = default;
  void draw(Uniforms& uniforms) override;

  void setupWireframeShader();
  void setupMeshShader();

  void updateShaderUniforms() override;
 private:
  // We'll only use this stuff if we're using the cpu to calculate our vertices
  std::shared_ptr<Configuration> configuration;
  std::vector<VertexType> particles;
  float elapsedTime=0;
  double lastTime;
  void initParticles();
  void resetParticles();
  void moveParticles(float time);
  vec3 gerstnerWave(float time, vec2 position, const Wave* wave) const;

  std::vector<Wave*> waves; //this is moving to the configuration

  void setWaveUniforms(const vector<shared_ptr<Wave>>& waves,
                         shared_ptr<ShaderProgram> program)const override;
};

#endif
