#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include "inc_render.h"
#include <vector> // später proj::render übergeben und das hier entfernen

// holds all trafficsigns, so they can be switched on/off, added, deleted
// traffic-signs are immobile
// - triangle
// - quad
// textured !?

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

  // 2do: richtiges naming verwenden ui_XXX, ...
  class Trafficsign
  {
  public:
    int Count;  // <-- Trafficsign count
    //        int triCount; // <-- triangle count
    int vCount;   // <-- vertex count

    ////        int VBOindex; // z.B. 3
    //        (unsigned int)* vCount; // vertex count <-- 2do: triangle count
    //        GLuint* vertexArray; // <-- irgendwie in const & oder so umschreiben
    GLuint* positionBuffer;
    GLuint* colorBuffer;

    Trafficsign()
    {     // <-- inline, sonst Linker error!
    };    // <-- inline

    // 2do: jetzt an Ziel-Stelle berechnet, aber stattdessen Koord. transformieren m. OGL
    proj::c_VAO Add(std::vector<proj::c_VAO> & vVAOs,/*int iType, */GLfloat x, GLfloat y, GLfloat z1, GLfloat dirx, GLfloat diry, GLfloat dirz)
    {
      unsigned int ui_idVBO = vVAOs.size();

      vCount = 3*3; // 9 = 3 triangles, 3 vertices
      //            *(vCount+VBOindex) = 3*3;

      GLfloat Vertices[3*3*3]; // <-- vertex count, fix, das muss mit memset dynamisch definiert werden!

      /*            if (iType==0) // quad-ratic
      {
      triCount = 4; // <-- triangle count
      //        GLfloat Vertices[4*3]; // <-- vertex count
      }
      else if (iType==1) // tri-angluar
      {
      triCount = 3; // Vertices
      //        GLfloat Vertices[3*3];
      }
      */
      //    GLfloat z1 = 0.0;
      GLfloat z2 = z1+1.7f;

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

      x = x+3.5f*_x;
      y = y+3.5f*_y;

      int i=0;
      Vertices[i++] = x-0.05f*_x; Vertices[i++] = y-0.05f*_y; Vertices[i++] = z1;
      Vertices[i++] = x-0.05f*_x; Vertices[i++] = y-0.05f*_y; Vertices[i++] = z2;
      Vertices[i++] = x+0.05f*_x; Vertices[i++] = y+0.05f*_y; Vertices[i++] = z1;

      Vertices[i++] = x+0.05f*_x; Vertices[i++] = y+0.05f*_y; Vertices[i++] = z1;
      Vertices[i++] = x+0.05f*_x; Vertices[i++] = y+0.05f*_y; Vertices[i++] = z2;
      Vertices[i++] = x-0.05f*_x; Vertices[i++] = y-0.05f*_y; Vertices[i++] = z2;

      //            if (iType==1)
      //            {
      Vertices[i++] = x-0.5f*_x; Vertices[i++] = y-0.5f*_y; Vertices[i++] = z2;
      Vertices[i++] = x;         Vertices[i++] = y;         Vertices[i++] = z2+0.5f;//0.25;
      Vertices[i++] = x+0.5f*_x; Vertices[i++] = y+0.5f*_y; Vertices[i++] = z2;
      //            }

      GLfloat colors[3*3*3]; // 27 = 3 Triangles * 3 Vertices * 3 Coordinates
      i=0;
      colors[i++] = 0.6f; colors[i++] = 0.6f; colors[i++] =  0.6f;
      colors[i++] = 0.6f; colors[i++] = 0.6f; colors[i++] =  0.6f;
      colors[i++] = 0.6f; colors[i++] = 0.6f; colors[i++] =  0.6f;
      colors[i++] = 0.6f; colors[i++] = 0.6f; colors[i++] =  0.6f;
      colors[i++] = 0.6f; colors[i++] = 0.6f; colors[i++] =  0.6f;
      colors[i++] = 0.6f; colors[i++] = 0.6f; colors[i++] =  0.6f;

      colors[i++] = 1.0f; colors[i++] = 0.0f; colors[i++] =  0.0f;
      colors[i++] = 1.0f; colors[i++] = 0.0f; colors[i++] =  0.0f;
      colors[i++] = 1.0f; colors[i++] = 0.0f; colors[i++] =  0.0f;

      // --> die infos erstmal am Objekt speichern !?
      proj::c_VAO vao;
      vao.Name          = "trafficsign";
      vao.t_Shade       = proj::SHADER_COLOR_FLAT;
      vao.uiVertexCount = vCount;

      glGenBuffers(1, &positionBuffer[ui_idVBO]);
      glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount*3, Vertices, GL_STATIC_DRAW); // init data storage

      glGenBuffers(1, &colorBuffer[ui_idVBO]);
      glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount*3, colors, GL_STATIC_DRAW);

      return vao;
    }
  private:
  };
}
