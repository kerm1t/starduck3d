// -----------------------
//
// Camera
//  -standard
//  -Oculus Rift
// -----------------------

#include "stdafx.h"
#pragma once

#include <math.h>
#include "glm.hpp" // is there an _ext ??
#include <gtc/matrix_transform.hpp>
#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library

//#define DEGTORAD ()

class Camera
{
private:
  void uploadMVP()
  {
    glm::mat4 MVPMatrix  = Projection * View * Model; // Remember, matrix multiplication is the other way around
    // ----------------------------------------------------------------------
    // Hier gibt's einen Fehler beim Umschalten auf die AMD Radeon GraKa --->
    glUniformMatrix4fv(*p_MVPMatrixAttrib, 1, GL_FALSE, &MVPMatrix[0][0]); // load matrix into shader
    // <--- Hier gibt's einen Fehler beim Umschalten auf die AMD Radeon GraKa
    // ----------------------------------------------------------------------
  }

public:
  bool bOculus; // 0: Norm z=1, 1: Norm ...

  // ====----
  int width;            // <-- window size change
  int height;           // <-- window size change

  glm::mat4 Projection; // change @ init
  glm::mat4 projectionSave;
  glm::mat4 View;       // camera move
  glm::mat4 Model;      // e.g. object rotate / translate	

  GLuint * p_MVPMatrixAttrib;
  // ====----

  // Projection (intrinsics)
  glm::float32 fovy_DEG, fovy_RAD; // e.g. 60.0f
  glm::float32 aspect;	// width/height
  glm::float32 zNear;		// e.g. 0.1f
  glm::float32 zFar;		// e.g. 100.0f

  // View (extrinsics)
  glm::vec3 Pos;        // eye - in glm-Sprech
  glm::vec3 At;         // center (viewing "target")
  glm::vec3 Norm;       // up
  glm::vec3 Dir;        // viewing direction
  glm::vec3 DirMouse;   // 

  glm::float32 mouselook_DEG, mouselook_RAD; // e.g. 90.0f

  Camera()
  {
    fovy_DEG =  60.0f;
    fovy_RAD = fovy_DEG * 3.14159f/180.0f; // 2do, DEGTORAD, math.pi
    aspect   =   4.0f / 3.0f;
    zNear    =   1.0f;
    zFar     = 100.0f;

    mouselook_DEG = 180.0f;
    mouselook_RAD = mouselook_DEG * 3.14159f/180.0f;

    Dir[0] =  sin(0.0f);
    Dir[1] =  1.0f;
    Dir[2] = -sin(0.0f);

    Norm = glm::vec3(0.0f,0.0f,1.0f); // z = up
  }

  void change_Aspect(int _width, int _height)
  {
    width  = _width;
    height = _height;
    aspect = glm::float32(_width)/glm::float32(_height);
    //		updatePos(); // <-- 2do: hier nur die projeciton updaten und dann die MVP neu erzeugen und zur Graka hochladen
  }

  void init_MVP()
  {
    // Projection matrix e.g. 60° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(fovy_DEG, aspect, zNear, zFar);

    // ... update Pos, At
    // Pos.z ~~ += camZ
    // At += m_x / m_y   <-- Mouse

    View = glm::lookAt(
      glm::vec3( Pos[0], Pos[1], Pos[2]), // Camera is at (x,y,z), in World Space
      glm::vec3(  At[0],  At[1],  At[2]), // and looks at ...
      glm::vec3(Norm[0],Norm[1],Norm[2])  // Head is up <-- change on Oculus!
      );

    // Model matrix : an identity matrix (model will be at the origin)
    Model = glm::mat4(1.0f);

    uploadMVP();
  }

  //	void updateView(glm::vec3 _Pos, glm::vec3 _At, glm::vec3 _Norm)
  void update_View()
  {
    View = glm::lookAt(
      glm::vec3( Pos[0], Pos[1], Pos[2]), // Camera is at (x,y,z), in World Space
      glm::vec3(  At[0],  At[1],  At[2]), // and looks at ...
      glm::vec3(Norm[0],Norm[1],Norm[2])  // Head is up <-- change on Oculus!
      );
    uploadMVP();
  }

  /*
  void Projection(fovy, aspect, zNear, zFar);
  void View(Pos,At,Norm);
  void Model(matrix);
  */
  void change_Model(glm::mat4 _Model)
  {
    Model = _Model;
    uploadMVP();
  }
  void reset_Model()
  {
    Model = glm::mat4(1.0f);
    uploadMVP();
  }

  void FollowObj()
  {
  }

  void MoveFwd()
  {
#define VELO_FIRST  25.0f;
#define VELO_SECOND 12.0f;
    // in "Dir" Richtung weiterbewegen
    Pos += DirMouse * 1.0f /VELO_FIRST; // /50 war zu langsam
  }
  void MoveLeft()
  {
    glm::vec3 ortho = glm::vec3(0,0,1);
    glm::vec3 dirortho = glm::cross(DirMouse,ortho);
    Pos -= dirortho;
  }
  void MoveBack()
  {
#define VELO_BACK   50.0f;
    Pos -= DirMouse * 1.0f /VELO_BACK;
  }
  void MoveRight()
  {
    glm::vec3 ortho = glm::vec3(0,0,1);
    glm::vec3 dirortho = glm::cross(DirMouse,ortho);
    Pos += dirortho;
  }
  void TurnLeft() // keep position
  {
    // 90 deg. left
//    glm::vec3 ortho = glm::vec3(0,0,1);
//    Dir = glm::cross(Dir,ortho);
    // 
    float thetaRAD = -5.0f * 3.14159f/180.0f  /20.0f; // /50 war zu langsam
    Dir[0] = Dir[0] * cos(thetaRAD) - Dir[1] * sin(thetaRAD);
    Dir[1] = Dir[0] * sin(thetaRAD) + Dir[1] * cos(thetaRAD);
  }
  void TurnRight() // keep position
  {
//    glm::vec3 ortho = glm::vec3(0,0,-1);
//    Dir = glm::cross(Dir,ortho);
    float thetaRAD = 5.0f * 3.14159f/180.0f  /20.0f; // /50 war zu langsam
    Dir[0] = Dir[0] * cos(thetaRAD) - Dir[1] * sin(thetaRAD);
    Dir[1] = Dir[0] * sin(thetaRAD) + Dir[1] * cos(thetaRAD);
  }

  void Look_with_Mouse(glm::vec2 mouse)
  {
    float w_half = width  / 2.0f;
    float h_half = height / 2.0f;
    float m_x = (mouse.x-w_half) / w_half;
    float m_z = (mouse.y-h_half) / h_half;
/*
    Dir[0] =  sin(m_x * mouselook_RAD);
    Dir[1] =  1.0f;
    Dir[2] = -sin(m_z * mouselook_RAD/aspect);
*/
    float thetaRAD = m_x * mouselook_RAD;
    DirMouse[1] = Dir[0] * cos(thetaRAD) - Dir[1] * sin(thetaRAD); // DirM...[1] ?
    DirMouse[0] = Dir[0] * sin(thetaRAD) + Dir[1] * cos(thetaRAD); // DirM...[0] ?
    DirMouse[2] = -sin(m_z * mouselook_RAD/aspect);

//    At = Pos + Dir;
    At = Pos + DirMouse;
    Norm = glm::vec3(0, 0, 1);  // Head is up <-- change on Oculus!
  }

};
