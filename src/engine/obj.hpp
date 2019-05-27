/*

Generic class for 3d-Objects consisting of parts (C) anigators

2019-03-xx  AABB added
2017-05-27, VAO structure is parallel, VAO relates to a) simple object
                                                      b) part of ObjectLoaded
            moving is done on VAO
            later there should be dynamic (re)loading of objects,
            also independence from OpenGL (--> Vulcan) etc.
            for now we keep it simple
2017-05-25, CObject <- CObjectParts <- CAbstr_ObjectLoaded <- CObjectWavefront
2016-03-25, start of development

- shall be easy importable with OBJ file-structure

Use STL or not !? --> http://stackoverflow.com/questions/2226252/embedded-c-to-use-stl-or-not
           Twitter:   Fabien Sanglard? @fabynou (28 Sep 2016)
                      "More Boost this, STL that. Small ns overhead ultimately turn into µs and then ms."
*/

#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

//#define GLM_FORCE_RADIANS // disabled ... distorts the projection
#include "glm/glm.hpp"
#include "math.h"

#include "Vec3f.hxx"

#include "proj_render.h"     // besser: proj_ext...

#include "mdl_wavefront.hpp" // load model
#include "img_bitmap.hpp"    // load texture (.bmp)
#include "img_any.h"         // load texture (.jpg,.png,...)

#include "geometry.hpp"
#include "obj_parts.hpp"

//#include "obj_simple.hpp"    // bboxes

