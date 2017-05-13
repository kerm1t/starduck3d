#include "stdafx.h"
#include "proj_render.h"

#include "Vec3f.hxx"
//#define GLM_FORCE_RADIANS // <-- strange!
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

#include "stb_font_arial_10_usascii.inl"
static stb_fontchar fontdata[STB_SOMEFONT_NUM_CHARS];

// ---> GL_TRIANGLES
// Vertexarrays (Hauptspeicher)
// OpenGL1.1 - Displaylist (VRam d. Grafikkarte, statisch)
//      2003 - VBO (VRam + einfaches dynamisches Ändern d. Daten)

// VBO tutorial: http://www.ozone3d.net/tutorials/opengl_vbo.php
// using multiple vs. using 1 large VBO: https://www.opengl.org/discussion_boards/showthread.php/176365-Working-with-multiple-VBO-s

proj::Render::Render() // constructor
{
  hRC=NULL;                           // Permanent Rendering Context
  hDC=NULL;                           // Private GDI Device Context
//  hWnd=NULL;                          // Holds Our Window Handle

//  static unsigned char fontpixels[STB_SOMEFONT_BITMAP_HEIGHT][STB_SOMEFONT_BITMAP_WIDTH];
//  STB_SOMEFONT_CREATE(fontdata, fontpixels, STB_SOMEFONT_BITMAP_HEIGHT);

//  Init_Textures();
}

int proj::Render::Init()
{
  // only f. fixed pipeline --> glEnable(GL_TEXTURE_2D);
  glClearColor(0.3f,0.5f,1.0f,0.0f);

  glClearDepth(1.0f);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST); // <-- !
  // schneidet "zu viel" weg -->    glEnable(GL_CULL_FACE);

  InitShader1(); // <-- wenn ich das auskommentiere, dann erscheint ein weisses Rechteck oben rechts !?
  InitShader2(); // Splash screen

  b_splash_screen = true;
    
  return 0;
}

GLvoid proj::Render::ReSizeGLScene(GLsizei width, GLsizei height) // Resize And Initialize The GL Window
{
  if (height==0)                     // Prevent A Divide By Zero By
  {
    height = 1;                    // Making Height Equal One
  }
  glViewport(0, 0, width, height);   // Reset The Current Viewport
}

HDC proj::Render::GL_attach_to_DC(HWND hWnd)
{ 
  GLuint PixelFormat;                     // Holds The Results After Searching For A Match

#define ERR_HDC           0
#define ERR_PIXELFORMAT   0
#define ERR_CONTEXT       0
#define ERR_GLMAKECURRENT 0

  static PIXELFORMATDESCRIPTOR pfd=       // pfd Tells Windows How We Want Things To Be
  {
    sizeof(PIXELFORMATDESCRIPTOR),      // Size Of This Pixel Format Descriptor
    1,                                  // Version Number
    PFD_DRAW_TO_WINDOW |                // Format Must Support Window
    PFD_SUPPORT_OPENGL |                // Format Must Support OpenGL
    PFD_DOUBLEBUFFER,                   // Must Support Double Buffering
    PFD_TYPE_RGBA,                      // Request An RGBA Format
    8, /*32*/                                // framebuffer Color Depth
    0, 0, 0, 0, 0, 0,                   // Color Bits Ignored
    0,                                  // No Alpha Buffer
    0,                                  // Shift Bit Ignored
    0,                                  // No Accumulation Buffer
    0, 0, 0, 0,                         // Accumulation Bits Ignored
    16,                                 // 16Bit Z-Buffer (Depth Buffer)
    0,                                  // No (number of) bits for Stencil Buffer
    0,                                  // No bits for Auxiliary Buffer
    PFD_MAIN_PLANE,                     // Main Drawing Layer
    0,                                  // Reserved
    0, 0, 0                             // Layer Masks Ignored
  };
  if (!(hDC=GetDC(hWnd)))                 // Did We Get A Device Context?
  {
    MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_HDC;
  }
  if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) // Did Windows Find A Matching Pixel Format?
  {
    MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_PIXELFORMAT;
  }
  if(!SetPixelFormat(hDC,PixelFormat,&pfd)) // Are We Able To Set The Pixel Format?
  {
    char str[255];
    DWORD err = GetLastError();
    sprintf(str, "Can't Set The PixelFormat, err=%d", err);
    MessageBox(NULL,str,"ERROR",MB_OK|MB_ICONEXCLAMATION);

    // --> on NVidia GTX 780 error : -1073283066 = 0xc0070006
    return ERR_PIXELFORMAT;
  }

  /*  ... when running in MTS-Thread --> s. http://www.graphicsgroups.com/6-opengl/0bafc0120a809ed8.htm

  According to the Windows OpenGL documentation:

  "Setting the pixel format of a window more than once can lead to significant 
  complications for the Window Manager and for multithread applications, so it 
  is not allowed. An application can only set the pixel format of a window one 
  time. Once a window's pixel format is set, it cannot be changed."

  The card drivers that allow you do change the format are not following the 
  rules; the ones (like GeForce3) that are giving you problems are giving you 
  problems because you're violating the rules. 
  As has been suggested elsewhere, create for yourself an invisible and/or 
  off-screen dummy window, and use the rendering context you create for that 
  window as your display-list share. You can either create it from a 
  registered class with CS_OWNDC style, or you can just create a compatible DC 
  and then not delete that DC until the program is done. Either way, this will 
  be low-overhead solution, plus it should work on all compliant video 
  drivers.
  */

  if (!(hRC=wglCreateContext(hDC))) // Are We Able To Get A Rendering Context?
