#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include "glm.hpp"
#include "math.h"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx

    // 2do: richtiges naming verwenden ui_XXX, ...
    class Moving
    {
    private:
        GLfloat* pf_Vertices;
        GLfloat* pf_Colors;
        GLfloat* pf_Textures;

    public:
        int Count;  // <-- Guardrail count
        int iNdx;

        int VBOindex; // z.B. 4
        (unsigned int)* vCount; // vertex count <-- 2do: triangle count
// -------------------------------------------------------------
// 2do: merge these and new Buffers for blender model reading !?
// -------------------------------------------------------------
        GLuint* vertexArray; // <-- irgendwie in const & oder so umschreiben
        GLuint* positionBuffer;
        GLuint* colorBuffer;
        GLuint* texBuffer;

        // reading the blender model -->
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals; // Won't be used at the moment.

        glm::vec3 position;

        Moving()
        {     // <-- inline, sonst Linker error!
            position = glm::vec3(0.0f,0.0f,0.0f); // <--- ist das ok so ?
        };    // <-- inline

        void Move(glm::vec3 vMove)
        {
            position += vMove;
        }

        void MoveTo(glm::vec3 vPosnew)
        {
            position = vPosnew;
        }

        void Init(int iCount) // no. of moving objects (should be 1, so better remove this!)
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

        void Fini_blend()
        {
            vCount[VBOindex] = vertices.size();
/*            pf_Vertices = new GLfloat[vCount[VBOindex]*3]; // vertex.x/y/z
            pf_Colors = new GLfloat[vCount[VBOindex]*3]; // color.r/g/b

            int j=0;
            for (int i=0; i<vertices.size(); i++)
            {
                pf_Vertices[j++] = vertices.at(i).x;
                pf_Vertices[j++] = vertices.at(i).y;
                pf_Vertices[j++] = vertices.at(i).z;
            }

            pf_Textures = new GLfloat[vCount[VBOindex]*2]; // texture.u/v
            j=0;
            for (int i=0; i<vertices.size(); i++)
            {
                pf_Textures[j++] = uvs.at(i).x;
                pf_Textures[j++] = uvs.at(i).y;
            }
*/
            glGenBuffers(1, &positionBuffer[VBOindex]); // = 3
            glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[VBOindex]);
//            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[VBOindex]*3, pf_Vertices, GL_STATIC_DRAW); // init data storage
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW); // init data storage

//            glGenBuffers(1, &colorBuffer[VBOindex]); // = 3
//            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[VBOindex]);
//            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[VBOindex]*3, pf_Colors, GL_STATIC_DRAW);

            glGenBuffers(1, &texBuffer[VBOindex]);
            glBindBuffer(GL_ARRAY_BUFFER, texBuffer[VBOindex]);
//            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[VBOindex]*2, pf_Textures, GL_STATIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

//            delete [] pf_Colors;
//            delete [] pf_Vertices;
//            delete [] pf_Textures;
        }

        // 2do: jetzt an Ziel-Stelle berechnet, aber stattdessen Koord. transformieren m. OGL
        void Add_Cube(/*int iType, */float x, float y, float z1, float dirx, float diry, float dirz)
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
/*
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
*/
// funktioniert nicht, wenn ich iNdx hier setze -->           iNdx = i;
// dann gibt es einen HEAP error
        }

//        void Add_BlenderObj(/*int iType, */float x, float y, float z1, float dirx, float diry, float dirz)

//        class Car

//        class Pedstrian

//        class Bicyclist
    };
}
