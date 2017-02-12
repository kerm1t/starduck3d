// -----------------------
//
// Camera
//  -standard
//  -Oculus Rift
// -----------------------

#include "stdafx.h"
#pragma once

#include "glm.hpp" // is there an _ext ??
#include <gtc/matrix_transform.hpp>
#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library

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
	glm::mat4 View;       // camera move
	glm::mat4 Model;	  // e.g. object rotate / translate	

	GLuint * p_MVPMatrixAttrib;
// ====----

	// Projection (intrinsics)
	glm::float32 fovy_DEG, fovy_RAD; // e.g. 60.0f
	glm::float32 aspect;	// width/height
	glm::float32 zNear;		// e.g. 0.1f
	glm::float32 zFar;		// e.g. 100.0f

	// View (extrinsics)
	glm::vec3 Pos;
	glm::vec3 At;
	glm::vec3 Norm;

	Camera()
	{
		fovy_DEG =  60.0f;
		fovy_RAD = fovy_DEG * 3.14159f/180.0f;
		aspect   =   4.0f / 3.0f;
		zNear    =   1.0f;
		zFar     = 100.0f;

		Norm = glm::vec3(0.0f,0.0f,1.0f);
	}

	void changeAspect(int _width, int _height)
	{
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
	void updateView()
	{
//		Pos  = _Pos;
//		At   = _At;
//		Norm = _Norm;

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
	void changeModel(glm::mat4 _Model)
	{
		Model = _Model;
		uploadMVP();
	}
	void resetModel()
	{
		Model = glm::mat4(1.0f);
		uploadMVP();
	}

	void FollowObj()
	{
	}

	void Move_by_Mouse(glm::vec2 mouse)
	{
		float x, y, z;
		float xto, yto, zto;

		float w_half = width / 2.0f;
		float h_half = height/2.0f;
		float m_x = (mouse.x-w_half) / w_half;// * PI; // [0..1] 1 = 180deg = PI
		float m_z = (mouse.y-h_half) / h_half;      // Mouse-y = z-Achse

		// get position within scene
//		m_proj.m_render.get_xyz_Hack(iT,x,y,z,xto,yto,zto);
		x = 0.0f;
		y = 0.0f;
		z = 1.34f;

		xto  = sin(m_x * fovy_RAD) * 1.0f;
		yto  = y + 1.0f;
		zto  = z - sin(m_z * fovy_RAD/aspect) * 1.0f;
		Pos  = glm::vec3(x, y, z);               // Camera is at (x,y,z), in World Space
		At = glm::vec3(xto, yto, zto); // and looks at ...
		Norm = glm::vec3(0, 0, 1);  // Head is up <-- change on Oculus!
							
	}
};
