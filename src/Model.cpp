#include "Model.hpp"

Model::Model(int meshSize): meshes({Mesh(meshSize)}),transform(1.0){
;
}

void Model::draw(glm::mat4 view, glm::mat4 projection){
// for each shader
  for(int i=0; i<meshes.size(); i++){
        meshes[i].draw(view,projection);
  }
}

void Model::setTransform(glm::mat4 transform){
  this->transform = transform;
}

