#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include "math.h"

// holds guardrail-sections (from-to, side, distance)
// http://www.guetegemeinschaft-stahlschutzplanken.de/handbuecher/Einbauhandbuch-EDS2.0.pdf

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

    // 2do: richtiges naming verwenden ui_XXX, ...
    class Guardrail
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

        Guardrail()
        {     // <-- inline, sonst Linker error!
        };    // <-- inline

        void Init(int iCount) // no. of guardrails
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

        // 2do: jetzt an Ziel-Stelle berechnet, aber stattdessen Koord. transformieren m. OGL
        void Add(/*int iType, */float x, float y, float z1, float dirx, float diry, float dirz)
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
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;

            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;

// funktioniert nicht, wenn ich iNdx hier setze -->           iNdx = i;
// dann gibt es einen HEAP error
        }
        void Add2(/*int iType, */GLfloat x, GLfloat y, GLfloat z1/*, GLfloat xx, GLfloat yy, GLfloat zz1*/)
        {
            //    GLfloat z1 = 0.0;
            GLfloat z2 = z1+0.75f;
    
//            GLfloat ax = 0; // Vec facing upward, 2do: use vector representation!
//            GLfloat ay = 0;
//            GLfloat az = 1;
            GLfloat _x,_y;//,_z;

            x = x+3.5f*_x;
            y = y+3.5f*_y;

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
//            i=iNdx;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;

            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;
            pf_Colors[i++] = 0.6f; pf_Colors[i++] = 0.6f; pf_Colors[i++] =  0.6f;

// funktioniert nicht, wenn ich iNdx hier setze -->           iNdx = i;
// dann gibt es einen HEAP error
        }
//    private:
    };
}
