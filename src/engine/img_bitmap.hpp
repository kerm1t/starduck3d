// -----------------------
//
// Read Bitmap (*.bmp) into Texture
//
// -----------------------

#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include <stdio.h>

struct s_bmp
{
  unsigned char header[54];  // Each BMP file begins by a 54-bytes header
  unsigned int  dataPos;     // Position in the file where the actual data begins
  unsigned int  width, height;
  unsigned int  imageSize;   // = width*height*3
                             // Actual RGB data
  unsigned char * data;
};

class CBMPLoader
{
public:
  // return size in bytes
  size_t loadBMP(const char * imagepath, s_bmp & bmp)
  {
    // Data read from the header of the BMP file
    size_t size;

    // Open the file
    FILE * file = fopen(imagepath, "rb");
    assert(file);
    if (!file)
    {
      printf("Image could not be opened\n");
      return 0;
    }

    if (fread(bmp.header, 1, 54, file) != 54) // If not 54 bytes read : problem
    {
      printf("Not a correct BMP file\n");
      return false;
    }

    if (bmp.header[0] != 'B' || bmp.header[1] != 'M')
    {
      printf("Not a correct BMP file\n");
      return 0;
    }

    // Read ints from the byte array
    bmp.dataPos   = *(int*)&(bmp.header[0x0A]);
    bmp.imageSize = *(int*)&(bmp.header[0x22]);
    bmp.width     = *(int*)&(bmp.header[0x12]);
    bmp.height    = *(int*)&(bmp.header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (bmp.imageSize == 0) bmp.imageSize = bmp.width * bmp.height * 3; // 3 : one byte for each Red, Green and Blue component
    if (bmp.dataPos == 0)   bmp.dataPos = 54; // The BMP header is done that way

    // Create a buffer
    bmp.data = new unsigned char[bmp.imageSize];

    // Read the actual data from the file into the buffer
    size = fread(bmp.data, 1, bmp.imageSize, file);

    // Everything is in memory now, the file can be closed
    fclose(file);

    return size;
  }

  GLuint loadBMP_texID(const char * imagepath)
  {
    s_bmp bmp;

    size_t result = loadBMP(imagepath, bmp);

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.width, bmp.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.data);

    delete bmp.data;

    return textureID;
  }
};
