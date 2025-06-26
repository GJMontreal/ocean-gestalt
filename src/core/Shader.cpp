/**
 * Shader.cpp
 * Contributors:
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */

#include "Shader.hpp"

#include <cstdlib>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "glError.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

const std::string shader_header = 
#ifdef __EMSCRIPTEN__
"#version 300 es\n"
"precision mediump float;\n"
"precision mediump int;\n"
"precision mediump sampler2DArray;\n";
#else
"#version 330\n";
#endif

// file reading
void getFileContents(const char* filename, vector<char>& buffer) {
  ifstream file(filename, ios_base::binary);
  if (file) {
     // Load shader header.
    for (const auto& c : shader_header)
      buffer.push_back(c);

    file.seekg(0, ios_base::end);
    streamsize size = file.tellg();
    if (size > 0) {
      file.seekg(0, ios_base::beg);
      buffer.resize(static_cast<size_t>(size + shader_header.size()));
      file.read(&buffer[shader_header.size()], size);
    }
    buffer.push_back('\0');
  } else {
    throw std::invalid_argument(string("The file ") + filename +
                                " doesn't exist");
  }
}

Shader::Shader(const std::string& filename, GLenum type) {
  // file loading
  vector<char> fileContent;
  getFileContents(filename.c_str(), fileContent);

  // creation
  handle = glCreateShader(type);
  if (handle == 0)
    throw std::runtime_error("[Error] Impossible to create a new Shader");

  // code source assignation
  const char* shaderText(&fileContent[0]);
  glShaderSource(handle, 1, (const GLchar**)&shaderText, NULL);

  // compilation
  glCompileShader(handle);

  // compilation check
  GLint compile_status;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &compile_status);
  if (compile_status != GL_TRUE) {
    GLsizei logsize = 0;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logsize);

    char* log = new char[logsize + 1];
    glGetShaderInfoLog(handle, logsize, &logsize, log);

    cout << "[Error] compilation error: " << filename << endl;
    cout << log << endl;

    exit(EXIT_FAILURE);
  } else {
    cout << "[Info] Shader " << filename << " compiled successfully" << endl;
  }
}

GLuint Shader::getHandle() const {
  return handle;
}

Shader::~Shader() {}

ShaderProgram::ShaderProgram() {
  handle = glCreateProgram();
  if (!handle)
    throw std::runtime_error("Impossible to create a new shader program");
}

ShaderProgram::ShaderProgram(const std::string& name, std::initializer_list<Shader> shaderList)
    : ShaderProgram() {
  this->name = name;
  for (auto& s : shaderList)
    glAttachShader(handle, s.getHandle());
  link();
}

const std::string& ShaderProgram::getName() {
  return name;
}

void ShaderProgram::link() {
  glLinkProgram(handle);
  GLint result;
  glGetProgramiv(handle, GL_LINK_STATUS, &result);
  if (result != GL_TRUE) {
    cout << "[Error] linkage error" << endl;

    GLsizei logsize = 0;
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logsize);

    char* log = new char[logsize];
    glGetProgramInfoLog(handle, logsize, &logsize, log);

    cout << log << endl;
  }
}

GLint ShaderProgram::uniform(const std::string& name) {
  auto it = uniforms.find(name);
  if (it == uniforms.end()) {
    // uniform that is not referenced
    GLint r = glGetUniformLocation(handle, name.c_str());
    if (r == GL_INVALID_OPERATION){
      cout << "[ERROR] GL_INVALID_OPERATION Getting uniform location" << endl;
    } else if( r < 0) {
      cout << "[Warning] uniform " << name << " doesn't exist in program" << endl;
    }else{
      uniforms[name] = r; // I don't think we should add it if it's not in the program
    }
    return r;
  } else
    return it->second;
}

GLint ShaderProgram::attribute(const std::string& name) {
  GLint attrib = glGetAttribLocation(handle, name.c_str());
  if (attrib == GL_INVALID_OPERATION || attrib < 0)
    cout << "[Error] Attribute " << name << " doesn't exist in program" << endl;

  return attrib;
}

