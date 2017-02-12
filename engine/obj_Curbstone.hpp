#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include "Vec3f.hxx"
#include "math.h"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

    // 2do: richtiges naming verwenden ui_XXX, ...
    class Curbstone
    {
    private:
        GLfloat* pf_Vertices;
        GLfloat* pf_Colors;

    public:
        int Count;  // <-- Guardrail count
        int iNdx;

        int VBOindex; // z.B. 4
        (unsigned int)* vCount; // vertex count <-- 2do: triangle count
        GLuint* vertexArray; // <-- irgendwie in const & oder so umschreiben
        GLuint* positionBuffer;
        GLuint* colorBuffer;

        Curbstone()
        {     // <-- inline, sonst Linker error!
        };    // <-- inline

        void Init(int iCount)
        {
            Count = iCount;
            iNdx = 0;

            vCount[VBOindex] = iCount*4*3; // 12 = 4 triangles, 3 vertices
            pf_Vertices = new GLfloat[vCount[VBOindex]*3]; // vertex.x/y/z
            pf_Colors = new GLfloat[vCount[VBOindex]*3]; // color.r/g/b
        }

        void Fini()
        {
            glGenBuffers(1, &positionBuffer[VBOindex]); // = 3
            glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[VBOindex]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[VBOindex]*3, pf_Vertices, GL_STATIC_DRAW); // init data storage

            glGenBuffers(1, &colorBuffer[VBOindex]); // = 3
            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[VBOindex]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[VBOindex]*3, pf_Colors, GL_STATIC_DRAW);

            delete [] pf_Colors;
            delete [] pf_Vertices;
        }
// statt Vec3f könnte man auch glm::vec3 nehmen
// glm hat auch bereits Quaternionen!!??!!
        void Add(/*int iType,*/ float x, float y, float z1, float xx, float yy, float zz1, Vec3f col)
        {
            //    GLfloat z1 = 0.0;
            GLfloat z2 = z1+0.25f;
            GLfloat zz2 = zz1+0.25f;

            int i=iNdx;
            // vertical element
            pf_Vertices[i++] = x;  pf_Vertices[i++] = y;  pf_Vertices[i++] = z1;
            pf_Vertices[i++] = xx; pf_Vertices[i++] = yy; pf_Vertices[i++] = zz1;
            pf_Vertices[i++] = x;  pf_Vertices[i++] = y;  pf_Vertices[i++] = z2;

            pf_Vertices[i++] = x;  pf_Vertices[i++] = y;  pf_Vertices[i++] = z2;
            pf_Vertices[i++] = xx; pf_Vertices[i++] = yy; pf_Vertices[i++] = zz2;
            pf_Vertices[i++] = xx; pf_Vertices[i++] = yy; pf_Vertices[i++] = zz1;

            GLfloat ax = 0; // Vec facing upward, 2do: use vector representation!
            GLfloat ay = 0;
            GLfloat az = 1;
            GLfloat _x,_y,_z,x1,y1,x2,y2;

            // Cross product
            _x = ay*(zz1-z1) - az*(yy-y);
            _y = az*(xx-x) - ax*(zz1-z1);
            _z = ax*(xx-y) - ay*(xx-x);
            GLfloat norm = sqrt(_x*_x + _y*_y + _z*_z);
            _x = _x/norm;
            _y = _y/norm;
            _z = _z/norm;

            x1 = x+0.5f*_x;
            y1 = y+0.5f*_y;
            x2 = xx+0.5f*_x;
            y2 = yy+0.5f*_y;

            // "flat" element
            pf_Vertices[i++] = x;  pf_Vertices[i++] = y;  pf_Vertices[i++] = z2;
            pf_Vertices[i++] = x1; pf_Vertices[i++] = y1; pf_Vertices[i++] = z2;
            pf_Vertices[i++] = xx; pf_Vertices[i++] = yy; pf_Vertices[i++] = zz2;

            pf_Vertices[i++] = xx; pf_Vertices[i++] = yy; pf_Vertices[i++] = zz2;
            pf_Vertices[i++] = x2; pf_Vertices[i++] = y2; pf_Vertices[i++] = zz2;
            pf_Vertices[i++] = x1; pf_Vertices[i++] = y1; pf_Vertices[i++] = z2;
//            iNdx = i;

//            GLfloat colors[4*3*3]; // 36 = 4 Triangles * 3 Vertices * 3 Coordinates
            i=iNdx;
            pf_Colors[i++] = col.x; pf_Colors[i++] = col.y; pf_Colors[i++] =  col.z;
            pf_Colors[i++] = col.x; pf_Colors[i++] = col.y; pf_Colors[i++] =  col.z;
            pf_Colors[i++] = col.x; pf_Colors[i++] = col.y; pf_Colors[i++] =  col.z;
            pf_Colors[i++] = col.x; pf_Colors[i++] = col.y; pf_Colors[i++] =  col.z;
            pf_Colors[i++] = col.x; pf_Colors[i++] = col.y; pf_Colors[i++] =  col.z;
            pf_Colors[i++] = col.x; pf_Colors[i++] = col.y; pf_Colors[i++] =  col.z;

            pf_Colors[i++] = col.x+0.1f; pf_Colors[i++] = col.y+0.1f; pf_Colors[i++] =  col.z+0.1f;
            pf_Colors[i++] = col.x+0.1f; pf_Colors[i++] = col.y+0.1f; pf_Colors[i++] =  col.z+0.1f;
            pf_Colors[i++] = col.x+0.1f; pf_Colors[i++] = col.y+0.1f; pf_Colors[i++] =  col.z+0.1f;
            pf_Colors[i++] = col.x+0.1f; pf_Colors[i++] = col.y+0.1f; pf_Colors[i++] =  col.z+0.1f;
            pf_Colors[i++] = col.x+0.1f; pf_Colors[i++] = col.y+0.1f; pf_Colors[i++] =  col.z+0.1f;
            pf_Colors[i++] = col.x+0.1f; pf_Colors[i++] = col.y+0.1f; pf_Colors[i++] =  col.z+0.1f;

// funktioniert nicht, wenn ich iNdx hier setze -->           iNdx = i;
// dann gibt es einen HEAP error
        }
//    private:
    };
}
