#pragma once

#include <iostream> // <-- fbo to pgm|ppm
#include <fstream> // <-- fbo to pgm|ppm
#include <string> // <-- fbo to pgm|ppm

#if 0
// ---
// FBO
// ---
void CHECK_FRAMEBUFFER_STATUS()
{                                                         
  GLenum status;
  status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER); 
  switch(status)
  {
  case GL_FRAMEBUFFER_COMPLETE:
    break;

  case GL_FRAMEBUFFER_UNSUPPORTED: // choose different formats
    break;

  default: // programming error; will fail on all hardware
    //        fputs("Framebuffer Error\n", stderr);
    exit(-1);
  }
}

void proj::Render::init_FBO()
{
  glGenFramebuffers(1, &fb);
  glGenTextures(1, &color);
  glGenRenderbuffers(1, &depth);

  glBindFramebuffer(GL_FRAMEBUFFER, fb);

  glBindTexture(GL_TEXTURE_2D, color);
  glTexImage2D(GL_TEXTURE_2D, 
    0, 
    GL_RGBA, 
    fbo_width, fbo_height,
    0, 
    GL_RGBA,
    GL_UNSIGNED_BYTE, 
    NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbo_width, fbo_height);
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

  CHECK_FRAMEBUFFER_STATUS();
}

//GLubyte pixels[fbo_height][fbo_width][3]; // too big for the stack
GLubyte puxels[1000][600][3]; // too big for the stack
void proj::Render::FBO_to_PPM() // write out as binary PPM (with lines in reverse order)
{
  glBindTexture(GL_TEXTURE_2D, color);//textureId[0]);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, puxels);
  glBindTexture(GL_TEXTURE_2D, 0); // <-?

  std::ofstream myfile;
  std::string filename;
  filename = "d:\\glShoot_FBO.ppm";
  myfile.open(filename.c_str());
  myfile << "P6" << std::endl;
  myfile << std::to_string((_ULonglong)fbo_width) << " " << std::to_string((_ULonglong)fbo_height) << std::endl; // "w h"
  //    myfile << "1000" << " " << "600" << std::endl; // "w h"
  myfile << "255" << std::endl;
  myfile.close();
  myfile.open(filename.c_str(),std::ofstream::app | std::ofstream::binary); // binary-"trick" from: http://www.gamedev.net/topic/510801-render-to-memory/

  for (int i = 0; i < fbo_height; i++)
  {
    for (int j = 0; j < fbo_width; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        myfile << puxels[(fbo_height-1)-i][j][k];
      }
    }
  }
  myfile.close();
}
#endif
