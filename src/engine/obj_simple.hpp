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

//    proj::c_VAO Create(GLfloat x, GLfloat y, GLfloat z)
    proj::c_VAO Create(glm::vec3 min, glm::vec3 max)
    {
/*      std::vector<GLfloat> coords;
      float w = 2.0f;
      float h = 2.0f;
      float whalf = w / 2.0f;
      // 3D
      coords.push_back(x + whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x - whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x - whalf); coords.push_back(y); coords.push_back(z + h);

      coords.push_back(x + whalf); coords.push_back(y); coords.push_back(z);
      coords.push_back(x - whalf); coords.push_back(y); coords.push_back(z + h);
      coords.push_back(x + whalf); coords.push_back(y); coords.push_back(z + h);

      float r = 1.0f;
      float g = 1.0f;
      float b = 1.0f;
      std::vector<GLfloat> cols;
      for (int i = 0; i < 6; i++)
      {
        cols.push_back(r); cols.push_back(g); cols.push_back(b);
      }

      uint32 vCount = 6;
*/
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

      float r = 1.0f;
      float g = 1.0f;
      float b = 1.0f;
      std::vector<glm::vec3> cols;
      for (int i = 0; i < vCount; i++)
      {
        cols.push_back({ r,g,b });
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
  }; // class Groundplane

}

