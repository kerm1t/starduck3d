#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

//#define GLM_FORCE_RADIANS // disabled ... distorts the projection
#include "glm/glm.hpp"
#include "math.h"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

  // 2do: richtiges naming verwenden ui_XXX, ...
  class Moving
  {
  private:
    GLfloat* pf_Vertices;
    GLfloat* pf_Colors;
    GLfloat* pf_UVs;

  public:
    int Count;  // <-- Guardrail count
    int iNdx;
    int iT; // for running on the trajectory
    int iTspeed;

    int VBOindex; // z.B. 4
    ////        (unsigned int)* vCount; // vertex count <-- 2do: triangle count
    int nVertices;
    // -------------------------------------------------------------
    // 2do: merge these and new Buffers for blender model reading !?
    // -------------------------------------------------------------
    //        GLuint* vertexArray; // <-- irgendwie in const & oder so umschreiben
    GLuint* positionBuffer;
    GLuint* colorBuffer;
    GLuint* uvBuffer;    // unschön, das gesamte Array zu übergeben!!! 2do

    // reading the blender model -->
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals; // Won't be used at the moment.

    glm::vec3 position;
    glm::vec3 direction; // position - prev.position

    Moving()
    {     // <-- inline, sonst Linker error!
      position = glm::vec3(0.0f,0.0f,0.0f); // <--- ist das ok so ?
      //            direction = glm::vec3(1.0f,0.0f,0.0f); // <--- ist das ok so ?
      iTspeed = 0;
      iT = 0;
    };    // <-- inline

    void Move(glm::vec3 vMove)
    {
      direction = vMove;
      //            direction = (position+vMove)-position;
      position += vMove;
    }

    void MoveTo(glm::vec3 vPosnew)
    {
      direction = vPosnew-position;
      position = vPosnew;
    }

    void Init(int iCount) // no. of moving objects (should be 1, so better remove this!)
    {
      Count = iCount;
      iNdx = 0;

      ////            vCount[VBOindex] = iCount*4*3; // 12 = 4 triangles, 3 vertices
      nVertices = iCount*4*3; // 12 = 4 triangles, 3 vertices
      pf_Vertices = new GLfloat[nVertices*3]; // vertex.x/y/z
      pf_Colors = new GLfloat[nVertices*3]; // color.r/g/b

      // uvs = vector, mem allocated in mdl_blender
    }

    void Fini()
    {
      glGenBuffers(1, &positionBuffer[VBOindex]); // = 3
      glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[VBOindex]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVertices*3, pf_Vertices, GL_STATIC_DRAW); // init data storage

      glGenBuffers(1, &colorBuffer[VBOindex]); // = 3
      glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[VBOindex]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVertices*3, pf_Colors, GL_STATIC_DRAW);

      delete [] pf_Colors;
      delete [] pf_Vertices;
    }

    void Fini_blend() // blender object
    {
      // -----------  copy ... maybe there is something easier as reinterpret_cast (s.u. hehe)
      /*			int nUV = uvs.size();
      pf_UVs = new GLfloat[nUV*2]; // Texture.U/V
      int j=0;
      for (unsigned int i=0; i<uvs.size(); i++)
      {
      pf_UVs[j++] = uvs[i].x;
      pf_UVs[j++] = uvs[i].y;
      }*/
      // -----------
      ////			vCount[VBOindex] = vertices.size();

      glGenBuffers(1, &positionBuffer[VBOindex]); // = 3
      glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[VBOindex]);
      glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW); // init data storage

      glGenBuffers(1, &uvBuffer[VBOindex]); // = uv-Buffer
      glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[VBOindex]);
      glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    }

    // 2do: jetzt an Ziel-Stelle berechnet, aber stattdessen Koord. transformieren m. OGL
    void Add_Cube(float x, float y, float z1, float dirx, float diry, float dirz)
    {
      //    GLfloat z1 = 0.0;
      GLfloat z2 = z1+0.75f;

      GLfloat ax = 0; // Vec facing upward, 2do: use vector representation!
      GLfloat ay = 0;
      GLfloat az = 1;
      GLfloat _x,_y,_z;

      // Cross product
      _x = ay*dirz - az*diry;
      _y = az*dirx - ax*dirz;
      _z = ax*diry - ay*dirx;
      GLfloat norm = sqrt(_x*_x + _y*_y + _z*_z);
      _x = _x/norm;
      _y = _y/norm;
      _z = _z/norm;

      x = x;//+3.5*_x;
      y = y;//+3.5*_y;

      int i=iNdx;
      // vertical element
      pf_Vertices[i++] = x-0.05f*_x; pf_Vertices[i++] = y-0.05f*_y; pf_Vertices[i++] = z1;
      pf_Vertices[i++] = x-0.05f*_x; pf_Vertices[i++] = y-0.05f*_y; pf_Vertices[i++] = z2;
      pf_Vertices[i++] = x+0.05f*_x; pf_Vertices[i++] = y+0.05f*_y; pf_Vertices[i++] = z1;

      pf_Vertices[i++] = x+0.05f*_x; pf_Vertices[i++] = y+0.05f*_y; pf_Vertices[i++] = z1;
      pf_Vertices[i++] = x+0.05f*_x; pf_Vertices[i++] = y+0.05f*_y; pf_Vertices[i++] = z2;
      pf_Vertices[i++] = x-0.05f*_x; pf_Vertices[i++] = y-0.05f*_y; pf_Vertices[i++] = z2;

      // horizontal element
      pf_Vertices[i++] = x-1.5f*_x; pf_Vertices[i++] = y-1.5f*_y; pf_Vertices[i++] = z2;
      pf_Vertices[i++] = x-1.5f*_x; pf_Vertices[i++] = y-1.5f*_y; pf_Vertices[i++] = z2-0.3f;
      pf_Vertices[i++] = x+1.5f*_x; pf_Vertices[i++] = y+1.5f*_y; pf_Vertices[i++] = z2;

      pf_Vertices[i++] = x+1.5f*_x; pf_Vertices[i++] = y+1.5f*_y; pf_Vertices[i++] = z2;
      pf_Vertices[i++] = x+1.5f*_x; pf_Vertices[i++] = y+1.5f*_y; pf_Vertices[i++] = z2-0.3f;
      pf_Vertices[i++] = x-1.5f*_x; pf_Vertices[i++] = y-1.5f*_y; pf_Vertices[i++] = z2-0.3f;
      //            iNdx = i;

      //            GLfloat colors[4*3*3]; // 36 = 4 Triangles * 3 Vertices * 3 Coordinates
      i=iNdx;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;

      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      pf_Colors[i++] = 1.f; pf_Colors[i++] = 1.f; pf_Colors[i++] =  1.f;
      // funktioniert nicht, wenn ich iNdx hier setze -->           iNdx = i;
      // dann gibt es einen HEAP error
    }

    //        void Add_BlenderObj(/*int iType, */float x, float y, float z1, float dirx, float diry, float dirz)

    //        class Car

    //        class Pedstrian

    //        class Bicyclist
  };
}
