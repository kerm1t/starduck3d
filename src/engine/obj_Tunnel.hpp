#include "stdafx.h"
#pragma once

#include "obj.hpp"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

  // 2do: richtiges naming verwenden ui_XXX, ...
  class CTunnel : public CGL_Object
  {
  public:
    int vCount; // #vertices
    int nBufferStart;

    CTunnel()
    {     // <-- inline, sonst Linker error!
    };    // <-- inline

    void Init(int iCount)
    {
//      count = iCount;
      // 2016-07-28, 2do: indizes sharen, hier sind eigentlich nur 6 vertices erforderlich
      vCount = iCount*6*3; // 12 = 4 triangles, 3 vertices
      pf_Vert = new GLfloat[vCount*3]; // vertex.x/y/z
      pf_Col  = new GLfloat[vCount*3]; // color.r/g/b
    }

    void Add(glm::vec3 voli, glm::vec3 hili, glm::vec3 vore, glm::vec3 hire, Vec3f col)
    {
      GLfloat height = 3.25f;
//      GLfloat width  = 5.0f;

      nVert = nBufferStart;
      nCol  = nBufferStart;

      glm::vec3 volio = glm::vec3(voli.x,voli.y,voli.z + height); // vorne links oben
      glm::vec3 hilio = glm::vec3(hili.x,hili.y,hili.z + height); // hinten links oben
      glm::vec3 voreo = glm::vec3(vore.x,vore.y,vore.z + height); // vorne rechts oben
      glm::vec3 hireo = glm::vec3(hire.x,hire.y,hire.z + height); // hinten rechts oben

      // A) vertical element left --> add 6 points 
      vert_pushback(voli);
      vert_pushback(hili);
      vert_pushback(volio);
      vert_pushback(volio); // Tri 2
      vert_pushback(hili);
      vert_pushback(hilio);

      // B) vertical element right --> add 6 points 
      vert_pushback(vore);
      vert_pushback(hire);
      vert_pushback(voreo);
      vert_pushback(voreo); // Tri 2
      vert_pushback(hire);
      vert_pushback(hireo);

      // C) "upper" element
      vert_pushback(volio);
      vert_pushback(voreo);
      vert_pushback(hireo);
      vert_pushback(hireo); // Tri 2
      vert_pushback(hilio);
      vert_pushback(volio);

      // color each vertex
      for (int i=0;i<6;i++)
      {
        col_pushback(glm::vec3(col.x,col.y,col.z));
      }
      for (int i=0;i<6;i++)
      {
        col_pushback(glm::vec3(col.x,col.y,col.z));
      }
      for (int i=0;i<6;i++)
      {
        col_pushback(glm::vec3(col.x+0.1f,col.y+0.1f,col.z+0.1f));
      }
    }

    void _ToVBO()
    {
      // hier vCount statt nVert nutzen, da nVert li+re+...? beinhaltet (14400 statt 48xx) <-- klären!
      ToVBO(vCount, pf_Vert, pf_Col);
      delete[] pf_Col;
      delete[] pf_Vert;
    }

    proj::c_VAO VAO()
    {
      // --> die infos erstmal am Objekt speichern !?
      proj::c_VAO vao;
      vao.Name = "tunnel";
      vao.t_Shade = proj::SHADER_COLOR_FLAT;
      vao.uiVertexCount = nVert;
      vao.vPos = Vec3f(0.0f, 0.0f, 0.0f);
      return vao;
    }
  };
}
