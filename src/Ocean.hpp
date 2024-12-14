#ifndef OCEAN_HPP
#define OCEAN_HPP

#include "Model.hpp"
#include "Wave.hpp"
#include "camera.h"
class Ocean : public Model{
public:
  Ocean(int meshSize, std::vector<ShaderProgram>shaderPrograms, Camera *camera);
  virtual ~Ocean();
  void draw()override;
private:
  Camera *camera;
  std::vector<VertexType> particles;
  void initParticles();
  void resetParticles();
  void moveParticles(float time);
  std::vector<Wave*> waves;
};

#endif
