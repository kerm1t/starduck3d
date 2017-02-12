#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include "inc_render.h"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

  // 2do: richtiges naming verwenden ui_XXX, ...
  class Guardrail
  {
  private:
    GLfloat* pf_Vertices;
    GLfloat* pf_Colors;
    unsigned int ui_idVBO;
    int nVert;
    int nCol;

  public:
    proj::Render * p_Render;
    int Count;  // <-- Guardrail count
    //        int iNdx;

    int vCount; // #vertices
    int nBufferStart;

    //        int VBOindex; // z.B. 4
    //        (unsigned int)* vCount; // vertex count <-- 2do: triangle count
    //        GLuint* vertexArray; // <-- irgendwie in const & oder so umschreiben
    GLuint* positionBuffer; // pointer auf den "globalen" pos.-buffer
    GLuint* colorBuffer;

    Guardrail()
    {     // <-- inline, sonst Linker error!
    };    // <-- inline

    void Init(int iCount)
    {
      Count = iCount;
      //            iNdx = 0;
      //            nVert = 0;
      //            nCol = 0;
      ui_idVBO = p_Render->vVAOs.size();

      // 2016-07-28, 2do: indizes sharen, hier sind eigentlich nur 6 vertices erforderlich
      vCount = iCount*4*3; // 12 = 4 triangles, 3 vertices
      pf_Vertices = new GLfloat[vCount*3]; // vertex.x/y/z
      pf_Colors = new GLfloat[vCount*3]; // color.r/g/b
    }

    proj::c_VAO Fini()
    {
      // hier vCount statt nVert nutzen, da nVert li+re+...? beinhaltet (14400 statt 48xx) <-- klären!
      glGenBuffers(1, &positionBuffer[ui_idVBO]); // = 3
      glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount*3, pf_Vertices, GL_STATIC_DRAW); // init data storage

      glGenBuffers(1, &colorBuffer[ui_idVBO]); // = 3
      glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount*3, pf_Colors, GL_STATIC_DRAW);

      delete [] pf_Colors;
      delete [] pf_Vertices;

      // --> die infos erstmal am Objekt speichern !?
      proj::c_VAO vao;
      vao.t_Shade       = proj::SHADER_COLOR_FLAT;
      vao.uiVertexCount = nVert;
      vao.vPos          = Vec3f(0.0f,0.0f,0.0f);
      return vao;
    }

    void xyz_push_back(GLfloat * pf_V, glm::vec3 V)
    {
      pf_V[nVert++] = V.x;
      pf_V[nVert++] = V.y;
      pf_V[nVert++] = V.z;
    }
    void rgb_push_back(GLfloat * pf_C, glm::vec3 C)
    {
      pf_C[nCol++] = C.r;
      pf_C[nCol++] = C.g;
      pf_C[nCol++] = C.b;
    }
    void Add(glm::vec3 vo, glm::vec3 hi, Vec3f col)
    {
      GLfloat height1 = 0.6f;
      GLfloat height2 = 0.45f;
      GLfloat width   = 0.5f;

      nVert = nBufferStart; // x * 482x
      nCol = nBufferStart;

      glm::vec3 voo1 = glm::vec3(vo.x,vo.y,vo.z + height1); // vorne oben
      glm::vec3 hio1 = glm::vec3(hi.x,hi.y,hi.z + height1); // hinten oben
      glm::vec3 voo2 = glm::vec3(vo.x,vo.y,vo.z + height2); // vorne oben
      glm::vec3 hio2 = glm::vec3(hi.x,hi.y,hi.z + height2); // hinten oben

      // A) "rail" --> add 6 points 
      xyz_push_back(pf_Vertices, voo1);
      xyz_push_back(pf_Vertices, hio1);
      xyz_push_back(pf_Vertices, voo2);
      xyz_push_back(pf_Vertices, voo2); // Tri 2
      xyz_push_back(pf_Vertices, hio1);
      xyz_push_back(pf_Vertices, hio2);

      glm::vec3 mid = vo + (hi-vo)/2.0f;
      vo = mid + glm::normalize(mid-vo)*0.03f;
      hi = mid - glm::normalize(hi-mid)*0.03f;
      voo1 = glm::vec3(vo.x,vo.y,vo.z + height1); // vorne oben
      hio1 = glm::vec3(hi.x,hi.y,hi.z + height1); // hinten oben

      // A) pole --> add 6 points 
      xyz_push_back(pf_Vertices, vo);
      xyz_push_back(pf_Vertices, hi);
      xyz_push_back(pf_Vertices, voo1);
      xyz_push_back(pf_Vertices, voo1); // Tri 2
      xyz_push_back(pf_Vertices, hi);
      xyz_push_back(pf_Vertices, hio1);

      // color each vertex
      for (int i=0;i<6;i++)
      {
        rgb_push_back(pf_Colors, glm::vec3(col.x,col.y,col.z));
      }
      for (int i=0;i<6;i++)
      {
        rgb_push_back(pf_Colors, glm::vec3(col.x+0.1f,col.y+0.1f,col.z+0.1f));
      }

      // funktioniert nicht, wenn ich iNdx hier setze -->           iNdx = i;
      // dann gibt es einen HEAP error
    }
    //    private:
  };

}
