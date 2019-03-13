#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

class CIMGLoader
{
public:
  GLuint loadIMG(const char * imagepath, bool bTransparent = false);
};
