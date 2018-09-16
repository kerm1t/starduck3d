#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include "obj.hpp"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

  // 2do: richtiges naming verwenden ui_XXX, ...
  class Tunnel : public CGL_Object
  {
  private:
    GLfloat* pf_Vertices;
    GLfloat* pf_Colors;
    int nVert;
    int nCol;

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

  public:
    int vCount; // #vertices
    int nBufferStart;

    Tunnel()
    {     // <-- inline, sonst Linker error!
    };    // <-- inline

    void Init(int iCount)
    {
//      count = iCount;
      // 2016-07-28, 2do: indizes sharen, hier sind eigentlich nur 6 vertices erforderlich
      vCount = iCount*6*3; // 12 = 4 triangles, 3 vertices
      pf_Vertices = new GLfloat[vCount*3]; // vertex.x/y/z
      pf_Colors   = new GLfloat[vCount*3]; // color.r/g/b
    }

    void Add(glm::vec3 voli, glm::vec3 hili, glm::vec3 vore, glm::vec3 hire, Vec3f col)
    {
      GLfloat height = 3.25f;
//			GLfloat width  = 5.0f;

      nVert = nBufferStart;
      nCol  = nBufferStart;

      glm::vec3 volio = glm::vec3(voli.x,voli.y,voli.z + height); // vorne links oben
      glm::vec3 hilio = glm::vec3(hili.x,hili.y,hili.z + height); // hinten links oben
      glm::vec3 voreo = glm::vec3(vore.x,vore.y,vore.z + height); // vorne rechts oben
      glm::vec3 hireo = glm::vec3(hire.x,hire.y,hire.z + height); // hinten rechts oben

      // A) vertical element left --> add 6 points 
      xyz_push_back(pf_Vertices, voli);
      xyz_push_back(pf_Vertices, hili);
      xyz_push_back(pf_Vertices, volio);
      xyz_push_back(pf_Vertices, volio); // Tri 2
      xyz_push_back(pf_Vertices, hili);
      xyz_push_back(pf_Vertices, hilio);

      // B) vertical element right --> add 6 points 
      xyz_push_back(pf_Vertices, vore);
      xyz_push_back(pf_Vertices, hire);
      xyz_push_back(pf_Vertices, voreo);
      xyz_push_back(pf_Vertices, voreo); // Tri 2
      xyz_push_back(pf_Vertices, hire);
      xyz_push_back(pf_Vertices, hireo);

      // B) "upper" element
      xyz_push_back(pf_Vertices, volio);
      xyz_push_back(pf_Vertices, voreo);
      xyz_push_back(pf_Vertices, hireo);
      xyz_push_back(pf_Vertices, hireo); // Tri 2
      xyz_push_back(pf_Vertices, hilio);
      xyz_push_back(pf_Vertices, volio);

      // color each vertex
      for (int i=0;i<6;i++)
      {
        rgb_push_back(pf_Colors, glm::vec3(col.x,col.y,col.z));
      }
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

    void _ToVBO()
    {
      // hier vCount statt nVert nutzen, da nVert li+re+...? beinhaltet (14400 statt 48xx) <-- klären!
      ToVBO(vCount, pf_Vertices, pf_Colors);
      delete[] pf_Colors;
      delete[] pf_Vertices;
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
