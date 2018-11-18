#include "stdafx.h"
#pragma once

#include "obj.hpp"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

  // 2do: richtiges naming verwenden ui_XXX, ...
  class CCurbstone : public CGL_Object
  {
    unsigned int ui_idVBO;

  public:
    proj::Render * p_render;
    int Count;  // <-- Guardrail count
    //        int iNdx;

    int vCount; // #vertices
    int nBufferStart;

    CCurbstone()
    {     // <-- inline, sonst Linker error!
    };    // <-- inline

    void Init(int iCount)
    {
      Count = iCount;
      ui_idVBO = p_render->vVAOs.size();

      // 2016-07-28, 2do: indizes sharen, hier sind eigentlich nur 6 vertices erforderlich
      vCount = iCount*4*3; // 12 = 4 triangles, 3 vertices
      pf_Vert = new GLfloat[vCount*3]; // vertex.x/y/z
      pf_Col  = new GLfloat[vCount*3]; // color.r/g/b
    }

    void Add(glm::vec3 vo, glm::vec3 hi, Vec3f col)
    {
      GLfloat height = 0.25f;
      GLfloat width  = 0.5f;

      nVert = nBufferStart; // x * 482x
      nCol = nBufferStart;

      glm::vec3 voo = glm::vec3(vo.x,vo.y,vo.z + height); // vorne oben
      glm::vec3 hio = glm::vec3(hi.x,hi.y,hi.z + height); // hinten oben

      // A) vertical element --> add 6 points 
      vert_Vpushback({ vo,hi,voo,voo,hi,hio });

      // B) "flat" element
      glm::vec3 vohi = hi-vo;  // kmöu
      glm::vec3 up = glm::vec3(0,0,1);
      glm::vec3 out = glm::cross(-vohi,up); // cross product
      out = glm::normalize(out);
      glm::vec3 voa = glm::vec3(vo.x+width*out.x,vo.y+width*out.y,vo.z + height); // vorne aussen (oben)
      glm::vec3 hia = glm::vec3(hi.x+width*out.x,hi.y+width*out.y,hi.z + height); // hinten aussen (oben)

      vert_Vpushback({ voo,voa,hio,hio,hia,voa });

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

    void ToVBO()
    {
      // hier vCount statt nVert nutzen, da nVert li+re+...? beinhaltet (14400 statt 48xx) <-- klären!
      glGenBuffers(1, &p_render->positionBuffer[ui_idVBO]); // = 3
      glBindBuffer(GL_ARRAY_BUFFER, p_render->positionBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount * 3, pf_Vert, GL_STATIC_DRAW); // init data storage

      glGenBuffers(1, &p_render->colorBuffer[ui_idVBO]); // = 3
      glBindBuffer(GL_ARRAY_BUFFER, p_render->colorBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount * 3, pf_Col, GL_STATIC_DRAW);

      delete[] pf_Col;
      delete[] pf_Vert;
    }

    proj::c_VAO VAO()
    {
      // --> die infos erstmal am Objekt speichern !?
      proj::c_VAO vao;
      vao.t_Shade = proj::SHADER_COLOR_FLAT;
      vao.uiVertexCount = nVert;
      vao.vPos = Vec3f(0.0f, 0.0f, 0.0f);
      return vao;
    }
  };
}
