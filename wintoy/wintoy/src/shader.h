#ifndef __GL_SHADER_H__
#define __GL_SHADER_H__
#include <GL/glew.h>
#include <gl/gl.h>

GLuint loadShaders(const char* vertexStr, const char* fragmentStr);

#endif