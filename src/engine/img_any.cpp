// -----------------------
//
// Read Image into Texture
//
// -----------------------

//#pragma once
#include "img_any.h"

//#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include <stdio.h>

GLuint CIMGLoader::loadIMG(const char * imagepath)
{
  int w;
  int h;
  int chan;
  unsigned char* data;
  data = stbi_load(imagepath, &w, &h, &chan, 0);
  assert(data != NULL); // e.g. file not found
  
// tut's net ->  stbi__vertically_flip_on_load = true; // 2019-03-03, uv texture vertices upside down!

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // tut's ??

  free(data);

  return textureID;
};
