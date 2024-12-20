#ifndef OCEAN_HPP
#define OCEAN_HPP

#include "Model.hpp"
#include "Wave.hpp"
#include "camera.h"
class Ocean : public Model{
public:
  Ocean(int meshSize, std::vector<ShaderProgram>shaderPrograms, Camera *camera);
  virtual ~Ocean();
  void draw(Uniforms uniforms)override;

  bool animateCamera = true;
private:

  std::vector<VertexType> particles;
  void initParticles();
  void resetParticles();
  void moveParticles(float time);
  vec3 gerstnerWave(float time, vec2 position, Wave* wave);  //we're only using pointers here to take advantage of the . members
  vec3 numericalDerivativeNormal(vec3 lastPosition, vec2 position, Wave* wave, float time, float offset = 0.01f);
  std::vector<Wave*> waves;
};

#endif