//  if (!(hRC=wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);
  {
    MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_CONTEXT;
  }
  if(!wglMakeCurrent(hDC,hRC)) // Try To Activate The Rendering Context
  {
    MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_GLMAKECURRENT;
  }

  ReSizeGLScene(width, height); // Set Up Our Perspective GL Screen
  return hDC;
}
/*
void proj::Render::Init_Textures()
{
  // s. proj.Init() ...
  //    aTextures[0].fXWorld = 5.0f; // [m]
  //    aTextures[0].fYWorld = 5.0f; // [m]
}
*/
/* Bind vertex buffers to VAO's
vertex buffers can hold any information: position, color, uv-coordinates
VAO's only exist from OpengL >=
*/
void proj::Render::Bind_VBOs_to_VAOs() // s. http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
{
  GLuint gi;
  GLenum err = glGetError();
  for (unsigned int iVAO = 0; iVAO < vVAOs.size(); iVAO++)
  {
    vVertexArray.push_back(gi);
  }
  glGenVertexArrays(vVAOs.size(), &vVertexArray[0]);

  // ===== 2d-VBO's (FPS shader) =====

  unsigned int iVAO = 0;
  glBindVertexArray(vVertexArray[iVAO]);  // select/bind array and
  // attach a) position and
  //        b) texture/uv-buffers
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[iVAO]);
  glVertexAttribPointer(sh2_attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // wichtig, hier das richtige Attrib (nicht 0 oder 1) zu übergeben!
  glEnableVertexAttribArray(sh2_attr_pos);
  err = glGetError();

  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // u,v-texture-coords
  err = glGetError();
  glVertexAttribPointer(sh2_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // texAttrib
  err = glGetError(); // Fehler 1281
  glEnableVertexAttribArray(sh2_attr_tex);
  err = glGetError();

  glBindVertexArray(0); // Unbind
  err = glGetError();


  // ===== 3d-VBO's (regular shader, i.e. Scene + Objects) =====

  for (unsigned int iVAO = 1; iVAO < vVAOs.size(); iVAO++) // <-- start with 1, as 0 is for FPS-coords
  {
    glBindVertexArray(vVertexArray[iVAO]);  // select/bind array and
    // attach a)  position and
    //        b1) col or
    //        b2) texture/uv-buffers
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[iVAO]);
    glVertexAttribPointer(sh1_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // wichtig, hier das richtige Attrib (nicht 0 oder 1) zu übergeben!
    glEnableVertexAttribArray(sh1_attr_pos);

    if (vVAOs[iVAO].t_Shade == SHADER_COLOR_FLAT) // flat (number of elements per Vertex = 3)
    {    
      glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[iVAO]);
      glVertexAttribPointer(sh1_attr_col, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // aeltere OpenGL-Version: glColorPointer
      glEnableVertexAttribArray(sh1_attr_col); // s. http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
    }
    else if (vVAOs[iVAO].t_Shade == SHADER_TEXTURE) // texture (number of elements per Vertex = 2)
    {
      glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // u,v-texture-coords
      glVertexAttribPointer(sh1_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
      err = glGetError();
      glEnableVertexAttribArray(sh1_attr_tex);
    }
    glBindVertexArray(0); // Unbind
  }
}

void proj::Render::FPS()
{
//#define FPS_3D
  std::vector<GLfloat> coords;
  // 3D
#ifdef FPS_3D
  coords.push_back( 0.0f); coords.push_back(0.0f); coords.push_back(0.5f);
  coords.push_back( 0.0f); coords.push_back(2.0f); coords.push_back(0.5f); 
  coords.push_back( 6.0f); coords.push_back(0.0f); coords.push_back(0.5f); 

  coords.push_back( 6.0f); coords.push_back(0.0f); coords.push_back(0.5f); 
  coords.push_back( 6.0f); coords.push_back(2.0f); coords.push_back(0.5f); 
  coords.push_back( 0.0f); coords.push_back(2.0f); coords.push_back(0.5f);
#else
  // 2D
  //         +-------+-------+ (1,1)
  //         |       |       |
  //         +-------+ (0,0) +
  //         |       |       |
  // (-1,-1) +---------------+
  //
  coords.push_back(-1.0f); coords.push_back(-1.0f);
  coords.push_back( 1.0f); coords.push_back(-1.0f);
  coords.push_back(-1.0f); coords.push_back( 1.0f);
  coords.push_back(-1.0f); coords.push_back( 1.0f);
  coords.push_back( 1.0f); coords.push_back(-1.0f);
  coords.push_back( 1.0f); coords.push_back( 1.0f);
#endif

  std::vector<GLfloat> uvs;
  uvs.push_back( 0.0f); uvs.push_back(0.0f);
  uvs.push_back( 0.0f); uvs.push_back(1.0f);
  uvs.push_back( 1.0f); uvs.push_back(0.0f);
  uvs.push_back( 1.0f); uvs.push_back(0.0f);
  uvs.push_back( 1.0f); uvs.push_back(1.0f);
  uvs.push_back( 0.0f); uvs.push_back(1.0f);

  // ---------------------------
  // >>> now Push to OpenGL! >>>
  // ---------------------------
  unsigned int ui_idVBO = vVAOs.size();
  glGenBuffers(1, &positionBuffer[ui_idVBO]);
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
#ifdef FPS_3D
  glBufferData(GL_ARRAY_BUFFER, 6*3*sizeof(GLfloat), &coords[0], GL_STATIC_DRAW);
#else
  glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(GLfloat), &coords[0], GL_STATIC_DRAW); // 2D
#endif

  glGenBuffers(1, &uvBuffer[ui_idVBO]);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[ui_idVBO]);
  glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(GLfloat), &uvs[0], GL_STATIC_DRAW);

  c_VAO fps;
  fps.t_Shade = SHADER_TEXTURE;
  fps.Name = "FPS";
  fps.ui_idTexture = TEX_ARIALFONT;
  fps.uiVertexCount = 6;
  vVAOs.push_back(fps);
}

