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

//    proj::c_VAO Create(std::string sTex, GLfloat x, GLfloat y, GLfloat z)
    proj::c_VAO Create(std::string sTex, glm::vec3 pos, glm::vec3 dir, GLfloat w = 2.0f) // draw orthogonal to dir
    {
      this->name = "billboard";
      // 2do: durchfahrbare billboards (Flaggen) und nicht durchfahrbare
      //      das ist hier beim collisioncheck erstmal unerheblich
      //      ich will zumindest wissen, ob das Billboard durchfahren wurde

      std::vector<GLfloat> coords;
      float h = 2.0f;
      float whalf = w / 2.0f;

      glm::vec3 orth = glm::normalize(glm::cross(dir, glm::vec3(0,0,1)));
      glm::vec3 li = pos + orth*whalf;
      glm::vec3 re = pos - orth*whalf;
      
      coords.push_back(li.x); coords.push_back(li.y); coords.push_back(li.z);
      coords.push_back(li.x); coords.push_back(li.y); coords.push_back(li.z+h);
      coords.push_back(re.x); coords.push_back(re.y); coords.push_back(re.z+h);

      coords.push_back(li.x); coords.push_back(li.y); coords.push_back(li.z);
      coords.push_back(re.x); coords.push_back(re.y); coords.push_back(re.z+h);
      coords.push_back(re.x); coords.push_back(re.y); coords.push_back(re.z);


      // bbox : ydim = 0.5f  ================== 2do: put into (abstract CGL_object class)
//      this->aabb.min_point = glm::vec3(li.x+0.5f, li.y, li.z);
//      this->aabb.max_point = glm::vec3(re.x-0.5f, re.y, re.z + h);
      li += glm::normalize(dir);
      re -= glm::normalize(dir);
      this->aabb.min_point = glm::vec3(li.x, li.y, li.z);
      this->aabb.max_point = glm::vec3(re.x, re.y, re.z + h);

#if(B_ADD_BBOX_VAO == 1)
      obj::CCube2 m_cube;
      m_cube.p_render = p_render;
      proj::c_VAO vao = m_cube.Create("bbox", aabb.min_point, aabb.max_point);
      p_render->vVAOs.push_back(vao);
#endif
      // ================== 2do: put into (abstract CGL_object class)



      std::vector<glm::vec2> uv;

      uv.push_back({ 0.0f, 1.0f });  // upside down!
      uv.push_back({ 0.0f, 0.0f });
      uv.push_back({ 1.0f, 0.0f });

      uv.push_back({ 0.0f, 1.0f });
      uv.push_back({ 1.0f, 0.0f });
      uv.push_back({ 1.0f, 1.0f });

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

