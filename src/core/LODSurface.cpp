#include "LODSurface.hpp"

#include "Camera.hpp"
#include "Configuration.hpp"
#include "glError.hpp"
#include "Light.hpp"
#include "Shader.hpp"


LODSurface::LODSurface(glm::vec3 origin, std::shared_ptr<Configuration> context)
    : Drawable(origin, context) {
      generateMesh(vertices, indices, 300,150);
      bindVertices();
      bindRays();
    }

void LODSurface::draw(Uniforms& uniforms) {
   auto transform = glm::mat4(1.f);
   drawMesh(uniforms, transform);
   drawProjectedMesh(uniforms);
  //  drawRays(uniforms);
}

void LODSurface::drawNormals(Uniforms& uniforms, glm::mat4 transform) {}

void LODSurface::drawMesh(Uniforms& uniforms, glm::mat4 transform) {
  assert(shader);
  auto _guard = ShaderScope(shader);
  // how could we toggle this using our restapi
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  setCommonUniforms(shader,uniforms);
  shader->setUniform("color",getContext()->meshColor);
  shader->setUniform("time", uniforms.time);
  shader->setUniform("lightPos", getContext()->light->getPosition());

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif  

}

void LODSurface::drawProjectedMesh(Uniforms& uniforms) {
  assert(projectionShader); // ensure we have assigned a shader
  auto _guard = ShaderScope(projectionShader);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDisable(GL_CULL_FACE);

  setCommonUniforms(projectionShader,uniforms);

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif  
}

void LODSurface::generateMesh(std::vector<glm::vec2>& vertices,
                              std::vector<unsigned int>& indices,
                              int width, int height) {
  vertices.clear();
  indices.clear();


  // Step size from -1 to 1
  float xStep = 2.0f / (width - 1);
  float yStep = 2.0f / (height - 1);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      float px = -1.0f + x * xStep;
      float py = -1.0f + y * yStep;
      auto vertex = glm::vec2(px,py);
      vertices.push_back({vertex});
    }
  }

  // Generate indices (triangle grid) & lines
  for (int y = 0; y < height - 1; ++y) {
    for (int x = 0; x < width - 1; ++x) {
      int i = y * width + x;

      indices.push_back(i);
      indices.push_back(i + width);
      indices.push_back(i + 1);

      indices.push_back(i + 1);
      indices.push_back(i + width);
      indices.push_back(i + width + 1);


    }
  }
}

void LODSurface::bindVertices() {
  std::vector<NDCVertex> vertexData;
  vertexData.reserve( vertices.size());
  for (auto vertex : vertices) {
    vertexData.push_back({vertex});
  }

  // VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(NDCVertex),
               vertexData.data(), GL_STATIC_DRAW);

  // EBO
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(NDCVertex),
                        (void*)offsetof(NDCVertex, position));

  glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
  glCheckError(__FILE__, __LINE__);
}

void LODSurface::bindRays() {
  std::vector<glm::vec2> vertexData;
  vertexData.reserve(2* vertices.size());
  for(auto vertex: vertices){
    vertexData.push_back(vertex);
    vertexData.push_back(vertex);
  }
  // VAO
  glGenVertexArrays(1, &rayVAO);
  glBindVertexArray(rayVAO);

    // VBO
  glGenBuffers(1, &rayVBO);
  glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(glm::vec2),
               vertexData.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

  glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
  glCheckError(__FILE__, __LINE__);
}


glm::vec3 intersectRayWithPlane(glm::vec3 origin, glm::vec3 dir, float yPlane) {
    float t = (yPlane - origin.y) / dir.y;
    return origin + dir * t;

}

void LODSurface::debugRay(Uniforms& uniforms){
    auto camera = getContext()->camera;
//just duplicate what's going on in the shader for a single vertex
  //let's try the top left (-1,-1)
    float tanHalfFovY = tan(0.5 * glm::radians(camera->Zoom));
    auto topLeft = glm::vec2(0.0, 0.0);
    auto rayView = glm::vec3(
        topLeft.x * 2.0 * tanHalfFovY,
        topLeft.y * 2.0,
        -1.0
    );
    
    auto viewRot = glm::inverse(glm::mat3(uniforms.view));
    auto cameraPos = camera->getPosition();
    // auto rayOrigin = cameraPos + (viewRot * rayView);
auto rayOrigin = cameraPos;
    auto rayDir = normalize(viewRot * rayView);

    auto hit = intersectRayWithPlane(rayOrigin, rayDir, 0.0);
    std::cout << "rayDir " << to_string(rayDir) << std::endl;
    std::cout << "hit " << to_string(hit) << std::endl;
    std::cout << "rayOrigin " << to_string(rayOrigin) << std::endl;
    std::cout << "cameraPos " << to_string(cameraPos) << std::endl;
    std::cout << "viewRot " << to_string(viewRot) << std::endl;
    std::cout << "cameraFront " << to_string(camera->Front) << std::endl;

    glm::vec4 clip = uniforms.projection * uniforms.view * glm::vec4(hit, 1.0f);
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
  std::cout << "NDC = " << glm::to_string(ndc) << std::endl;
}

void LODSurface::drawRays(Uniforms& uniforms){
 
  auto _guard = ShaderScope(rayShader);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // glDisable(GL_DEPTH_TEST);
  // glDisable(GL_CULL_FACE);
  glEnable(GL_PROGRAM_POINT_SIZE);
  setCommonUniforms(rayShader, uniforms);
  glBindVertexArray(rayVAO);
  glDrawArrays(GL_POINTS, 0, vertices.size() * 2 );
  glDrawArrays(GL_LINES, 0, vertices.size() * 2 );
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  #ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif 
  // debugRay(uniforms);
}

void LODSurface::setCommonUniforms(std::shared_ptr<ShaderProgram> aShader,
                             Uniforms& uniforms) {
  // our shader is already guarded

  auto camera = getContext()->camera;

  aShader->setUniform("cameraPos", camera->getPosition());
  aShader->setUniform("fovYRadians", glm::radians(camera->Zoom));
  aShader->setUniform("aspect", 2.0f);  // TODO: add this to the context
  // auto invViewProjection = glm::inverse(uniforms.projection * uniforms.view);
  // shader->setUniform("invViewProjection", invViewProjection);

}