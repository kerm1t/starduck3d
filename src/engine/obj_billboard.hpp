#include "stdafx.h"
#pragma once

#include "obj.hpp"

namespace obj
{
  class CBillboard : public CGL_Object
  {
  public:
    //    proj::Render * p_render;

    proj::c_VAO Create(GLfloat x, GLfloat y, GLfloat z)
    {
      std::vector<GLfloat> coords;
      float w = 2.0f;
      float h = 2.0f;
      float whalf = w / 2.0f;
      // 3D
      coords.push_back(x+ whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x- whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x- whalf); coords.push_back(y); coords.push_back(z+h);

      coords.push_back(x + whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x - whalf); coords.push_back(y); coords.push_back(z+h);
      coords.push_back(x + whalf); coords.push_back(y); coords.push_back(z+h);

      std::vector<glm::vec2> uv;
/*      uv.push_back({1.0f, 0.0f});
      uv.push_back({0.0f, 0.0f});
      uv.push_back({0.0f, 1.0f});

      uv.push_back({1.0f, 0.0f});
      uv.push_back({0.0f, 1.0f});
      uv.push_back({1.0f, 1.0f});
*/   // updside down, as imgANY import turns texture upside down (fix that later!)
      uv.push_back({ 1.0f, 1.0f });
      uv.push_back({ 0.0f, 1.0f });
      uv.push_back({ 0.0f, 0.0f });

      uv.push_back({ 1.0f, 1.0f });
      uv.push_back({ 0.0f, 0.0f });
      uv.push_back({ 1.0f, 0.0f });

      std::vector<GLfloat> cols;
      for (int i = 0; i < 6; i++)
      {
        cols.push_back(1.0f); cols.push_back(1.0f); cols.push_back(1.0f);
      }

      uint32 vCount = 6;
      // ---------------------------
      // >>> now Push to OpenGL! >>>
      // ---------------------------
//      ToVBO(vCount, &coords[0], &cols[0]);
      ToVBOTex(vCount, &coords[0], &uv[0]);

      proj::c_VAO billy;
      billy.t_Shade = proj::SHADER_TEXTURE;
      billy.Name = "billyboard";
      billy.uiVertexCount = vCount;
      billy.ui_idTexture = 4;
      return billy;
    }
  }; // class Groundplane

}

