#pragma once

#include "inc_render.h"
#include "rnd_shaderman.hpp"

#include "camera.hpp"
#include "proj_scene.h"
#include "obj_moving.hpp"
//#include "obj.hpp"

namespace proj
{
  class Render: public ShaderMan
  {
  public:
    HGLRC hRC;         // Permanent Rendering Context
    HDC   hDC;         // Private GDI Device Context
    HWND  hWnd;        // Holds Our Window Handle
    HWND  dummyHWND;   // for NVidia SetPixelformat-Fix
    int width, height;

    //        bool b_PNG;

    // =================
    // Objects' pointers
    // =================
    // objects shall store VAO-id und Texture-id
    //		std::vector <obj::CObject> * p_vObjects;

    //        c_Texture aTextures[TEXCOUNT];
    //		std::vector<c_Texture> vTextures;

    //        GLuint Texture[TEXCOUNT]; // --> checken, dass nicht mit der FBO-Textur kollidiert!
    std::vector<GLuint> vGLTexture;
    // =====
    // VAO's ... VAO = {VBO1,VBO2,VB3,...}
    // =====
    /*        #define VBO_2TRIANGLES   0
    #define VBO_LEFT         1
    #define VBO_RIGHT        2
    #define VBO_ROAD         3
    #define VBO_TRAFFICSIGNS 4
    #define VBO_GUARDRAIL    5
    #define VBO_CURBSTONES   6
    #define VBO_MOVING1      7
    #define VBO_MOVING2      8
    #define VBO_CAR			 9 // <-- entfernt!!
    #define VBO_CAR_WINDOWS 10 // <-- with parts!!
    #define VBO_CAR_BODY    11
    #define VBO_CAR_TIRE1   12
    #define VBO_CAR_TIRE2   13
    */        // nur statische integrale Datentypen können innerhalb einer Klasse initialisiert werden...
    static const unsigned int VBOCOUNT = 999;

    // pointers to all VAO buffer's
    GLuint positionBuffer[VBOCOUNT];
    GLuint    colorBuffer[VBOCOUNT]; // either color or ...
    GLuint       uvBuffer[VBOCOUNT]; // texture
//    GLuint ui_numVBOpos = 0;
//    GLuint ui_numVBOtex = 0;
//    GLuint ui_numVBOcol = 0;

    std::vector<c_VAO>  vVAOs;
    std::vector<GLuint> vVertexArray;    // stores VAO's: a) Position(x,y,z), b1) color OR b2) u,v-Texture

//    std::vector<GLuint> vPositionBuffer; // stores position
#define FPS_LOWPASS 255
    float aFPS[FPS_LOWPASS];
    int idxFPS;
    bool b_splash_screen;

    bool b_culling;

    // =============
    Camera * p_cam;
    float f_camy;

    proj::Scene * p_Scene;
    
    
    obj::Moving * m_Moving[2]; // Pointer to Moving objects, as their moving coord's are applied while drawing/rendering
//    obj::CObjParts * p_car;
//    obj::CObjParts * p_car2;

    Render();
    int Init();

    HDC GL_attach_to_DC(HWND hWnd);

    GLvoid ReSizeGLScene(GLsizei width, GLsizei height); // Resize And Initialize The GL Window
//    void Init_Textures();

//    void Groundplane();
    void FPS();
    void Triangles_to_VBO(Vec3f v3pos); // most simple VBO/VAO, just in case errors happen...
    int Scene_to_VBO();//uint * p_idxVBO);
//    int DestroyScene_VBO();

//    void get_xyz_Hack(int iT, float &x, float &y, float &z, float &xto, float &yto, float &zto);

    void Bind_VBOs_to_VAOs();
    void DrawVAOs_NEU();

  private:
    static int const fbo_width = 512;
    static int const fbo_height = 512;
    GLuint fb, color, depth; // <--- nicer names!
  };
}
