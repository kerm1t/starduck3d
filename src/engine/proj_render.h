#pragma once

#include "inc_render.h"
#include "rnd_shaderman.hpp"

#include "camera.hpp"
#include "proj_scene.h"
#include "obj_moving.hpp"
//#include "obj.hpp"

#include <map>

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

    glm::vec3 Cursor;  // 2do: move to editor.hpp !!
    glm::vec3 Scenepos[4];  // 2do: move to scene....hpp !!

    int touch_object_vaoId; // (not fully) redundant to proj.touch_object_id

    //        bool b_PNG;

    // =================
    // Objects' pointers
    // =================
    // objects shall store VAO-id und Texture-id
    //		std::vector <obj::CObject> * p_vObjects;

    std::map < std::string, GLuint> tex_map;

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
    static const unsigned int VBOCOUNT = 9999;

    // pointers to all VAO buffer's
    GLuint positionBuffer[VBOCOUNT];
    GLuint    colorBuffer[VBOCOUNT]; // either color or ...
    GLuint       uvBuffer[VBOCOUNT]; // texture
//    GLuint ui_numVBOpos = 0;
//    GLuint ui_numVBOtex = 0;
//    GLuint ui_numVBOcol = 0;

    std::vector<c_VAO>  vVAOs;
    std::vector<GLuint> vVertexArray;    // stores VAO's: a) Position(x,y,z), b1) color OR b2) u,v-Texture

    GLuint vao2;
    GLuint positionBuf2;
    GLuint    colorBuf2; // either color or ...
//    GLuint       uvBuf2; // texture

    GLuint vao3;
    GLuint positionBuf3;
    GLuint    colorBuf3; // either color or ... (texture)
                         
//    std::vector<GLuint> vPositionBuffer; // stores position
#define FPS_LOWPASS 255
    float aFPS[FPS_LOWPASS];
    int idxFPS;
    bool b_splash_screen;

    // global, i.e. not per object
    bool b_solid;
    bool b_wireframe;
    bool b_culling;

    // =============
    Camera * p_cam;
    float f_camy;

    proj::Scene * p_Scene;

    Render();
    int Init();

    HDC GL_attach_to_DC(HWND hWnd);

    GLvoid ReSizeGLScene(GLsizei width, GLsizei height); // Resize And Initialize The GL Window
//    void Init_Textures();

//    void Groundplane();
    void FPS();

    int Scene_to_VBO();//uint * p_idxVBO);
//    int DestroyScene_VBO();

    void Bind_NEW__VBOs_to_VAOs(int ui); // AHck!!
    void Bind_VBOs_to_VAOs();
    void DrawVAOs_NEU();

  private:
    static int const fbo_width = 512;
    static int const fbo_height = 512;
    GLuint fb, color, depth; // <--- nicer names!
  };
}
