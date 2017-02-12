#pragma once

#include "camera.hpp"
#include "proj_scene.h"
#include "obj_moving.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library

namespace proj
{
    enum tShading {SHADER_COLOR_FLAT,SHADER_TEXTURE};

/*
    VAO, s. http://lwjgl.org/wiki/index.php?title=The_Quad_with_DrawArrays
    explained: http://stackoverflow.com/questions/5970087/understanding-vertex-array-objects-glgenvertexarrays
    VAO (>=OGL3.0): bundle multiple VBO's for easier handling, don't need to bind ... them each
    ... really good tut: http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
*/
    class c_VAO
    {
    public:
        tShading t_Shade;
        unsigned int ui_idTexture;    // TextureID
        bool b_doDraw;
        bool b_Wireframe;
    };
    
    class c_Texture
    {
    public:
        char c_filename[255];
        float fXWorld,fYWorld;
    };

    class Render
    {
    public:
        HGLRC hRC;         // Permanent Rendering Context
        HDC   hDC;         // Private GDI Device Context
        HWND  hWnd;        // Holds Our Window Handle
        HWND  dummyHWND;   // for NVidia SetPixelformat-Fix
        int width, height;
        
        bool b_Minimal;
        bool b_PNG;
        bool b_Curbstones;
        bool b_Guardrail;

        float fBgColR;
        float fBgColG;
        float fBgColB;

        int bits_color;
        int bits_zbuffer;

        int iWireframe;

// ========
// Textures
// ========
        #define TEX_ROADSURFACE  0
        #define TEX_TRAFFICSIGN  1
        #define TEX_CAR          2

        static const unsigned int TEXCOUNT = 3;
        GLuint Texture[TEXCOUNT]; // --> checken, dass nicht mit der FBO-Textur kollidiert!

        c_Texture aTextures[TEXCOUNT];

// =====
// VAO's ... VAO = {VBO1,VBO2,VB3,...}
// =====
        #define VBO_2TRIANGLES   0
        #define VBO_LEFT         1
        #define VBO_RIGHT        2
        #define VBO_ROAD         3
        #define VBO_TRAFFICSIGNS 4
        #define VBO_GUARDRAIL    5
        #define VBO_CURBSTONES   6
        #define VBO_MOVING1      7
        #define VBO_MOVING2      8

        // nur statische integrale Datentypen können innerhalb einer Klasse initialisiert werden...
        static const unsigned int VBOCOUNT = 9;

        // pointers to all VAO buffer's
        unsigned int   vCount[VBOCOUNT]; // vertex count <-- 2do: triangle count
        GLuint    vertexArray[VBOCOUNT]; // ??
        GLuint positionBuffer[VBOCOUNT];
        GLuint    colorBuffer[VBOCOUNT]; // either color or ...
        GLuint       uvBuffer[VBOCOUNT]; // texture
       
        c_VAO aVAOs[VBOCOUNT];

// =============
// GLSL / Shader
// =============
        GLuint program;
        // std-attrib
        GLuint posAttrib;
        GLuint colAttrib;
        GLuint texAttrib; // uv

        // uniform
        GLuint offsetAttrib; // <-- for object-movement
        GLuint MVPMatrixAttrib;
        GLuint SamplerAttrib; // sampler2D
		GLuint col_texAttrib; // 0=col,1=tex

// =============
		Camera * p_cam;

        proj::Scene * m_Scene;

        obj::Moving * m_Moving[2]; // Pointer to Moving objects, as their moving coord's are applied while drawing/rendering

        Render();
        int Init();

        GLvoid ReSizeGLScene(GLsizei width, GLsizei height); // Resize And Initialize The GL Window
        void init_FBO();
        void InitShaders();

        void Init_VAOs();
        void Init_Textures();
        int CreateVBO_2Triangles(); // most simple VBO/VAO, just in case errors happen...
        int CreateVBO_Scene();
        int CreateVBO_Scene_w_o_Texture_Wrap();
        int DestroyScene_VBO();

        void get_xyz_Hack(int iT, float &x, float &y, float &z, float &xto, float &yto, float &zto);
        
        void Bind_VAOs();
        void DrawVAOs();
        
        HDC GL_attach_to_DC(HWND hWnd);

        void FBO_to_PPM(); // write out as binary PPM (with lines in reverse order)
    private:
        static int const fbo_width = 512;
        static int const fbo_height = 512;
        GLuint fb, color, depth; // <--- nicer names!
    };
}
