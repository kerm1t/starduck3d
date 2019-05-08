#include "stdafx.h"

#include "proj_render.h"

#include "Vec3f.hxx"
#define GLM_FORCE_RADIANS // <-- strange!
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

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
  b_solid = true;
  b_wireframe = true; // 2do: only debugging: set to default
}

int proj::Render::Init()
{
  // only f. fixed pipeline --> glEnable(GL_TEXTURE_2D);
  glClearColor(0.3f,0.5f,1.0f,0.0f);

  glClearDepth(1.0f);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST); // <-- !
  // schneidet "zu viel" weg -->    glEnable(GL_CULL_FACE);
  // falsch!
  // um Culling zu nutzen, müssen alle Triangles in derselben Order angelegt werden
  // 2/4/2019 fixed: groundplane
///  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT); // 2do: die Strassen-texturen andersherum (ccw oder cw) an die GPU übergeben
//  glFrontFace(GL_CCW);

  InitShader1(); // <-- wenn ich das auskommentiere, dann erscheint ein weisses Rechteck oben rechts !?
  InitShader2(); // Splash screen

  b_splash_screen = true;

  return 0;
}

GLvoid proj::Render::ReSizeGLScene(GLsizei width, GLsizei height) // Resize And Initialize The GL Window
{
  if (height==0)                     // Prevent A Divide By Zero By
  {
    height = 1;                      // Making Height Equal One
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
  if (!SetPixelFormat(hDC,PixelFormat,&pfd)) // Are We Able To Set The Pixel Format?
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

  if (!(hRC=wglCreateContext(hDC)))
  {
    MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_CONTEXT;
  }
  if (!wglMakeCurrent(hDC,hRC)) // Try To Activate The Rendering Context
  {
    MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_GLMAKECURRENT;
  }

  ReSizeGLScene(width, height); // Set Up Our Perspective GL Screen
  return hDC;
}

// HACK!!!
void proj::Render::Bind_NEW__VBOs_to_VAOs(int ui) // s. http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
{
//  GLuint gi;
  GLenum err = glGetError();
  for (unsigned int iVAO = ui; iVAO < vVAOs.size(); iVAO++)
  {
    vVertexArray.push_back(iVAO);
  }
  glGenVertexArrays(vVAOs.size()-ui, &vVertexArray[ui]);

  // ===== 3d-VBO's (regular shader, i.e. Scene + Objects) =====

  for (unsigned int iVAO = ui; iVAO < vVAOs.size(); iVAO++) // <-- start with 1, as 0 is for FPS-coords
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

  for (iVAO = 1; iVAO < vVAOs.size(); iVAO++) // <-- start with 1, as 0 is for FPS-coords
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
  coords.push_back(-1.0f); coords.push_back( 1.0f);
  coords.push_back(1.0f); coords.push_back(-1.0f);
  coords.push_back(-1.0f); coords.push_back( 1.0f);
  coords.push_back( 1.0f); coords.push_back( 1.0f);
  coords.push_back(1.0f); coords.push_back(-1.0f);
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

int proj::Render::Scene_to_VBO()//uint * p_idxVBO)
{
  unsigned int ui_idVBO = vVAOs.size();
  SceneParam &rc_Param = p_Scene->m_SceneLoader;

  unsigned int iLine;
  unsigned int iMarker;
  unsigned int sz;

  S_Point3D p0,p1,p2,p3;
  sz = (unsigned int)rc_Param.m_c_Markers.size(); // number of marker vectors (lines)

// ------------------------------------
//
// 11/17/2018 - different scene concept
// every segment is stored individually
//
// ------------------------------------

  for (iLine=0;iLine<sz;iLine++)  // das sollte nun segment heissen, es gibt ja keine Linien mehr in dem Sinne
  {                               // die sind jetzt Teil der Textur
    unsigned int vCount = 0;
    const std::vector<S_MarkerPoint> &rc_Marker = rc_Param.m_c_Markers[iLine];
    for (iMarker=0;iMarker<rc_Marker.size()-1;iMarker++) // no. of markersteps (typically > 500)
    {
      if (rc_Marker[iMarker].b_Visible) vCount+=6; // 2 triangles per Quad
    }

    GLfloat* vertices  = new GLfloat[vCount*3]; // Vertex.x/y/z
    GLfloat* colors    = new GLfloat[vCount*3]; // Color.r/g/b
    GLfloat* texCoords = new GLfloat[vCount*2]; // Texture.U/V

    int iV = 0;
    int iTx = 0;

    //      [0 ..  255] <-- 2do
    //      [0 .. 4095]
    // -> [0.0 ..  1.0]
    float f_R = rc_Param.m_c_Colors[iLine].u_Red   / 4095.0f;
    float f_G = rc_Param.m_c_Colors[iLine].u_Green / 4095.0f;
    float f_B = rc_Param.m_c_Colors[iLine].u_Blue  / 4095.0f;

//    int textureID = rc_Param.m_TextureIDs[*p_idxVBO]; // nur die "Road" hat eine texture, Left+Right nicht

    float fTexStrip = 0.0f; // store strip of texture, if not full texture is mapped to triangle
    float fTexIncr = 0.1f;  // 2do <-- aus der "Höhe" des triangles berechnen!

    for (iMarker=0;iMarker<rc_Marker.size()-1;iMarker++) // no. of markersteps (typically > 500)
    {
      if (rc_Marker[iMarker].b_Visible)
      {
        /*  p3 +--+ p2     two triangles from 1 quad
               | /|
               |/ |
            p0 +--+ p1
            2/4/2019 for culling in CW order: p2->p0->p3
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
        vertices[iV] = p2.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p2.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 1.0f;
        vertices[iV] = p2.rl_Z;   colors[iV++] = f_B;
        vertices[iV] = p1.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 1.0f;
        vertices[iV] = p1.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p1.rl_Z;   colors[iV++] = f_B;
        /*
          U ^                Texture coordinates
            |
            |
            +-----> V
        */
        texCoords[iTx++] = fTexStrip + 0.0f;     // p0 - U
        texCoords[iTx++] = 0.0f;                 // p0 - V
        texCoords[iTx++] = fTexStrip + fTexIncr; // p2 - U
        texCoords[iTx++] = 1.0f;                 // p2 - V
        texCoords[iTx++] = fTexStrip + 0.0f;     // p1 - U
        texCoords[iTx++] = 1.0f;                 // p1 - V

        // 2nd Triangle
        vertices[iV] = p2.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p2.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p2.rl_Z;   colors[iV++] = f_B;
        vertices[iV] = p0.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p0.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 1.0f;
        vertices[iV] = p0.rl_Z;   colors[iV++] = f_B;
        vertices[iV] = p3.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 1.0f;
        vertices[iV] = p3.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
        vertices[iV] = p3.rl_Z;   colors[iV++] = f_B;

        texCoords[iTx++] = fTexStrip + fTexIncr; // p2 - U
        texCoords[iTx++] = 1.0f;                 // p2 - V
        texCoords[iTx++] = fTexStrip + 0.0f;     // p0 - U
        texCoords[iTx++] = 0.0f;                 // p0 - V
        texCoords[iTx++] = fTexStrip + fTexIncr; // p3 - U
        texCoords[iTx++] = 0.0f;                 // p3 - V
        fTexStrip += fTexIncr;
      }
    }

    // b) store VAO props for OpenGL-drawing loop (and later manipulation, e.g. position change)
    c_VAO vao;
    vao.Name = "seg";// aParts[iLine];
    vao.t_Shade = SHADER_TEXTURE;
    std::string sTex = rc_Param.m_Textures[iLine];
    // 2do: texWater --> tx_Water
    if (sTex.compare("texWater;") == 0) vao.ui_idTexture = tex_map.find("tx_Water")->second;
    if (sTex.compare("texRoad;") == 0) vao.ui_idTexture = tex_map.find("tx_Road")->second;

    vao.uiVertexCount = vCount*3;
    vVAOs.push_back(vao);

    // VBO, s. http://ogldev.atspace.co.uk/www/tutorial02/tutorial02.html
    glGenBuffers(1, &positionBuffer[ui_idVBO]);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount*3, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &colorBuffer[ui_idVBO]);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[ui_idVBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount*3, colors, GL_STATIC_DRAW);

    glGenBuffers(1, &uvBuffer[ui_idVBO]);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[ui_idVBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount*2, texCoords, GL_STATIC_DRAW);

    delete [] colors;
    delete [] vertices;
    delete [] texCoords;

    // each part of the scene (pavement, left, right marker, ...) has 1 VBO
    ui_idVBO++;
  }

  return 0;
}

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

  if (b_culling)
  {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT); // 2do: die Strassen-texturen andersherum (ccw oder cw) an die GPU übergeben
//  glFrontFace(GL_CCW);
  }
  else
  {
    glDisable(GL_CULL_FACE);
  }

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
    if (ui == 1) // für Obj 1 und größer hier 1-malig setzen
      glUseProgram(program); // 2/2/19 für jedes Objekt glUseProgram aufrufen?
    err = glGetError();