// most simple VBO/VAO, just in case errors happen...
void proj::Render::Triangles_to_VBO(Vec3f v3pos)
{
  unsigned int ui_idVBO = vVAOs.size();

  // a) Object vertices + cols/texture
#define COORDS_PER_VERTEX 3
#define VERTICES_PER_TRI  3
#define TRIANGLES         2
#define TRI_COORDS        TRIANGLES*VERTICES_PER_TRI*COORDS_PER_VERTEX // 18 = 2 Triangles * 3 Vertices * 3 Coordinates

  std::vector<GLfloat> coords;
  coords.push_back(v3pos.x+ 0.6f); coords.push_back(v3pos.y+0.1f); coords.push_back(v3pos.z+0.0f);
  coords.push_back(v3pos.x+ 0.9f); coords.push_back(v3pos.y+0.5f); coords.push_back(v3pos.z+0.0f); 
  coords.push_back(v3pos.x+ 0.0f); coords.push_back(v3pos.y+0.7f); coords.push_back(v3pos.z+0.0f); 

  coords.push_back(v3pos.x+-0.4f); coords.push_back(v3pos.y+0.1f); coords.push_back(v3pos.z+0.0f); 
  coords.push_back(v3pos.x+ 0.4f); coords.push_back(v3pos.y+0.1f); coords.push_back(v3pos.z+0.0f); 
  coords.push_back(v3pos.x+ 0.0f); coords.push_back(v3pos.y+0.7f); coords.push_back(v3pos.z+0.0f); 

  std::vector<GLfloat> cols;
  cols.push_back(0.0f); cols.push_back(1.0f); cols.push_back(0.0f); 
  cols.push_back(1.0f); cols.push_back(1.0f); cols.push_back(0.0f); 
  cols.push_back(0.0f); cols.push_back(1.0f); cols.push_back(0.0f); 

  cols.push_back(0.0f); cols.push_back(0.0f); cols.push_back(1.0f); 
  cols.push_back(0.0f); cols.push_back(0.0f); cols.push_back(1.0f); 
  cols.push_back(0.0f); cols.push_back(0.0f); cols.push_back(1.0f); 

  // b) store VAO props for OpenGL-drawing loop (and later manipulation, e.g. position change)
  c_VAO tri;
  tri.Name = "triangle";
//  tri.b_moving = TRUE;
  tri.t_Shade = SHADER_COLOR_FLAT;
  tri.uiVertexCount = TRIANGLES*VERTICES_PER_TRI;
  vVAOs.push_back(tri);

  // ---------------------------
  // >>> now Push to OpenGL! >>>
  // ---------------------------
  glGenBuffers(1, &positionBuffer[ui_idVBO]); // a) position-buffer
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
  glBufferData(GL_ARRAY_BUFFER, TRI_COORDS*sizeof(GLfloat), &coords[0], GL_STATIC_DRAW);

  glGenBuffers(1, &colorBuffer[ui_idVBO]); // b) color-buffer
  glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[ui_idVBO]);
  glBufferData(GL_ARRAY_BUFFER, TRI_COORDS*sizeof(GLfloat), &cols[0], GL_STATIC_DRAW);

  // uvbuffer = NULL, don't try to access it!
  // ... after adding coords/cols, they can be forgotten
}

