#ifndef OCEAN_HPP
#define OCEAN_HPP

#include "Model.hpp"
#include "Wave.hpp"
#include "Camera.hpp"

#include <glm/glm.hpp>

using vec3 = glm::vec3;

class Ocean : public Model {
 public:
  Ocean(int meshSize,
        std::vector<ShaderProgram> shaderPrograms,
        std::shared_ptr<Camera> camera);
  ~Ocean() override = default;
  void draw(Uniforms& uniforms) override;
  void beginDrawing(ShaderProgram& program, Uniforms& uniforms) override;

  void setupWireframeShader();
  void setupMeshShader();

 private:
  // We'll only use this stuff if we're using the cpu to calculate our vertices
  std::vector<VertexType> particles;
  void initParticles();
  void resetParticles();
  void moveParticles(float time);
  vec3 gerstnerWave(float time, vec2 position, const Wave* wave) const;

  vec3 numericalDerivativeNormal(vec3 lastPosition,
                                 vec2 position,
                                 Wave* wave,
                                 float time,
                                 float offset = 0.01f);
  // end CPU routines

  std::vector<Wave*> waves;

  void setShaderUniforms(const std::vector<Wave*>& waves,
                         ShaderProgram* program);
};

#endif
