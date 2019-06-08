#include "stdafx.h"
#pragma once

#include <stdio.h> // <-- sprintf
#include "inc_render.h"
//#define GLM_FORCE_RADIANS // disabled ... distorts the projection
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class ShaderMan // Shader manager
{
public:
  // =============
  // GLSL / Shader
  // =============

  // ** Shader1 **
  GLuint program;           // = sh1
  // std-attrib
  GLuint sh1_attr_pos;
  GLuint sh1_attr_col;
  GLuint sh1_attr_tex;      // uv
  // uniform
  GLuint sh1_unif_offset;   // <-- for object-movement
  GLuint sh1_unif_wirecolor; // sh1_unif_wirecolor 0: nothing 1: set col f. overlayed wireframe (needed for colored, not textures objects)
  GLuint sh1_unif_MVPMatrix;
  GLuint sh1_unif_sampler;  // sampler2D
  GLuint sh1_unif_col_tex;  // 0=col,1=tex



  // ** Shader2 **
  GLuint program_fps; // = sh2
  // attrib + uniform
  GLuint sh2_attr_pos;
  GLuint sh2_attr_tex;
  GLuint sh2_unif_ID;



  // s. http://open.gl/drawing
  void InitShader1()
  {
    GLenum err = glGetError();

    char buffer[512];

#define COLS_N_TEXTURES
#ifdef COLS_N_TEXTURES
    const GLchar * vertexShaderSource[] = {
      "#version 330 core\n"
// in
      "in vec3 position;\n"
      "in vec2 vertexUV;\n"     //  i) textured
      "in vec3 color;\n"        // ii) colored f.d. nicht texturierten, sondern Farbdreiecke
// modifiers
      "uniform mat4 MVPMatrix;\n"
      "uniform vec3 offset;\n"  // object-movement (redundant to MVP?)
      "uniform int wirecolor;\n"
// out
      "out vec2 UV;\n"          //  i) textured
      "out vec3 fragColor;\n"   // ii) colored triangles, ...
// code
      "void main()\n"
      "{\n"
      "  if (wirecolor==1)\n"
      "    fragColor = vec3(0.0,0.0,0.0);\n"
      "  else\n"
      "    fragColor = color;\n"// f.d. nicht texturierten, sondern Farbdreiecke
      "  vec4 totaloffset = vec4(offset.x, offset.y, offset.z, 0.0);\n"
      "  gl_Position = MVPMatrix * (vec4(position, 1.0) + totaloffset);\n" // totaloffset <-- fragementshader for movement! 
      "  UV = vertexUV;\n"
      "}"
    };
    // der Vertexshader gibt die fragColor an den Fragment-Shader weiter!!
    const GLchar * fragmentShaderSource[] = {
      "#version 330 core\n"
// in
      "in vec2 UV;\n"
      "in vec3 fragColor;\n"
// modifiers
      "uniform sampler2D myTexSampler;\n" // <-- hier hatte ich das Semikolon vergessen
      "uniform int col_tex;\n"            // 0 = color, 1 = texture
// out
      "out vec4 outColor;\n"              // <-- texture
      "void main()\n"
      "{\n"
      "  if (col_tex==0)\n"
      "    outColor = vec4(vec3(fragColor),1.0);\n"
      "  else\n"
      "  {"
      "    outColor = texture(myTexSampler, UV);\n" // texture2D ist deprecated
//s. https://learnopengl.com/Advanced-OpenGL/Blending
      "    if (outColor.a < 0.1)" // transparent
      "      discard;"
      "  }"
      "}"
    };
#endif
    // s. auch: http://wiki.lwjgl.org/index.php?title=GLSL_Tutorial:_Communicating_with_Shaders

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, vertexShaderSource, 0);
    glCompileShader(vertexShader);
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer); // <-- debug, kann man sich schoen im debugger ansehen!!
    err = glGetError();
    // 2do: check that buffer = "No errors."
// comment out on SONY    assert(strlen(buffer)==0);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource    (fragmentShader, 1, fragmentShaderSource, 0); // set array of strings as source code
    glCompileShader   (fragmentShader); // compile
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer); // <-- debug
    err = glGetError();
    // 2do: check that buffer = "No errors."
