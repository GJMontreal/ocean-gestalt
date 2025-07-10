/**
 * glError.cpp
 * Contributors:
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */

#include "glError.hpp"

#include <GL/glew.h>
#include <iostream>
#include <string>

using namespace std;

bool glCheckError(const char* file, unsigned int line, const char* info ) {
  bool error = false;
  GLenum errorCode = glGetError();

  while (errorCode != GL_NO_ERROR) {
    error = true;
    string fileString(file);
    string error = "unknown error";

    // clang-format off
    switch (errorCode) {
      case GL_INVALID_ENUM:      error = "GL_INVALID_ENUM"; break;
      case GL_INVALID_VALUE:     error = "GL_INVALID_VALUE"; break;
      case GL_INVALID_OPERATION: error = "GL_INVALID_OPERATION"; break;
      case GL_STACK_OVERFLOW:    error = "GL_STACK_OVERFLOW"; break;
      case GL_STACK_UNDERFLOW:   error = "GL_STACK_UNDERFLOW"; break;
      case GL_OUT_OF_MEMORY:     error = "GL_OUT_OF_MEMORY"; break;
    }
    // clang-format on

    cerr << "OpenglError : file=" << file << " line=" << line
         << " error:" << error << " " << info <<  endl;
    errorCode = glGetError();
  }
  return error;
}

 void glCheckBoundTexture(){
  GLint boundTexture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &boundTexture);
  std::cout << "Bound cube map ID: " << boundTexture << std::endl;
 }

 void glDumpTextureBindings() {
    GLint maxUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxUnits);

    GLint activeProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgram);

    std::cout << "Current GL Program: " << activeProgram << "\n";

    for (int unit = 0; unit < maxUnits; ++unit) {
        glActiveTexture(GL_TEXTURE0 + unit);

        GLint bound2D = 0, boundCube = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound2D);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &boundCube);

        std::cout << "Texture Unit " << unit << ": ";
        if (bound2D)
            std::cout << "2D = " << bound2D << " ";
        if (boundCube)
            std::cout << "CubeMap = " << boundCube << " ";
        if (!bound2D && !boundCube)
            std::cout << "(unbound)";
        std::cout << "\n";
    }
}