int proj::Render::Scene_to_VBO()//uint * p_idxVBO)
{
  unsigned int ui_idVBO = vVAOs.size();
  SceneParam &rc_Param = m_Scene->m_SceneLoader;

  std::string aParts[3] = {"Marker Left","Marker Right","Road"};

  unsigned int iLine;
  unsigned int iMarker;
  unsigned int sz;

  S_Point3D p0,p1,p2,p3;

  unsigned int vCount[3];
  vCount[0] = 0; // VBO_LEFT
  vCount[1] = 0; // VBO_RIGHT
  vCount[2] = 0; // VBO_ROAD
  sz = (unsigned int)rc_Param.m_c_Markers.size(); // number of marker vectors (lines)
  for (iLine=0;iLine<sz;iLine++)
  {
    const std::vector<S_MarkerPoint> &rc_Marker = rc_Param.m_c_Markers[iLine];
    for (iMarker=0;iMarker<rc_Marker.size()-1;iMarker++) // no. of markersteps (typically > 500)
    {
      if (rc_Marker[iMarker].b_Visible) vCount[iLine]+=6; // 2 triangles per Quad
    }

    GLfloat* vertices  = new GLfloat[vCount[iLine]*3]; // Vertex.x/y/z
    GLfloat* colors    = new GLfloat[vCount[iLine]*3]; // Color.r/g/b
    GLfloat* texCoords = new GLfloat[vCount[iLine]*2]; // Texture.U/V

    int iV = 0;
    int iTx = 0;

    //      [0 ..  255] <-- 2do
    //      [0 .. 4095]
    // -> [0.0 ..  1.0]
    float f_R = rc_Param.m_c_Colors[iLine].u_Red   / 4095.0f;
    float f_G = rc_Param.m_c_Colors[iLine].u_Green / 4095.0f;
    float f_B = rc_Param.m_c_Colors[iLine].u_Blue  / 4095.0f;

    //        int textureID = rc_Param.m_TextureIDs[*p_idxVBO]; // nur die "Road" hat eine texture, Left+Right nicht

    float fTexStrip = 0.0f; // store strip of texture, if not full texture is mapped to triangle
    float fTexIncr = 0.1f; // 2do <-- aus der "Höhe" des triangles berechnen!

    for (iMarker=0;iMarker<rc_Marker.size()-1;iMarker++) // no. of markersteps (typically > 500)
    {
      if (rc_Marker[iMarker].b_Visible)
      {
        /*  p3 +--+ p2     two triangles from 1 quad
               | /|
               |/ |
            p0 +--+ p1
        */
        p0 = rc_Marker[iMarker].s_Left;
        p1 = rc_Marker[iMarker].s_Right;
        p2 = rc_Marker[iMarker+1].s_Right;
        p3 = rc_Marker[iMarker+1].s_Left;
        // Farben funktionieren nicht, ich denke, man muss die Shader (vert+frag) einbauen
        // 1st Triangle
        vertices[iV] = p0.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;  // U
        vertices[iV] = p0.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;  // V
        vertices[iV] = p0.rl_Z;   colors[iV++] = f_B;
        vertices[iV] = p1.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p1.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 1.0f;
        vertices[iV] = p1.rl_Z;   colors[iV++] = f_B;
        vertices[iV] = p2.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 1.0f;
        vertices[iV] = p2.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p2.rl_Z;   colors[iV++] = f_B;
        /*
          U ^                Texture coordinates
            |
            |
            +-----> V
        */
        texCoords[iTx++] = fTexStrip + 0.0f;     // p0 - U
        texCoords[iTx++] = 0.0f;                 // p0 - V
        texCoords[iTx++] = fTexStrip + 0.0f;     // p1 - U
        texCoords[iTx++] = 1.0f;                 // p1 - V
        texCoords[iTx++] = fTexStrip + fTexIncr; // p2 - U
        texCoords[iTx++] = 1.0f;                 // p2 - V

        // 2nd Triangle
        vertices[iV] = p2.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p2.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p2.rl_Z;   colors[iV++] = f_B;
        vertices[iV] = p3.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p3.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 1.0f;
        vertices[iV] = p3.rl_Z;   colors[iV++] = f_B;
        vertices[iV] = p0.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 1.0f;
        vertices[iV] = p0.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p0.rl_Z;   colors[iV++] = f_B;

        texCoords[iTx++] = fTexStrip + fTexIncr; // p2 - U
        texCoords[iTx++] = 1.0f;                 // p2 - V
        texCoords[iTx++] = fTexStrip + fTexIncr; // p3 - U
        texCoords[iTx++] = 0.0f;                 // p3 - V
        texCoords[iTx++] = fTexStrip + 0.0f;     // p0 - U
        texCoords[iTx++] = 0.0f;                 // p0 - V
        fTexStrip += fTexIncr;
      }
    }

    // b) store VAO props for OpenGL-drawing loop (and later manipulation, e.g. position change)
    c_VAO vao;
    vao.Name = aParts[iLine];
    if (iLine==2) // 2do --> in der Scene-description speichern
    {
      vao.t_Shade = SHADER_TEXTURE;
      vao.ui_idTexture = TEX_ROADSURFACE;
    }
    else
    {
      vao.t_Shade = SHADER_COLOR_FLAT;
    }
    vao.uiVertexCount = vCount[iLine]*3;
    vVAOs.push_back(vao);

    // VBO, s. http://ogldev.atspace.co.uk/www/tutorial02/tutorial02.html
    glGenBuffers(1, &positionBuffer[ui_idVBO]);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[iLine]*3, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &colorBuffer[ui_idVBO]);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[ui_idVBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[iLine]*3, colors, GL_STATIC_DRAW);

    glGenBuffers(1, &uvBuffer[ui_idVBO]);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[ui_idVBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[iLine]*2, texCoords, GL_STATIC_DRAW);

    delete [] colors;
    delete [] vertices;
    delete [] texCoords;

    // each part of the scene (pavement, left, right marker, ...) has 1 VBO
    ui_idVBO++;
  }

  return 0;
}

