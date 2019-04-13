#include "stdafx.h"
#pragma once

#include "obj.hpp"

namespace obj
{
  class CBillboard : public CGL_Object
  {
  public:
//    static int idbb;
    //    proj::Render * p_render;

    proj::c_VAO Create(std::string sTex, GLfloat x, GLfloat y, GLfloat z)
    {
      this->name = "billboard";
      // 2do: durchfahrbare billboards (Flaggen) und nicht durchfahrbare
      //      das ist hier beim collisioncheck erstmal unerheblich
      //      ich will zumindest wissen, ob das Billboard durchfahren wurde

      std::vector<GLfloat> coords;
      float w = 2.0f;
      float h = 2.0f;
      float whalf = w / 2.0f;
      // 3D
/*      coords.push_back(x+ whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x- whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x- whalf); coords.push_back(y); coords.push_back(z+h);

      coords.push_back(x + whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x - whalf); coords.push_back(y); coords.push_back(z+h);
      coords.push_back(x + whalf); coords.push_back(y); coords.push_back(z+h);
      */
      // 90 Grad
      coords.push_back(x); coords.push_back(y + whalf); coords.push_back(z);
      coords.push_back(x); coords.push_back(y - whalf); coords.push_back(z);
      coords.push_back(x); coords.push_back(y - whalf); coords.push_back(z + h);

      coords.push_back(x); coords.push_back(y + whalf); coords.push_back(z);
      coords.push_back(x); coords.push_back(y - whalf); coords.push_back(z + h);
      coords.push_back(x); coords.push_back(y + whalf); coords.push_back(z + h);


      // bbox : ydim = 0.5f  ================== 2do: put into (abstract CGL_object class)
//      this->aabb.min_point = glm::vec3(x - whalf, y-0.5f, z);
//      this->aabb.max_point = glm::vec3(x + whalf, y+0.5f, z+h);
// 90 Grad
      this->aabb.min_point = glm::vec3(x - 0.5f, y - whalf, z);
      this->aabb.max_point = glm::vec3(x + 0.5f, y + whalf, z + h);

#if(B_ADD_BBOX_VAO == 1)
      obj::CCube2 m_cube;
      m_cube.p_render = p_render;
      proj::c_VAO vao = m_cube.Create("bbox", aabb.min_point, aabb.max_point);
      p_render->vVAOs.push_back(vao);
#endif
      // ================== 2do: put into (abstract CGL_object class)



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

//      std::vector<GLfloat> cols;
//      for (int i = 0; i < 6; i++)
//      {
//        cols.push_back(1.0f); cols.push_back(1.0f); cols.push_back(1.0f);
//      }

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
//      billy.ui_idTexture = 4;
//      billy.ui_idTexture = p_render->tex_map.find("tx_Banner")->second;
      billy.ui_idTexture = p_render->tex_map.find(sTex)->second;
      //      billy.vPos = Vec3f(x, y, z);
      return billy;
    }
  }; // class CBillboard

}