// MOVED ???? ... wenn ich das ausschalte, werden billboards ok, aber barriers (+ Jeep) falsch gezeichnet
    bool bMoved = false; // workaround, removed later

    if (
      ((vVAOs[ui].pos.x < -0.001f) || (vVAOs[ui].pos.x > 0.001f)) ||
      ((vVAOs[ui].pos.y < -0.001f) || (vVAOs[ui].pos.y > 0.001f)) ||
      ((vVAOs[ui].pos.z < -0.001f) || (vVAOs[ui].pos.z > 0.001f))
      )
    {
      bMoved = true; // workaround, removed later
      glm::mat4 Model = glm::mat4(1.0f);
      // first translate
      Model = glm::translate(Model, vVAOs[ui].pos);
      p_cam->change_Model(Model);
    }
// MOVED ???? ... wenn ich das ausschalte, werden billboards ok, aber barriers (+ Jeep) falsch gezeichnet

    if (p_cam->iStickToObj > 0)
    {
      // ----------------
      // move ego vehicle
      // ----------------
      if (((p_cam->iStickToObj==1) && (vVAOs[ui].Name.compare("Jeep") == 0)) // Conticar, Jeep_Openair
        ||
        ((p_cam->iStickToObj == 2) && (vVAOs[ui].Name.compare("Jeep_default") == 0)))
      {
        glm::vec3 move; // hack, just a test for object movement
        move.x = p_cam->Pos.x - vVAOs[ui].pos.x;
        move.y = p_cam->Pos.y - vVAOs[ui].pos.y;
        glm::vec3 dir = p_cam->At - p_cam->Pos;
        move.x += dir.x*3.0f;
        move.y += dir.y*3.0f;
        glm::mat4 Model = glm::mat4(1.0f);
        glm::float32 f_VehRot_Z = -atan2(dir.x, dir.y);
        glm::float32 f_VehRot_Z_DEG = glm::degrees(f_VehRot_Z);
        // "steering the car" works only with first ranslation, then rotation
        // yet, there is a flickering problem!!
        Model = glm::translate(Model, glm::vec3(p_cam->Pos.x + dir.x*f_camy, p_cam->Pos.y + dir.y*f_camy, 0));
        Model = glm::rotate(Model, f_VehRot_Z_DEG, glm::vec3(0.0f, 0.0f, 1.0f)); // where x, y, z is axis of rotation (e.g. 0 1 0)
        p_cam->change_Model(Model);
      }
      else
      {
        glUniform3f(sh1_unif_offset, 0.0f, 0.0f, 0.0f);
        //      p_cam->reset_Model();
      }
    }

    glUniform1i(sh1_unif_wirecolor, 0); // sh1_unif_wirecolor 0: nothing 1: set col f. overlayed wireframe (needed for colored, not textures objects)

