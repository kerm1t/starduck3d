#include "stdafx.h"
#pragma once

#include <stdio.h> // <-- sprintf
#include "inc_render.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

class ShaderMan // Shader manager
{
public:
  // =============
  // GLSL / Shader
  // =============
  GLuint program_fps;
  GLuint program;

  // std-attrib
  GLuint posAttribFPS;
  GLuint texAttribFPS;
  GLuint posAttrib;
  GLuint colAttrib;
  GLuint texAttrib; // uv

  // uniform
  GLuint SamplerAttribFPS; // Shader #2 !!!
  GLuint offsetAttrib; // <-- for object-movement
  GLuint MVPMatrixAttrib;
  GLuint SamplerAttrib; // sampler2D
  GLuint col_texAttrib; // 0=col,1=tex

  // s. http://open.gl/drawing
  void InitShaders()
  {
    GLenum err = glGetError();

    char buffer[512];
    // "GLSL" starts -->

    /* vertex shader : output always to "homogeneous clip space", i.e. (-1:1, -1:1, -1:1, -1:1) */
    const GLchar * vshd_src_FPS[] = {
      "#version 330 core\n" // 410 not supported by SONY OpenGL driver
      "in vec2 vp_clipspace;\n"
      "out vec2 UV;\n"
      "void main()\n"
      "{\n"
      //        "    UV = (vp+1.0) * 0.5;\n"
      "    gl_Position = vec4(vp_clipspace.x, vp_clipspace.y, 0.0, 1.0);\n"
      //        "    gl_Position.xy *= 0.5;\n"
      //        "    vec2 vp_hom = vp_clipspace - vec2(400,300);\n"
      //        "    vp_hom /= vec2(400,300);\n"
      //        "    gl_Position = vec4(vp_hom, 0.0, 1.0);\n"
      "    UV = vp_clipspace;\n"
      "}"
    };
    const GLchar * fshd_src_FPS[] = {
      "#version 330 core\n"
      "in vec2 UV;\n"
      "out vec4 color ;\n"
      "void main()\n"
      "{\n"
      "    color = texture(myTexSampler, UV).rgb;\n" // texture2D ist deprecated
      "}"
    };

#define COLS_N_TEXTURES
#ifdef COLS_N_TEXTURES
    const GLchar * vertexShaderSource[] = {
      "#version 330 core\n"
      "uniform mat4 MVPMatrix;\n"
      "in vec3 position;\n"
      "uniform vec3 offset;\n"  // <-- for object-movement = uniform!!
      "in vec2 vertexUV;\n"
      "out vec2 UV;\n"
      "in vec3 color;\n"
      "out vec3 fragColor;\n"
      "void main()\n"
      "{\n"
      "    fragColor = color;\n"
      //        "    gl_Position = MVPMatrix * vec4(position, 1.0);\n"
      "    vec4 totaloffset = vec4(offset.x, offset.y, offset.z, 0.0);\n"
      "    gl_Position = MVPMatrix * (vec4(position, 1.0) + totaloffset);\n" // totaloffset <-- fragementshader for movement! 
      "    UV = vertexUV;\n"
      "}"
    };
    // der Vertexshader gibt die fragColor an den Fragment-Shader weiter!!
    const GLchar * fragmentShaderSource[] = {
      "#version 330 core\n"
      "in vec2 UV;\n"
      "uniform sampler2D myTexSampler;\n" // <-- hier hatte ich das Semikolon vergessen
      "uniform int col_tex;\n" // 0 = color, 1 = texture
      "in vec3 fragColor;\n"
      "out vec3 outColor;\n"              // <-- texture
      //        "out vec4 outColor;\n"              // <-- color
      "void main()\n"
      "{\n"
      "    if (col_tex==0)\n"
      "        outColor = vec3(fragColor);\n"
      "    else\n"
      "        outColor = texture(myTexSampler, UV).rgb;\n" // texture2D ist deprecated
      "}"
    };
#endif
    // s. auch: http://wiki.lwjgl.org/index.php?title=GLSL_Tutorial:_Communicating_with_Shaders

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, vertexShaderSource, 0);
    glCompileShader(vertexShader);
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer); // <-- debug, kann man sich schoen im debugger ansehen!!
    err = glGetError(); 
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, 0); // set array of strings as source code
    glCompileShader(fragmentShader); // compile
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer); // <-- debug
    err = glGetError();

    program = glCreateProgram(); // create empty program object
    glAttachShader(program, vertexShader); // attach shader
    glAttachShader(program, fragmentShader); // attach shader
    glLinkProgram(program); // link
    //    glUseProgram(program); // install ... and use in the further runtime ...
    glUseProgram(program); // start here, otherwise --> http://stackoverflow.com/questions/8688171/android-opengl20-error-on-command-gluniformmatrix4fv 
    err = glGetError();

    // Shader for FPS
    GLuint vshaderFPS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshaderFPS, 1, vshd_src_FPS, 0);
    glCompileShader(vshaderFPS);
    glGetShaderInfoLog(vshaderFPS, 512, NULL, buffer); // <-- debug, kann man sich schoen im debugger ansehen!!
    GLuint fshaderFPS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshaderFPS, 1, fshd_src_FPS, 0); // set array of strings as source code
    glCompileShader(fshaderFPS); // compile
    glGetShaderInfoLog(fshaderFPS, 512, NULL, buffer); // <-- debug
    program_fps = glCreateProgram(); // create empty program object
    glAttachShader(program_fps, vshaderFPS); // attach shader
    glAttachShader(program_fps, fshaderFPS); // attach shader
    glLinkProgram(program_fps); // link
    err = glGetError();

    memset(buffer,0,512);
    sprintf(buffer,"%s",gluErrorString(glGetError()));

    // attribs (fix)
//    posAttribFPS    = glGetAttribLocation(program_fps, "vp_clipspace"); // vertex position
//    texAttribFPS    = glGetAttribLocation(program_fps, "vertexUV");
//    SamplerAttribFPS= glGetUniformLocation(program, "myTexSampler");
    err = glGetError();

    memset(buffer,0,512);
    sprintf(buffer,"%s",gluErrorString(glGetError()));

    // attribs (fix)
    posAttrib       = glGetAttribLocation(program, "position");
    colAttrib       = glGetAttribLocation(program, "color");
    texAttrib       = glGetAttribLocation(program, "vertexUV");
    err = glGetError();
    // uniforms
    offsetAttrib    = glGetUniformLocation(program, "offset");       // object movement
    MVPMatrixAttrib = glGetUniformLocation(program, "MVPMatrix");    // camera movement
    SamplerAttrib   = glGetUniformLocation(program, "myTexSampler");
    col_texAttrib   = glGetUniformLocation(program, "col_tex");
    err = glGetError();

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
      glm::vec3(0,0,1.33), // Camera is at (4,3,-3), in World Space
      glm::vec3(1,0,1.33), // and looks at the origin
      glm::vec3(0,0,1)     // Head is up (set to 0,-1,0 to look upside-down)
      );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model      = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVPMatrix  = Projection * View * Model; // Remember, matrix multiplication is the other way around
    glUniformMatrix4fv(MVPMatrixAttrib, 1, GL_FALSE, &MVPMatrix[0][0]); // load matrix into shader
    err = glGetError();

    //    glActiveTexture(GL_TEXTURE0);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    //    glBindTexture(GL_TEXTURE_2D, Texture[1]);
    //    glUniform1i(SamplerAttrib, 0);
  }
};
