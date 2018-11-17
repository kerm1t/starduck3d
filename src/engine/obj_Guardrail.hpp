#include "stdafx.h"
#pragma once

#include "obj.hpp"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

  class CGuardrail: public CGL_Object
  {
  public:
    uint16 count;         // <-- Guardrail count
    uint32 vCount;        // #vertices
    uint32 nBufferStart;  // for multiple Add's

    CGuardrail()
    {     // <-- inline, sonst Linker error!
    };    // <-- inline

    void Init(int iCount)
    {
      count = iCount;

      // 2016-07-28, 2do: indizes sharen, hier sind eigentlich nur 6 vertices erforderlich
      vCount = iCount*4*3; // 12 = 4 triangles, 3 vertices
      pf_Vert = new GLfloat[vCount*3]; // vertex.x/y/z
      pf_Col   = new GLfloat[vCount*3]; // color.r/g/b
    }

    void Add(glm::vec3 vo, glm::vec3 hi, Vec3f col)
    {
      GLfloat height1 = 0.6f;
      GLfloat height2 = 0.45f;
      GLfloat width   = 0.5f;

      nVert = nBufferStart; // x * 482x
      nCol  = nBufferStart;

      glm::vec3 voo1 = glm::vec3(vo.x,vo.y,vo.z + height1); // vorne oben
      glm::vec3 hio1 = glm::vec3(hi.x,hi.y,hi.z + height1); // hinten oben
      glm::vec3 voo2 = glm::vec3(vo.x,vo.y,vo.z + height2); // vorne oben
      glm::vec3 hio2 = glm::vec3(hi.x,hi.y,hi.z + height2); // hinten oben

      // A) "rail" --> add 6 points 
      vert_pushback(voo1);
      vert_pushback(hio1);
      vert_pushback(voo2);
      vert_pushback(voo2); // Tri 2
      vert_pushback(hio1);
      vert_pushback(hio2);

      glm::vec3 mid = vo + (hi-vo)/2.0f;
      vo = mid + glm::normalize(mid-vo)*0.03f;
      hi = mid - glm::normalize(hi-mid)*0.03f;
      voo1 = glm::vec3(vo.x,vo.y,vo.z + height1); // vorne oben
      hio1 = glm::vec3(hi.x,hi.y,hi.z + height1); // hinten oben

      // A) pole --> add 6 points 
      vert_pushback(vo);
      vert_pushback(hi);
      vert_pushback(voo1);
      vert_pushback(voo1); // Tri 2
      vert_pushback(hi);
      vert_pushback(hio1);

      // color each vertex
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
      ToVBO(vCount, pf_Vert, pf_Col);
      delete[] pf_Col; // need to free here already (other than in obj destructor)?
      delete[] pf_Vert;
    }

    proj::c_VAO VAO()
    {
      // --> die infos erstmal am Objekt speichern !?
      proj::c_VAO vao;
      vao.Name = "guardrail";
      vao.t_Shade = proj::SHADER_COLOR_FLAT;
      vao.uiVertexCount = nVert;
      vao.vPos = Vec3f(0.0f, 0.0f, 0.0f);
      return vao;
    }

  };
}
