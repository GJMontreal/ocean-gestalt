#include "Ocean.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>

#include <memory>
#include <string>
#include <stdexcept>

#include <iostream>

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}


Ocean::Ocean(std::shared_ptr<Configuration> aConfiguration): 
Model(aConfiguration->meshDimension, aConfiguration->meshSize, aConfiguration){
  configuration = aConfiguration;
  std::cout << "Constructing ocean" << std::endl;
}

void Ocean::updateShaderUniforms(){
  setWaveUniforms(configuration->waves,configuration->meshShader);
  setWaveUniforms(configuration->waves,configuration->wireframeShader); 
  configuration->wireframeShader->setUniform("lineColor",configuration->wireframeColor);
  
  #ifndef __EMSCRIPTEN__
  setWaveUniforms(configuration->waves,configuration->normalShader);
  configuration->wireframeShader->setUniform("lineColor",configuration->normalColor);
  #endif
}

void Ocean::draw(Uniforms& uniforms) {

  // we want to limit how often we draw ie. there's no point in having an excessively high framerate

  // calculate new positions for our particles
  auto time = float(glfwGetTime());
  auto interval = time - lastTime;
  lastTime = time;

  if (isRunning()) {
    elapsedTime += (float)interval;
  }
  uniforms.time = elapsedTime;
  Model::draw(uniforms);
}

// Ocean::~Ocean() {
// //TO DO: destructor
// }

// From
// https://www.gamedev.net/forums/topic/687501-how-to-compute-normal-vectors-for-gerstner-waves/5337386/
// https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models

void Ocean::setWaveUniforms(const vector<shared_ptr<Wave>>& waves,
                       shared_ptr<ShaderProgram> program) const{
  program->activate();
  int i = 0;
  for( const shared_ptr<Wave> wave: waves){
    std::string uniformName = string_format("waves[%i].amplitude", i);

    program->setUniform(uniformName, wave->amplitude);

    uniformName = string_format("waves[%i].steepness", i);
    program->setUniform(uniformName, wave->steepness);

    uniformName = string_format("waves[%i].wavelength", i);
    program->setUniform(uniformName, wave->wavelength);

    uniformName = string_format("waves[%i].direction", i);
    program->setUniform(uniformName, glm::vec3(wave->direction, 0));

    i++;
  }
}