//    if (vVAOs[ui].b_Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
//      glBindTexture(GL_TEXTURE_2D, vGLTexture[vVAOs[ui].ui_idTexture-1]); // TEXTURE_ID shall be > 0 !     (-1!!)
      glBindTexture(GL_TEXTURE_2D, vVAOs[ui].ui_idTexture); // TEXTURE_ID shall be > 0 !     (-1!!)
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


    //  wenn's hier crasht, dann ist der Fehler vermutlich vorher beim buffern passiert und
    //  glGetError hätte etwas melden sollen!!

    if (b_solid)
    {
      glDrawArrays(GL_TRIANGLES, 0, vVAOs[ui].uiVertexCount); // <-- if error is thrown here,
      err = glGetError();                                     //     it can be either positionbuffer, colorbuffer or uvbuffer
                                                              //     if t_Shade == TEXTURE,
                                                              //     then colorbuffer is NULL and vice versa!

      // if solid: draw wireframe in black
      glUniform1i(sh1_unif_wirecolor, 1); // sh1_unif_wirecolor 0: nothing 1: set col f. overlayed wireframe (needed for colored, not textures objects)
    }
    else
      glUniform1i(sh1_unif_wirecolor, 0); // sh1_unif_wirecolor 0: nothing 1: set col f. overlayed wireframe (needed for colored, not textures objects)

    if (b_wireframe)
    {
      glUniform1i(sh1_unif_col_tex, 0); // shader into color-branch
      glLineWidth(5.0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glDrawArrays(GL_TRIANGLES, 0, vVAOs[ui].uiVertexCount); // <-- if error is thrown here,
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

/*    if (vVAOs[ui].t_Shade == SHADER_TEXTURE)
    {
      //      glUniform1i(sh1_unif_col_tex, 1); // shader into texture-branch
      //      glActiveTexture(GL_TEXTURE0);
      //      err = glGetError();
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    */

    glBindVertexArray(0); // 2019-04-13 unbind -> jetzt wird das letzte Objekt nicht mehr vom Cursor (s.u.) "überschrieben"
                          //                      aber die Textur flackert

//    if (vVAOs[ui].b_Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

// MOVED ???? ... wenn ich das ausschalte, werden billboards ok, aber barriers (+ Jeep) falsch gezeichnet
    if (vVAOs[ui].b_moving || bMoved)
    {
      p_cam->reset_Model();
    }
// MOVED ???? ... wenn ich das ausschalte, werden billboards ok, aber barriers (+ Jeep) falsch gezeichnet

  } // for ...

  err = glGetError();


  // set back coloring
  glUniform1i(sh1_unif_col_tex, 0); // shader into color-branch
  glUniform1i(sh1_unif_wirecolor, 0); // sh1_unif_wirecolor 0: nothing 1: set col f. overlayed wireframe (needed for colored, not textures objects)



/*  //  ---------------------------------------------------------------------------------------
  //  glVertexAttribPointer is the current and preferred way of passing attributes to the GPU.
  //  glVertexPointer is part of the old and deprecated fixed function pipeline and set openGL to use the VBO for the attribute.
  //  ---------------------------------------------------------------------------------------
  glPointSize(48.0);
  GLfloat coordsies[8] = { 0.0f, 1.0f, 0.0f , Cursor.x,Cursor.y,Cursor.z, 0.0f, 0.0f };

  glGenVertexArrays(1, &vao2);
  glBindVertexArray(vao2);
  glGenBuffers(1, &positionBuf2); // <-- Achtung !! nimmt die aktuelle Anzahl VBO als index !!
  glGenBuffers(1, &colorBuf2); // <-- Achtung !! nimmt die aktuelle Anzahl VBO als index !!
  glBindBuffer(GL_ARRAY_BUFFER, positionBuf2);
  glBindBuffer(GL_ARRAY_BUFFER, colorBuf2);

  glEnableVertexAttribArray(sh1_attr_col); // Attribute indexes were received from calls to glGetAttribLocation, or passed into glBindAttribLocation.
  glEnableVertexAttribArray(sh1_attr_pos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3, coordsies, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(sh1_attr_col, 3, GL_FLOAT, false, 0, 0); // col_data is a float*, 3 per vertex
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3, coordsies+3, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(sh1_attr_pos, 3, GL_FLOAT, false, 0, 0); // vertex_data is a float*, 3 per vertex, representing the position of each vertex
// ---------------------------------------------------------------------------------
// https://www.gamedev.net/forums/topic/659810-gldrawelements-isnt-drawing-anything/
// You cant just replace it with DrawArrays, they work differently.
//  DrawArrays needs a vertex buffer(or more), DrawElements also needs an index buffer.
// ---------------------------------------------------------------------------------
  glDrawArrays(GL_POINTS, 0, 1); // vertexcount = 1
  glBindVertexArray(0);
  glDisableVertexAttribArray(sh1_attr_pos);
  glDisableVertexAttribArray(sh1_attr_col);

  err = glGetError();
  */


  // --------------------------------------------
  // draw sceneblock in purple, that player is on
  // --------------------------------------------
//  glPointSize(1.0);
  GLfloat col[3*4] = { 1.0f, .5f, 1.f,   1.0f, .5f, 1.f,   1.0f, .5f, 1.f,   1.0f, .5f, 1.f };
  GLfloat * pos = (GLfloat*)Scenepos;

  glGenVertexArrays(1, &vao3);
  glBindVertexArray(vao3);

  glEnableVertexAttribArray(sh1_attr_col); // Attribute indexes were received from calls to glGetAttribLocation, or passed into glBindAttribLocation.
  glEnableVertexAttribArray(sh1_attr_pos);

  glGenBuffers(1, &colorBuf3); // <-- Achtung !! nimmt die aktuelle Anzahl VBO als index !!
  glBindBuffer(GL_ARRAY_BUFFER, colorBuf3);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3*4, col, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(sh1_attr_col, 3, GL_FLOAT, false, 0, 0); // col_data is a float*, 3 per vertex

  glGenBuffers(1, &positionBuf3); // <-- Achtung !! nimmt die aktuelle Anzahl VBO als index !!
  glBindBuffer(GL_ARRAY_BUFFER, positionBuf3);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3*4, pos, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(sh1_attr_pos, 3, GL_FLOAT, false, 0, 0); // vertex_data is a float*, 3 per vertex, representing the position of each vertex

  glDrawArrays(GL_QUADS, 0, 4); // 4 = vertexcount
  glBindVertexArray(0);
  glDisableVertexAttribArray(sh1_attr_pos);
  glDisableVertexAttribArray(sh1_attr_col);

  err = glGetError();



  //    if (b_PNG) FBO_to_PPM();
}