#define B_ADD_BBOX_VAO 1 // 0 | 1    2019-05-08, draw only that bbox, which is "touched"

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx    

  class CObject
  {
  protected:
    GLfloat* pf_Vert;
    GLfloat* pf_Col;
    int nVert; // <--- ?? wird unten benutzt, aber wozu: mehrere guardrails ?
    int nCol;

    void vert_Vpushback(std::vector<glm::vec3> vVert)
    {
      for (uint16 i = 0; i < vVert.size(); i++)
      {
        pf_Vert[nVert++] = vVert[i].x;
        pf_Vert[nVert++] = vVert[i].y;
        pf_Vert[nVert++] = vVert[i].z;
      }
    }
    void vert_pushback(glm::vec3 V)
    {
      pf_Vert[nVert++] = V.x;
      pf_Vert[nVert++] = V.y;
      pf_Vert[nVert++] = V.z;
    }
    void col_pushback(glm::vec3 C)
    {
      pf_Col[nCol++] = C.r;
      pf_Col[nCol++] = C.g;
      pf_Col[nCol++] = C.b;
    }

    void set_pos(glm::vec3 pos)
    {
    }

  public:
    std::string name;
    
    bool bHasParts;
// slows down to 1-8 FPS (before 130 FPS) std::vector <CPart> v_parts;
//    CPart a_parts[10];
// ==========================================================================================================================
// CPart stores all pos, uv- and col vertices as std::vector. That is not necessary. After loading those buffers to GFX card,
// the vectors are not needed anymore. Parts can store pos, direction etc.
// - but how to create VAO's then?
//   a) create VAO's within mdl_wavefront class
//   b) parts is a "global" std::vector, obj only has pointers, pointing to parts vector
//   c) es gibt eine von mdl_wavefront abgeleitete Klasse, die einliest, und i) parts und ii) VAO's erstellt, wobei
//      parts links auf die VAO's enthalten
// ==========================================================================================================================
    glm::vec3 position;
    //    glm::vec3 direction; // position - prev.position
    
    s_AABB aabb;
    unsigned int vaoID; // link to VAO, idea: combine vObjects and vVAO

    CObject()
    {
      bHasParts = false;
//      iNdx = 0;
      nVert = 0;
      nCol = 0;
    }
    ~CObject()
    {
//      delete[] pf_Col;
//      delete[] pf_Vert;
    }
  };

  // from here objects are interwoven with OpenGL (-> VBO / VAO)

  class CGL_Object : public CObject
  {
  protected:
/*    void set_aabb(glm::vec3 min, glm::vec3 max, bool b_create_VAO)
    {
      aabb.min_point = min;
      aabb.max_point = max;
      if (b_create_VAO)
      {
        obj::CCube2 m_cube;
        m_cube.p_render = p_render;
        proj::c_VAO vao = m_cube.Create("bbox", aabb.min_point, aabb.max_point);
        p_render->vVAOs.push_back(vao);
      }
    }
    */

  public:
    proj::Render * p_render;

    // a) colored
    void ToVBO(uint32 vCount, const GLvoid * p_coords, const GLvoid * p_colors)
    {
      assert(vCount > 0);
      // ---------------------------
      // >>> now Push to OpenGL! >>>
      // ---------------------------
      unsigned int ui_idVBO = p_render->vVAOs.size();
      glGenBuffers(1, &p_render->positionBuffer[ui_idVBO]); // <-- Achtung !! nimmt die aktuelle Anzahl VBO als index !!
      glBindBuffer(GL_ARRAY_BUFFER, p_render->positionBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, vCount * 3 * sizeof(GLfloat), p_coords, GL_STATIC_DRAW);

      glGenBuffers(1, &p_render->colorBuffer[ui_idVBO]);
      glBindBuffer(GL_ARRAY_BUFFER, p_render->colorBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, vCount * 3 * sizeof(GLfloat), p_colors, GL_STATIC_DRAW);
    }
    // b) textured
    void ToVBOTex(uint32 vCount, const GLvoid * p_coords, const glm::vec2 * p_uv)
    {
      assert(vCount > 0);
      // ---------------------------
      // >>> now Push to OpenGL! >>>
      // ---------------------------
      unsigned int ui_idVBO = p_render->vVAOs.size();
      glGenBuffers(1, &p_render->positionBuffer[ui_idVBO]); // <-- Achtung !! nimmt die aktuelle Anzahl VBO als index !!
      glBindBuffer(GL_ARRAY_BUFFER, p_render->positionBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, vCount * 3 * sizeof(GLfloat), p_coords, GL_STATIC_DRAW);

      glGenBuffers(1, &p_render->uvBuffer[ui_idVBO]);
      glBindBuffer(GL_ARRAY_BUFFER, p_render->uvBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, vCount * sizeof(glm::vec2), p_uv, GL_STATIC_DRAW);
    }
  };

  class CCube2 : public CGL_Object  /// hack! same as CCube in obj_simple, proposal: make obj.hpp und obj_GL.hpp, neeee, doch nicht
  {
  public:
    //    proj::Render * p_render;

    proj::c_VAO Create(std::string name, glm::vec3 min, glm::vec3 max)
    {
      this->name = name;

      std::vector<glm::vec3> coords;
      coords.push_back({ min.x,min.y,min.z });
      coords.push_back({ min.x,max.y,min.z });
      coords.push_back({ max.x,max.y,min.z });

      coords.push_back({ min.x,min.y,min.z });
      coords.push_back({ max.x,max.y,min.z });
      coords.push_back({ max.x,min.y,min.z });

      coords.push_back({ min.x,min.y,max.z });
      coords.push_back({ min.x,max.y,max.z });
      coords.push_back({ max.x,max.y,max.z });

      coords.push_back({ min.x,min.y,max.z });
      coords.push_back({ max.x,max.y,max.z });
      coords.push_back({ max.x,min.y,max.z });

      uint32 vCount = 12;

      std::vector<glm::vec3> cols;
      for (unsigned int i = 0; i < vCount; i++)
      {
        cols.push_back({ 1.0f, 1.0f, 1.0f }); // r,g,b
      }

      // ---------------------------
      // >>> now Push to OpenGL! >>>
      // ---------------------------
      ToVBO(vCount, &coords[0], &cols[0]);

      proj::c_VAO cube;
      cube.t_Shade = proj::SHADER_COLOR_FLAT;
      cube.Name = name;
      cube.uiVertexCount = vCount;
      return cube;
    }
  }; // class CCube2


  class CGL_ObjectParts : public CGL_Object // ist per default private, dann Variablen nicht zugänglich
  {
  public:
    // parts --> move them? e.g. wheels
    std::vector <CPart> v_parts;

    CGL_ObjectParts()
    {
      bHasParts = true;
    }

// ...
  }; // class CObjectParts



  class CGL_Abstr_ObjectLoaded : public CGL_ObjectParts
  {
  public:
    // load .obj
    std::string sObjectFullpath;
    std::string sObjectDirectory;

    virtual void Load(glm::vec3, glm::vec3, float, float) = 0;  // includes ToVBO, ToVAO
    virtual void LoadParts(glm::vec3, glm::vec3, float, float) = 0;    // load OBJ 'n texture
  };



  class CGL_ObjectWavefront : public CGL_Abstr_ObjectLoaded
  {
  public:
    std::vector <CMaterial> v_mat;
    CGL_ObjectWavefront() {}; // default constructor
    CGL_ObjectWavefront(proj::Render * p_rnd)
    {     // <-- inline, sonst Linker error!
      position = glm::vec3(0.0f, 0.0f, 0.0f); // <--- ist das ok so ?

      p_render = p_rnd;
    };    // <-- inline
    
    void setRender(proj::Render * p_rnd)
    {
      p_render = p_rnd;
    };

    void Load(glm::vec3 pos, glm::vec3 dir = glm::vec3(0.0f,1.0f,0.0f), float fScale = 1.0f, float fZ = 0.0f) // load OBJ 'n texture
    {
      LoadParts(pos, dir, fScale, fZ); // nothing done with pos here
      PartsToVBOs();// pos);
      PartsToVAOs(pos); // pos. stored @ obj and later applied in the shader
    }



  }; // class CObjectWavefront



}