// comment out on SONY   assert(strlen(buffer)==0);

    program = glCreateProgram(); // create empty program object
    glAttachShader(program, vertexShader); // attach shader
    err = glGetError();
    glAttachShader(program, fragmentShader); // attach shader
    glLinkProgram (program); // link
    glUseProgram  (program); // install ... and use in the further runtime ...
                           // start here, otherwise --> http://stackoverflow.com/questions/8688171/android-opengl20-error-on-command-gluniformmatrix4fv 
    err = glGetError();

    // attribs (fix)
    sh1_attr_pos = glGetAttribLocation(program, "position");
    sh1_attr_col = glGetAttribLocation(program, "color");
    sh1_attr_tex = glGetAttribLocation(program, "vertexUV");
    err = glGetError();

    // uniforms, 2do: die Variablen sollen nicht ...Attrib, sondern ...Uniform heissen!!
    sh1_unif_offset    = glGetUniformLocation(program, "offset");       // object movement
    // sh1_unif_wirecolor 0: nothing 1: set col f. overlayed wireframe (needed for colored, not textures objects)
    sh1_unif_wirecolor = glGetUniformLocation(program, "wirecolor");
    sh1_unif_MVPMatrix = glGetUniformLocation(program, "MVPMatrix");    // camera movement
    sh1_unif_sampler   = glGetUniformLocation(program, "myTexSampler");
    sh1_unif_col_tex   = glGetUniformLocation(program, "col_tex");
    err = glGetError();
  }

  void InitShader2()
  {
    GLenum err = glGetError();

    char buffer[512];

/*
  4 coord.systems: Object, World, View, Clip
  ==========================================
  Object --> Model Matrix --> World
  World  --> View matrix --> View
  View   --> Projection Matrix (w. implicit homogeneous divide) --> Clip
*/
    /* vertex shader : output always to "homogeneous clip space", i.e. (-1:1, -1:1, -1:1, -1:1) */
    const GLchar * vshd_src_FPS[] = {
      "#version 330 core\n" // 410 not supported by SONY OpenGL driver
      "in vec2 vp_clipspace;\n" // vertexposition? in clipspace
      "out vec2 UV;\n"
      "void main()\n"
      "{\n"
      "  UV = (vp_clipspace+1.0) * 0.5;\n"
//      "  UV = (vp_clipspace) * 0.5;\n" // 2mal (oder 4mal) dargestellt
//      "  UV = (vp_clipspace+1.0);\n" // 4mal in viertelgröße dargestellt
//      "  UV = (vp_clipspace);\n" // 4x dargestellt
// s. https://stackoverflow.com/questions/31125387/why-is-gl-position-a-different-data-type-than-position
      "  gl_Position = vec4(vp_clipspace.x, vp_clipspace.y, 0.5, 1.0);\n" // global variable (x,y,z,w), [z] e.g. 0.5, but not 1.0
//      "  gl_Position.xy *= 0.5;\n" // scale to half of screen
      "  gl_Position.x *= 0.8;\n" // scale to half of screen
      "  gl_Position.y *= 0.4;\n" // scale to half of screen
      "  gl_Position.y += 0.3;\n" // up
      "}"
    };
    const GLchar * fshd_src_FPS[] = {
      "#version 330 core\n"
      "in vec2 UV;\n"
      "out vec4 color;\n"
      "uniform sampler2D myTexSampler;\n"
      "void main()\n"
      "{\n"
      "  color = texture(myTexSampler, UV).rgba;\n" // texture2D ist deprecated
                                                   //s. https://learnopengl.com/Advanced-OpenGL/Blending
      "    if (color.a < 0.1)" // transparent
      "      discard;"
      "}"
    };


    // Shader for FPS
    GLuint vshaderFPS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource    (vshaderFPS, 1, vshd_src_FPS, 0);
    glCompileShader   (vshaderFPS);
    glGetShaderInfoLog(vshaderFPS, 512, NULL, buffer); // <-- debug, kann man sich schoen im debugger ansehen!!
    err = glGetError();
    // 2do: check that buffer = "No errors."
    GLuint fshaderFPS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource    (fshaderFPS, 1, fshd_src_FPS, 0); // set array of strings as source code
    glCompileShader   (fshaderFPS); // compile
    glGetShaderInfoLog(fshaderFPS, 512, NULL, buffer); // <-- debug
    err = glGetError();
    // 2do: check that buffer = "No errors."
    program_fps = glCreateProgram(); // create empty program object
    glAttachShader(program_fps, vshaderFPS); // attach shader
    glAttachShader(program_fps, fshaderFPS); // attach shader
    glLinkProgram (program_fps); // link
    err = glGetError();
    glUseProgram  (program_fps);
    err = glGetError();

    // attribs
    sh2_attr_pos = glGetAttribLocation(program_fps, "vp_clipspace");
    err = glGetError();
    sh2_attr_tex = glGetAttribLocation(program_fps, "vertexUV");     // UV geht auch
    err = glGetError();
    sh2_unif_ID = glGetUniformLocation(program_fps, "myTexSampler");
    err = glGetError();
  }

};