/* === Move objects ===
http://www.arcsynthesis.org/gltut/positioning/Tut03%20A%20Better%20Way.html  <-- 2015-12-12, fkt. leider nicht mehr
ist jetzt hier -->
https://paroj.github.io/gltut/
bzw. hier -->
http://alfonse.bitbucket.org/oldtut/
*/
/*
int proj::Render::DestroyScene_VBO()
{
glDeleteVertexArrays(2, &vertexArray[0]);
glDeleteBuffers(2, &positionBuffer[0]);
glDeleteBuffers(2, &colorBuffer[0]);
//    glDeleteBuffers(2, &uvBuffer[0]);
return 0;
}
*/
void proj::Render::get_xyz_Hack(int iT, GLfloat &x, GLfloat &y, GLfloat &z, GLfloat &xto, GLfloat &yto, GLfloat &zto)
{
  SceneParam &rc_Param = m_Scene->m_SceneLoader;
  x = rc_Param.m_c_Trajectory[iT].s_Pos.rl_X;  // 2do Vec3f ...
  y = rc_Param.m_c_Trajectory[iT].s_Pos.rl_Y;
  z = rc_Param.m_c_Trajectory[iT].s_Pos.rl_Z + 1.34f;
  float xlook = rc_Param.m_c_Trajectory[iT+1].s_Pos.rl_X;
  float ylook = rc_Param.m_c_Trajectory[iT+1].s_Pos.rl_Y;
  float zlook = rc_Param.m_c_Trajectory[iT+1].s_Pos.rl_Z + 1.34f;
  xto = xlook;//-x;
  yto = ylook;//-y;
  zto = zlook;//-z;
}

