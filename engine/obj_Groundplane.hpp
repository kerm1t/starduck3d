/*
#include "stdafx.h"
#pragma once

#include "inc_render.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library

class Groundplane
{    
    void Create_VAO()
    {
        std::vector<GLfloat> coords;
        float fPlanewidth = 300.0f;
    // 3D
        coords.push_back( fPlanewidth); coords.push_back( fPlanewidth); coords.push_back(-0.1f);
        coords.push_back(-fPlanewidth); coords.push_back( fPlanewidth); coords.push_back(-0.1f); 
        coords.push_back( fPlanewidth); coords.push_back(-fPlanewidth); coords.push_back(-0.1f); 

        coords.push_back( fPlanewidth); coords.push_back(-fPlanewidth); coords.push_back(-0.1f); 
        coords.push_back(-fPlanewidth); coords.push_back( fPlanewidth); coords.push_back(-0.1f); 
        coords.push_back(-fPlanewidth); coords.push_back(-fPlanewidth); coords.push_back(-0.1f);

        std::vector<GLfloat> cols;
        cols.push_back(0.0f); cols.push_back(1.0f); cols.push_back(0.0f); 
        cols.push_back(0.0f); cols.push_back(1.0f); cols.push_back(0.0f); 
        cols.push_back(0.0f); cols.push_back(1.0f); cols.push_back(0.0f); 

        cols.push_back(0.0f); cols.push_back(1.0f); cols.push_back(0.0f); 
        cols.push_back(0.0f); cols.push_back(1.0f); cols.push_back(0.0f); 
        cols.push_back(0.0f); cols.push_back(1.0f); cols.push_back(0.0f); 

        // ---------------------------
        // >>> now Push to OpenGL! >>>
        // ---------------------------
        unsigned int ui_idVBO = vVAOs.size();
        glGenBuffers(1, &positionBuffer[ui_idVBO]);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
        glBufferData(GL_ARRAY_BUFFER, 6*3*sizeof(GLfloat), &coords[0], GL_STATIC_DRAW);

        glGenBuffers(1, &colorBuffer[ui_idVBO]);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[ui_idVBO]);
        glBufferData(GL_ARRAY_BUFFER, 6*3*sizeof(GLfloat), &cols[0], GL_STATIC_DRAW);

        c_VAO plane;
        plane.t_Shade = SHADER_COLOR_FLAT;
        plane.uiVertexCount = 6;
        vVAOs.push_back(plane);
    }
};
*/
