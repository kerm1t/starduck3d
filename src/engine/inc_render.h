#pragma once

#include <string>

#include "../common/types.h"

#include "Vec3f.hxx"
#include "math.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library

//#include "obj.hpp"

namespace proj
{
  // ========
  // Textures
  // ========
#define TEX_ARIALFONT    1 // war vorher 0, da gab's einen Fehler!!
#define TEX_ROADSURFACE  2
//#define TEX_TRAFFICSIGN  3
//#define TEX_CAR          4 // <-- this is actually the body
//#define TEX_CAR_WINDOWS  5
//#define TEX_CAR_TIRE     6
//#define TEX_BARRIER      7
//  static const unsigned int TEXCOUNT = 7;

  enum tShading {SHADER_COLOR_FLAT, SHADER_TEXTURE};

  /*
  VAO, s. http://lwjgl.org/wiki/index.php?title=The_Quad_with_DrawArrays
  explained: http://stackoverflow.com/questions/5970087/understanding-vertex-array-objects-glgenvertexarrays
  VAO (>=OGL3.0): bundle multiple VBO's for easier handling, don't need to bind ... them each
  ... really good tut: http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
  */
  class c_VAO //: public obj::CObject
  {
  public:
    std::string Name;
    tShading t_Shade;

/*    uint16 idVBO_pos;		// OpenGL VBO
    uint16 idVBO_tex;
    uint16 idVBO_col;
*/
    uint16 ui_idTexture;    // TextureID

    bool b_doDraw;
    bool b_Wireframe;
    Vec3f vPos;
    uint16 uiVertexCount;
    bool b_moving;

    c_VAO::c_VAO()
    {
      ui_idTexture = 0;
      b_doDraw     = true;
      b_Wireframe  = false;
      vPos         = Vec3f(0.0f,0.0f,0.0f);
      b_moving     = false;
    }
  };

  /*    
  class c_Texture
  {
  public:
  char c_filename[255];
  float fXWorld,fYWorld;
  };
  */
}