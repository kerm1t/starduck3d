
#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library

#include "obj.hpp"

namespace obj
{
  class Groundplane : public CGL_Object
  {
  public:
    //    proj::Render * p_render;

    proj::c_VAO Create()
    {
      std::vector<GLfloat> coords;
      float fPlanewidth = 300.0f;
      // 3D
      coords.push_back( fPlanewidth); coords.push_back( fPlanewidth); coords.push_back(-0.1f);
      coords.push_back(-fPlanewidth); coords.push_back( fPlanewidth); coords.push_back(-0.1f);
      coords.push_back( fPlanewidth); coords.push_back(-fPlanewidth); coords.push_back(-0.1f);

      coords.push_back( fPlanewidth); coords.push_back(-fPlanewidth); coords.push_back(-0.1f);
      coords.push_back(-fPlanewidth); coords.push_back( fPlanewidth); coords.push_back(-0.1f);
      coords.push_back(-fPlanewidth); coords.push_back(-fPlanewidth); coords.push_back(-0.1f);

      float r = 0.349f; // 89.0 / 255.0f;
      float g = 1.0f;
      float b = 0.349f;
      std::vector<GLfloat> cols;
      for (int i = 0; i < 6; i++)
      {
        cols.push_back(r); cols.push_back(g); cols.push_back(b);
      }

      uint32 vCount = 6;
      // ---------------------------
      // >>> now Push to OpenGL! >>>
      // ---------------------------
      ToVBO(vCount, &coords[0], &cols[0]);

      proj::c_VAO plane;
      plane.t_Shade = proj::SHADER_COLOR_FLAT;
      plane.Name = "plane";
      plane.uiVertexCount = vCount;
      return plane;
    }
  }; // class Groundplane

}

