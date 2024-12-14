#ifndef OCEAN_HPP
#define OCEAN_HPP

#include "Model.hpp"
#include "Wave.hpp"
class Ocean : public Model{
public:
  Ocean(int meshSize, std::vector<ShaderProgram>shaderPrograms);
  virtual ~Ocean();
  void draw()override;
private:
  std::vector<VertexType> particles;
  void initParticles();
  void resetParticles();
  void moveParticles(float time);
  std::vector<Wave*> waves;
};

#endif