void ShaderProgram::setAttribute(const std::string& name,
                                 GLint size,
                                 GLsizei stride,
                                 GLuint offset,
                                 GLboolean normalize,
                                 GLenum type) {
  GLint loc = attribute(name);
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, size, type, normalize, stride,
                        reinterpret_cast<void*>(offset));
}

void ShaderProgram::setAttribute(const std::string& name,
                                 GLint size,
                                 GLsizei stride,
                                 GLuint offset,
                                 GLboolean normalize) {
  setAttribute(name, size, stride, offset, normalize, GL_FLOAT);
}

void ShaderProgram::setAttribute(const std::string& name,
                                 GLint size,
                                 GLsizei stride,
                                 GLuint offset,
                                 GLenum type) {
  setAttribute(name, size, stride, offset, false, type);
}

void ShaderProgram::setAttribute(const std::string& name,
                                 GLint size,
                                 GLsizei stride,
                                 GLuint offset) {
  setAttribute(name, size, stride, offset, false, GL_FLOAT);
}

void ShaderProgram::setUniform(const std::string& name,
                               float x,
                               float y,
                               float z) {
  glUniform3f(uniform(name), x, y, z);
}

void ShaderProgram::setUniform(const std::string& name, const vec2& v) {
  glUniform3fv(uniform(name), 1, value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const vec3& v) {
  glUniform3fv(uniform(name), 1, value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const dvec3& v) {
  glUniform3dv(uniform(name), 1, value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const vec4& v) {
  glUniform4fv(uniform(name), 1, value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const dvec4& v) {
  glUniform4dv(uniform(name), 1, value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const dmat4& m) {
  glUniformMatrix4dv(uniform(name), 1, GL_FALSE, value_ptr(m));
}

void ShaderProgram::setUniform(const std::string& name, const mat4& m) {
  glUniformMatrix4fv(uniform(name), 1, GL_FALSE, value_ptr(m));
}

void ShaderProgram::setUniform(const std::string& name, const mat3& m) {
  glUniformMatrix3fv(uniform(name), 1, GL_FALSE, value_ptr(m));
}

void ShaderProgram::setUniform(const std::string& name, float val) {
  glUniform1f(uniform(name), val);
}

void ShaderProgram::setUniform(const std::string& name, int val) {
  glUniform1i(uniform(name), val);
}

void ShaderProgram::listUniforms() const {
  GLint count;
  glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);

  for (GLint i = 0; i < count; ++i) {
    char name[256];
    GLsizei length;
    GLint size;
    GLenum type;

    glGetActiveUniform(handle, i, sizeof(name), &length, &size, &type, name);

    GLint location = glGetUniformLocation(handle, name);
    std::cout << "Uniform: " << name << " (type: " << type
              << ", location: " << location << ")" << std::endl;
  }
}

// TODO: get rid of this, we'll rely on our uniform state and that everything passes through the api
std::optional<std::string> ShaderProgram::getUniform(const std::string& name) const{
  return std::string("0.35"); // till we get this working
}
ShaderProgram::~ShaderProgram() {
  // glDeleteProgram(handle);
}

void ShaderProgram::activate() const {
  glCheckError(__FILE__, __LINE__);
  glUseProgram(handle);

  // for(auto tex: textures){
  //   glActiveTexture(GL_TEXTURE + handle);
  //   glBindTexture(GL_TEXTURE_2D, tex.texID);
  // }
}

void ShaderProgram::deactivate() const {
  glUseProgram(0);
}

GLuint ShaderProgram::getHandle() const {
  return handle;
}

GLuint ShaderProgram::loadTexture(const std::string& path, const std::string& uniformName, GLuint unit) {
    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Texture settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 1);
    if (!data) {
        throw std::runtime_error("Failed to load texture: " + path);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Link texture to shader
    GLint loc = glGetUniformLocation(getHandle(), uniformName.c_str());
    glUniform1i(loc, unit);

    textures.push_back(TexBinding{loc,uniformName});
    return texID;
}
