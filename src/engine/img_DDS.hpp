// -----------------------
//
// Read Compressed Texture (DDS = Direct Draw Surface)
//
// from:  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
//
// -----------------------

#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include <stdio.h>

#include <MMSystem.h>
// s. http://www.gamedev.net/topic/342580-dds-fileformat/
#define FOURCC_DXT1  MAKEFOURCC('D', 'X', 'T', '1')
#define FOURCC_DXT3  MAKEFOURCC('D', 'X', 'T', '3')
#define FOURCC_DXT5  MAKEFOURCC('D', 'X', 'T', '5')

GLuint loadDDS(const char * imagepath)
{ 
  unsigned char header[124];

  // Open the file
  FILE * fp = fopen(imagepath, "rb"); // 2do: durch C++ Funktionen (ifstream) ersetzen, s. mdl_wavefront
  if (fp == NULL)
    return 0;

  // Verify the type of file
  char filecode[4];
  fread(filecode, 1, 4, fp);
  if (strncmp(filecode, "DDS ", 4) != 0) {
    fclose(fp);
    return 0;
  }

  // get the surface desc
  fread(&header, 124, 1, fp); 

  unsigned int height      = *(unsigned int*)&(header[ 8]);
  unsigned int width       = *(unsigned int*)&(header[12]);
  unsigned int linearSize  = *(unsigned int*)&(header[16]);
  unsigned int mipMapCount = *(unsigned int*)&(header[24]);
  unsigned int fourCC      = *(unsigned int*)&(header[80]);

  unsigned char * buffer;
  unsigned int bufsize;
  // how big is it going to be including all mipmaps?
  bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
  buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
  fread(buffer, 1, bufsize, fp);
  // close the file pointer
  fclose(fp);

  unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
  unsigned int format;
  switch(fourCC)
  {
  case FOURCC_DXT1:
    format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    break;
  case FOURCC_DXT3:
    format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    break;
  case FOURCC_DXT5:
    format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    break;
  default:
    free(buffer);
    return 0;
  }

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1); // ADDED FROM TUT
  unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
  unsigned int offset = 0;

  // load the mipmaps
  for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
  {
    unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
    glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

    offset += size;
    width  /= 2;
    height /= 2;
  }
  free(buffer); 

  return textureID;
}
