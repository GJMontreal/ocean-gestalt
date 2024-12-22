#ifndef OCEAN_HPP
#define OCEAN_HPP

#include "Model.hpp"
#include "Wave.hpp"
#include "camera.h"

#include <glm/glm.hpp>

using vec3 = glm::vec3;

class Ocean : public Model{
public:
  Ocean(int meshSize, std::vector<ShaderProgram> shaderPrograms, Camera *camera);
  ~Ocean()override = default;
  void draw(const Uniforms& uniforms)override;
private:

  std::vector<VertexType> particles;
  void initParticles();
  void resetParticles();
  void moveParticles(float time);
  vec3 gerstnerWave(float time, vec2 position, const Wave* wave)const;
  vec3 numericalDerivativeNormal(vec3 lastPosition, vec2 position, Wave* wave, float time, float offset = 0.01f);
  std::vector<Wave*> waves;
};

#endif
