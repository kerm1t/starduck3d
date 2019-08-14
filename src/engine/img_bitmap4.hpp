// -----------------------
//
// Read Bitmap (*.bmp) into Texture
//   with transparency!!
// -----------------------

#include "stdafx.h"
#pragma once

#include "img_bitmap.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include <stdio.h>

struct s_bmp4
{
  unsigned char header[54];  // Each BMP file begins by a 54-bytes header
  unsigned int  dataPos;     // Position in the file where the actual data begins
  unsigned int  width, height;
  unsigned int  imageSize;   // = width*height*4
                             // RGBA data
  unsigned char * data;
};

struct s_col4
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

class CBMP4
{
public:
  void BMP(s_bmp4 & bmp, int w, int h)
  {
    bmp.width = w;
    bmp.height = h;
    bmp.imageSize = bmp.width * bmp.height * 4; // rgb

    // Create a buffer
    bmp.data = new unsigned char[bmp.imageSize];

    // transparent (0) - opaque is 255
    memset(bmp.data, 0, bmp.imageSize);
  }

  void copy(const s_bmp4 & src, s_bmp4 & dst, const int xsrc, const int ysrc, const int xdst, const int ydst)
  {
    for (int x = 0; x < 32; x++)
    {
      for (int y = 0; y < 32; y++)
      {
        s_col4 col = getPixel(src, xsrc + x, ysrc + y);
        setPixel(dst, xdst + x, ydst + y, col);
      }
    }
  }

  void copy3(const s_bmp & src, s_bmp4 & dst, const int xsrc, const int ysrc, const int xdst, const int ydst) // copy from RGB-bmp to RGBA-bmp
  {
    CBMP bmp;
    for (int x = 0; x < 32; x++)
    {
      for (int y = 0; y < 32; y++)
      {
        s_col col = bmp.getPixel(src, xsrc + x, ysrc + y);
        if ((x > 0) && (y > 0))
        {
          s_col colL = bmp.getPixel(src, xsrc + x-1, ysrc + y);
          s_col colU = bmp.getPixel(src, xsrc + x, ysrc + y-1);
          if (
            ((col.r == 0) && (col.g == 0) && (col.b == 0)) // background?
            &&
            (
            ((colL.r != 0) || (colL.g != 0) || (colL.b != 0)) // left pixel set?
              ||
              ((colU.r != 0) || (colU.g != 0) || (colU.b != 0)) // above pixel set?
              )
            )
          {
            col.r = 80;
            col.g = 80;
            col.b = 80;
          }
        }
        char a = 255; // opaque
        if ((col.r == 0) && (col.g == 0) && (col.b == 0)) a = 0; // hack!!
        s_col4 col4 = { col.r, col.b, col.g, a };
        setPixel(dst, xdst + x, ydst + y, col4);
      }
    }
  }

  s_col4 getPixel(const s_bmp4 & bmp, const unsigned int x, const unsigned int y)
  {
    unsigned int _y = bmp.height - 1 - y; // hack, texture upside down? 2do: check!!
                                          //    unsigned int _y = y;
    unsigned int pos = 4 * (_y * bmp.width + x);
    //    assert(pos < imageSize);
    if (pos >= bmp.imageSize) return{ 0,0,0 };
    if (x > bmp.width) return{ 0,0,0 }; // test < 0 ?? <-- need signed then
    s_col4 col;
    col.b = bmp.data[pos];
    col.g = bmp.data[pos + 1];
    col.r = bmp.data[pos + 2];
    col.a = bmp.data[pos + 3];
    return col;
  }

  void setPixel(s_bmp4 & bmp, const unsigned int x, const unsigned int y, const s_col4 col)
  {
    unsigned int _y = bmp.height - 1 - y; // hack, texture upside down? 2do: check!!
                                        //    unsigned int _y = y;
    unsigned int pos = 4 * (_y * bmp.width + x);
    //    assert(pos < imageSize);
    if (pos >= bmp.imageSize) return;
    if (x > bmp.width) return; // test < 0 ?? <-- need signed then
    bmp.data[pos]     = col.b;
    bmp.data[pos + 1] = col.g;
    bmp.data[pos + 2] = col.r;
    bmp.data[pos + 3] = col.a;
  }

  void red(s_bmp4 & bmp) // test: draw red line
  {
    setPixel(bmp, 20, 20, { 255, 0, 0, 0 }); // transparency between 0 (trans) and 255 (opaque)
    setPixel(bmp, 21, 20, { 255, 0, 0, 0 });
    setPixel(bmp, 22, 20, { 255, 0, 0, 0 });
    setPixel(bmp, 23, 20, { 255, 0, 0, 0 });
    setPixel(bmp, 24, 20, { 255, 0, 0, 0 });
  }
  void green(s_bmp4 & bmp) // test: draw green line
  {
    setPixel(bmp, 20, 21, { 0, 255, 0, 255 });
    setPixel(bmp, 21, 21, { 0, 255, 0, 255 });
    setPixel(bmp, 22, 21, { 0, 255, 0, 255 });
    setPixel(bmp, 23, 21, { 0, 255, 0, 255 });
    setPixel(bmp, 24, 21, { 0, 255, 0, 255 });
  }

  GLuint BMP_texID(const s_bmp4 & bmp, const GLuint textureID) // bitmap is created and deleted outside
  {
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Give the image to OpenGL
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.width, bmp.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.width, bmp.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp.data);

    return textureID;
  }
};

class CBMP4Loader
{
public:
  
  size_t loadBMP_to_bmp4(const char * imagepath, s_bmp4 & bmp4) // return size in bytes
  {
    // Data read from the header of the BMP file
    size_t size;

    // Open the file
    FILE * file = fopen(imagepath, "rb");
//    assert(file);
    if (!file)
    {
      printf("Image could not be opened\n");
      return 0;
    }

    s_bmp bmp;
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
    if (bmp.imageSize == 0) bmp.imageSize = bmp.width * bmp.height * 4; // 4 : RGBA
    if (bmp.dataPos == 0)   bmp.dataPos = 54; // The BMP header is done that way

    // Create a buffer
    unsigned char * data = new unsigned char[bmp.imageSize];

    // Read the actual data from the file into the buffer
    size = fread(bmp.data, 1, bmp.imageSize, file);

    // Everything is in memory now, the file can be closed
    fclose(file);

    // now copy from RGB to RGBA
    // [...]


    delete data;
    return size;
  }

  GLuint loadBMP(const char * imagepath)
  {
    s_bmp4 bmp;

    size_t result = loadBMP_to_bmp4(imagepath, bmp);

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Give the image to OpenGL
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.width, bmp.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.width, bmp.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp.data);

    delete bmp.data;

    return textureID;
  }
};
