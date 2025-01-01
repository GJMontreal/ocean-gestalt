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

Ocean::Ocean(int meshSize,
             std::vector<ShaderProgram> shaderPrograms,
             std::shared_ptr<Camera> camera)
    : Model(meshSize, shaderPrograms, camera) {
  initParticles();
  resetParticles();

  waves.push_back(new Wave(2, 10, vec2(0, 1),2));
  waves.push_back(new Wave(4,17,vec2(0.4,1)));
  waves.push_back(new Wave(0.1f,2,vec2(.5,1)));
  
  // setShaderUniforms(waves, getShaderProgram(0));
}

Ocean::Ocean(int meshSize, std::shared_ptr<Configuration> configuration): 
Model(meshSize,configuration){
  // setup the shaders wave uniforms
  setShaderUniforms(configuration->waves,configuration->meshShader);
  setShaderUniforms(configuration->waves,configuration->wireframeShader);
  setShaderUniforms(configuration->waves,configuration->normalShader);
}

//If this is all that's happening here, we can probably move it
void Ocean::beginDrawing(std::shared_ptr<ShaderProgram> program, Uniforms& uniforms){
   program->setUniform("time",uniforms.time);
}

void Ocean::draw(Uniforms& uniforms) {
  // calculate new positions for our particles
  auto time = float(glfwGetTime());
  uniforms.time = time;
  if (isRunning()) {
    // auto time = float(glfwGetTime());
    // uniforms.time = time;
    // moveParticles(time);
  }
 
  Model::draw(uniforms);
}

// Ocean::~Ocean() {
// //TO DO: destructor
// }

void Ocean::initParticles() {
  int size = getMesh(0)->getSize();
  for (int i = 0; i < (size + 1) * (size + 1); i++) {
    particles.resize((size + 1) * (size + 1));
  }
}

void Ocean::resetParticles() {
  int size = getMesh(0)->getSize();
  std::vector<VertexType>::iterator particle;
  particle = particles.begin();
  for (int y = 0; y <= size; ++y)
    for (int x = 0; x <= size; ++x) {
      auto xx = (float)x - (float)size / 2.0f;
      auto yy = (float)y - (float)size / 2.0f;
      particle->position.x = xx;
      particle->position.y = yy;
      particle->position.z = 0;
      particle->normal = glm::normalize(glm::vec3(xx, yy, 1.0)); //TO DO: do we care about this
      particle->color = glm::vec4({.01f, .15f, .210f, 0.0f});  //TO DO: we should specify this
      particle++;
    }
}

void Ocean::moveParticles(float time) {
  int size = getMesh(0)->getSize();
  std::vector<VertexType>::iterator iter;
  iter = particles.begin();

  for (int y = 0; y <= size; ++y) {
    for (int x = 0; x <= size; ++x) {
      auto xx = (float)x - (float)size / 2.0f;
      auto yy = (float)y - (float)size / 2.0f;

      vec2 position(xx, yy);

      iter->position = vec3(position, 0);
      iter->normal = vec3(0.0f, 0.0f, 0.0f);

      for (Wave* wave : waves) {
        vec3 newPosition = gerstnerWave(time, position, wave);

        vec3 normal =
            numericalDerivativeNormal(newPosition, position, wave, time);
        iter->position += newPosition;
        iter->normal += normal;

#ifdef USE_MINE

        float innerProd =
            time * wave->velocity - (wave->k.x * vec0.x + wave->k.y * vec0.y);
        iter->position.z += wave->amplitude * cos(innerProd);
        // we can do some optimizations here
        iter->position.x +=
            wave->amplitude * sin(innerProd) * wave->k.x / wave->kMagnitude;
        iter->position.y +=
            wave->amplitude * sin(innerProd) * wave->k.y / wave->kMagnitude;
#endif
      }

      iter++;
    }
  }
  // update the vertices
  //  meshes[0].calculateNormals(particles, meshes[0].getTriangularIndices());
  glBindBuffer(GL_ARRAY_BUFFER, getMesh(0)->getVbo());
  glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(VertexType),
                  particles.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// From
// https://www.gamedev.net/forums/topic/687501-how-to-compute-normal-vectors-for-gerstner-waves/5337386/
// https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models
vec3 Ocean::gerstnerWave(float time, vec2 position, const Wave* wave) const{
  float k = wave->getVelocity();
  vec2 D = normalize(wave->direction);  // normalized direction
  vec2 K = D * k;                       // wave vector and magnitude (direction)

  // peak/crest steepness high means steeper, but too much
  // can cause the wave to become inside out at the top
  //  float Q = steepness; //max(steepness, 0.1);

  // Original formula, however is more difficult to control speed
  // float w = sqrt(9.82*k); // frequency (speed)
  // float wt = w*Time;
  float speed = 2;
  // float S = speed * 0.5; // Speed 1 =~ 2m/s so halve first
  float w = speed * k;  // Phase/frequency //we need to work out th velocity
  float wT = w * time;

  // Unoptimized
  // xz = position.xz - K/k*Q*A*sin(dot(K,position.xz)- wT);
  //  y = A*cos(dot(K,position.xz)- wT);

  // Calculate once instead of 4 times
  // I think we should rely on the compiler optimizing things, at least at first
  float KPwT = dot(K, position) - wT;
  float S0 = sin(KPwT);
  float C0 = cos(KPwT);

  VertexType vertex;

  vec2 xy = position - D * wave->steepness * wave->amplitude * S0;
  float z = wave->amplitude * C0;
  return vec3(xy, z);
}

vec3 Ocean::numericalDerivativeNormal(vec3 lastPosition,
                                      vec2 position,
                                      Wave* wave,
                                      float time,
                                      float offset) {
  // discrete/numerical derivative
  vec3 dx = vec3(offset, 0, 0) +
            gerstnerWave(time, vec2(position.x + offset, position.y), wave);
  vec3 dy = vec3(0, offset, 0) +
            gerstnerWave(time, vec2(position.x, position.y + offset), wave);
            //   vec3 dt = vec3(offset, offset, 0) +
            // gerstnerWave(time, vec2(position.x+offset, position.y + offset), wave);
  vec3 normal = normalize(cross(dx - lastPosition, dy - lastPosition));
  return normal;  // flip the normal because of our winding order
}

void Ocean::setShaderUniforms(const vector<shared_ptr<Wave>>& waves,
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
