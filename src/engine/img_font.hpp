// -----------------------
//
// Write text into Bitmap
//   using a Bitap Font
//
// -----------------------

#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include <stdio.h>
#include "img_bitmap.hpp"
#include "img_bitmap4.hpp"

class C_BMP_FONT: public CBMP4
{
public:
  s_bmp bmp_font;
  
  C_BMP_FONT::C_BMP_FONT()
  {
    CBMPLoader ldrBMP;
    size_t result = ldrBMP.loadBMP_to_bmp("..\\data\\buggyboy\\fnt_Sylfaen.bmp", bmp_font); // 2do: make sure, this is loaded only once!
  }

  void C_BMP_FONT::letter(char c, s_bmp4 & dst, int x, int y)
  {
    int i = (int)c - 32; // space = 32
    int xfnt = i % 8;
    int yfnt = i / 8;
    copy3(bmp_font, dst, xfnt * 32, yfnt * 32, x, y);
  }

  void C_BMP_FONT::word(std::string s, s_bmp4 & dst, int x, int y)
  {
    for (int i = 0; i < s.size(); i++)
    {
      letter(s[i], dst, x + i*16, y + 0);
    }
  }

  C_BMP_FONT::~C_BMP_FONT()
  {
    delete bmp_font.data;
  }
};
