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

class C_BMP_FONT
{
public:
  s_bmp bmp_font;
  
  C_BMP_FONT::C_BMP_FONT()
  {
    CBMPLoader ldrBMP;
    size_t result = ldrBMP.loadBMP_to_bmp("..\\data\\buggyboy\\fnt_Sylfaen.bmp", bmp_font);
  }

  C_BMP_FONT::~C_BMP_FONT()
  {
    delete bmp_font.data;
  }

};