// int proj::Render::Egopos_and_Lookat()

// rotate: https://www.opengl.org/discussion_boards/showthread.php/179290-Rotation-w-Rectangular-Pixels-(2D-VAO)

void proj::Render::DrawVAOs_NEU()
{
  /*
  Achtung, hier springt das Programm nochmal 'rein nach Drücken des Close Buttons,
  dann sind aber die vVAO-Inhalte schon beliebig "dirty"!!
  z.B. ist dann vVAOs[ui].b_moving = TRUE und die if Abfrage (s.u.) wird angesprungen
  */

  //    char buf[512];
  //    GLenum glErr;
  glm::vec3 v3;

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


//  glUseProgram(program_fps);
  /*
  [...] funktioniert noch nicht, dass 2 Shader hintereinander aktiv sind:
  der program_fps funktioniert nur (weisses Dreieck darstellen), wenn ich den "program" shader kommentiere
  */

//  glUseProgram(program);

  GLenum err = GL_NO_ERROR;

  // draw Scene + Objects
  unsigned int ui_start = 1;
  if (b_splash_screen) ui_start = 0;
  for (unsigned int ui = ui_start; ui < vVAOs.size(); ui++) // start with 1 as 0 is fps-counter
  {
    if (ui == 0)
      glUseProgram(program_fps);
    else
      glUseProgram(program);
    
    bool bMoved = false; // workaround, removed later

    if (
      ((vVAOs[ui].vPos.x < -0.001f) || (vVAOs[ui].vPos.x > 0.001f)) ||
      ((vVAOs[ui].vPos.y < -0.001f) || (vVAOs[ui].vPos.y > 0.001f)) ||
      ((vVAOs[ui].vPos.z < -0.001f) || (vVAOs[ui].vPos.z > 0.001f))
      )
    {
      bMoved = true; // workaround, removed later
      glm::mat4 Model = glm::mat4(1.0f);
      // first translate
      Model = glm::translate(Model,glm::vec3(vVAOs[ui].vPos.x,vVAOs[ui].vPos.y,vVAOs[ui].vPos.z));
      p_cam->change_Model(Model);
    }

    if (vVAOs[ui].b_moving)
    {
      v3 = m_Moving[1]->position; // <-- Bewegung muss in einer eigenen loop erfolgen, da OpenGl nur bei neu malen bewegt (ala GLUT-Idle)
      //    glUniform3f(offsetAttrib, v3.x, v3.y, v3.z);
      //ROTATE (only after offset)
      glm::mat4 Model = glm::mat4(1.0f);
      // first translate
      Model = glm::translate(Model,glm::vec3(v3.x,v3.y,v3.z));
      // then rotate
      // ! --> rotation is not "compatible" with offsetAttrib, that's why translation also done on Model matrix here
      // s. http://stackoverflow.com/questions/9920624/glm-combine-rotation-and-translation
      // Reihenfolge der Rotationen ist wichtig, s. http://gamedev.stackexchange.com/questions/73467/glm-rotating-combining-multiple-quaternions
      glm::float32 f_VehRot_Z = -atan2(m_Moving[1]->direction[0],m_Moving[1]->direction[1]);
      glm::float32 f_VehRot_Z_DEG = glm::degrees(f_VehRot_Z);
      Model = glm::rotate(Model, f_VehRot_Z_DEG, glm::vec3(0.0f,0.0f,1.0f)); // where x, y, z is axis of rotation (e.g. 0 1 0)
      //	glm::float32 f_VehTilt_DEG = 20.0f*abs(f_VehRot_Z);
      //	Model = glm::rotate(Model, f_VehTilt_DEG, glm::vec3(0.0f,1.0f,0.0f)); // where x, y, z is axis of rotation (e.g. 0 1 0)
      p_cam->change_Model(Model);
    }

    if (vVAOs[ui].b_Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (vVAOs[ui].t_Shade == SHADER_TEXTURE)
    {
      glUniform1i(sh1_unif_col_tex, 1); // shader into texture-branch

      // http://ogldev.atspace.co.uk/www/tutorial16/tutorial16.html
      glActiveTexture(GL_TEXTURE0);
      err = glGetError();
// ----------------------
// Fehler 1282 auf NVidia
// Ideen: rendern im Wireframe - hilft nicht --> konnte die Textur nicht geladen werden?
// ----------------------
      glBindTexture(GL_TEXTURE_2D, vGLTexture[vVAOs[ui].ui_idTexture-1]); // TEXTURE_ID shall be > 0 !     (-1!!)
      err = glGetError();
      
      if (ui == 0) // hack!!
        glUniform1i(sh2_unif_ID, 0); // hack!!
      else
        glUniform1i(sh1_unif_sampler, 0);
    }
    else // vVAOs[ui].t_Shade == SHADER_COLOR_FLAT
    {
      glUniform1i(sh1_unif_col_tex, 0); // shader into color-branch
    }
    err = glGetError();
    
    glBindVertexArray(vVertexArray[ui]); // <--- NVidia: hier Problem, wenn ui = 13 (beim ersten colorierten + texturierten Objekt!!)
    
    if (ui==0)
    {
//      glEnable(GL_BLEND);
//      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /*
      wenn's hier crasht, dann ist der Fehler vermutlich vorher beim buffern passiert und
      glGetError hätte etwas melden sollen!!
    */

    glDrawArrays(GL_TRIANGLES, 0, vVAOs[ui].uiVertexCount); // <-- if error is thrown here,
    err = glGetError();                                     //     it can be either positionbuffer, colorbuffer or uvbuffer
                                                            //     if t_Shade == TEXTURE,
                                                            //     then colorbuffer is NULL and vice versa!

    if (ui==0)
    {
//      glDisable(GL_BLEND);
    }
    if (vVAOs[ui].b_Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (vVAOs[ui].b_moving || bMoved)
    {
      p_cam->reset_Model();
    }
  } // for ...

  //    if (b_PNG) FBO_to_PPM();
}
