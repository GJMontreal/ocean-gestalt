#ifndef OCEAN_HPP
#define OCEAN_HPP

#include "Model.hpp"
#include "Configuration.hpp"
#include "Wave.hpp"
#include "Camera.hpp"

#include <glm/glm.hpp>


class Ocean : public Model {
 public:
  explicit Ocean(std::shared_ptr<Configuration> configuration);

  ~Ocean() override = default;
  void draw(Uniforms& uniforms) override;

  void setupWireframeShader();
  void setupMeshShader();

  void updateShaderUniforms() override;
 private:
  float elapsedTime=0;
  double lastTime;
  std::vector<Wave*> waves; //this is moving to the configuration

  void setWaveUniforms(const vector<shared_ptr<Wave>>& waves,
                         shared_ptr<ShaderProgram> program)const override;
};

#endif
