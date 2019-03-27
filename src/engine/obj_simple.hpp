#pragma once
#include "stdafx.h"
#pragma once

#include "obj.hpp"

namespace obj
{
  class CCube : public CGL_Object
  {
  public:
    //    proj::Render * p_render;

    proj::c_VAO Create(glm::vec3 min, glm::vec3 max)
    {
      std::vector<glm::vec3> coords;
      coords.push_back({ min.x,min.y,min.z });
      coords.push_back({ min.x,max.y,min.z });
      coords.push_back({ max.x,max.y,min.z });

      coords.push_back({ min.x,min.y,min.z });
      coords.push_back({ max.x,max.y,min.z });
      coords.push_back({ max.x,min.y,min.z });

      coords.push_back({ min.x,min.y,max.z });
      coords.push_back({ min.x,max.y,max.z });
      coords.push_back({ max.x,max.y,max.z });

      coords.push_back({ min.x,min.y,max.z });
      coords.push_back({ max.x,max.y,max.z });
      coords.push_back({ max.x,min.y,max.z });
      
      uint32 vCount = 12;

      std::vector<glm::vec3> cols;
      for (unsigned int i = 0; i < vCount; i++)
      {
        cols.push_back({ 1.0f, 1.0f, 1.0f }); // r,g,b
      }

      // ---------------------------
      // >>> now Push to OpenGL! >>>
      // ---------------------------
      ToVBO(vCount, &coords[0], &cols[0]);

      proj::c_VAO cube;
      cube.t_Shade = proj::SHADER_COLOR_FLAT;
      cube.Name = "cube";
      cube.uiVertexCount = vCount;
      return cube;
    }
  }; // class CCube

}

