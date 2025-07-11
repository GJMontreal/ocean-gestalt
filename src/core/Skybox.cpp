#include "Skybox.hpp"

#include "Configuration.hpp"
#include "glError.hpp"
#include "Shader.hpp"

#include <iostream>

Skybox::Skybox(std::shared_ptr<Configuration> context):Drawable(glm::vec3(0.f,0.f,0.f),context){
  bindVertices();
}

void Skybox::bindVertices() {
  std::vector<glm::vec3> vertices = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1},
                                     {-1, 1, -1},  {-1, -1, 1}, {1, -1, 1},
                                     {1, 1, 1},    {-1, 1, 1}};
  // std::vector<unsigned int> indices = {// back
  //                           2, 1, 0, 0, 3, 2,
  //                           // front
  //                           4, 5, 6, 6, 7, 4,
  //                           // left
  //                           0, 3, 7, 7, 4, 0,
  //                           // right
  //                           1, 5, 6, 6, 2, 1,
  //                           // top
  //                           3, 2, 6, 6, 7, 3,
  //                           // bottom
  //                           0, 4, 5, 5, 1, 0};

                            std::vector<unsigned int> indices = {
    // -Z (back)
    2, 1, 0, 0, 3, 2,
    // +Z (front)
    4, 5, 6, 6, 7, 4,
    // -X (left)
    0, 4, 7, 7, 3, 0,
    // +X (right)
    1, 2, 6, 6, 5, 1,
    // +Y (top)
    3, 7, 6, 6, 2, 3,
    // -Y (bottom)
    0, 1, 5, 5, 4, 0
};
  
  indexCount = indices.size();

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
               vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (void*)nullptr);

  glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);
}

void Skybox::draw(Uniforms& uniforms){
  if(!getIfShouldDraw()){
    return;
  }

  drawMesh(uniforms, glm::mat4(1.0f));
}

void Skybox::drawMesh(Uniforms& uniforms, glm::mat4 transform) {
  auto shader = getContext()->getShader("skybox");
  auto _guard = ShaderScope(shader);

  glFrontFace(GL_CW);
  glDepthMask(GL_FALSE); 
  // glDepthFunc(GL_LEQUAL);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  // glDepthFunc(GL_LESS);
  glFrontFace(GL_CCW);
  glDepthMask(GL_TRUE);
#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif
}

// GLuint loadCubemap(const std::vector<std::string>& faces) {
//     GLuint textureID;
//     glGenTextures(1, &textureID);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

//     int width, height, nrChannels;
//     for (GLuint i = 0; i < faces.size(); i++) {
//         unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//         if (data) {
//             glTexImage2D(
//                 GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//                 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
//             );
//             stbi_image_free(data);
//         } else {
//             std::cerr << "Failed to load cubemap face: " << faces[i] << std::endl;
//         }
//     }

//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

//     return textureID;
// }

// std::vector<std::string> faces = {
//     "right.jpg",  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
//     "left.jpg",   // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
//     "top.jpg",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
//     "bottom.jpg", // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
//     "front.jpg",  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
//     "back.jpg"    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
// };

// //disable depth writing
// glDepthFunc(GL_LEQUAL);  // Change depth function so skybox passes

// skyboxShader.use();
// mat4 view = mat4(mat3(camera.getViewMatrix())); // Remove translation
// skyboxShader.setMat4("view", view);
// skyboxShader.setMat4("projection", projection);

// glBindVertexArray(skyboxVAO);
// glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
// glDrawArrays(GL_TRIANGLES, 0, 36);

// glDepthFunc(GL_LESS); // Restore default


// // Reflections in fragment shader
// vec3 I = normalize(fs_in.FragPos - viewPos);      // Incident view vector
// vec3 R = reflect(I, normalize(fs_in.Normal));     // Reflection vector
// vec3 reflection = texture(skybox, R).rgb;         // Sample the cubemap