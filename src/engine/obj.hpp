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
    glm::vec3 position;
    //    glm::vec3 direction; // position - prev.position
    
    s_AABB aabb;
    unsigned int vaoID;

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

//    void PartsToVBOs(Vec3f vPos = Vec3f(0.0f, 0.0f, 0.0f))
    void PartsToVBOs()//glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f))
    {
      GLenum err = GL_NO_ERROR;
      err = glGetError(); // 3/8/2019 - when adding at draw time, I got an error 1282 here and object not shown
                          //            thus moved the Obj.load to Renderthread (before drawing) and ... worked!!

      unsigned int ui_idVBO = p_render->vVAOs.size();

      // 2do: einmal glGenBuffers mit Anzahl vert + uv + col aufrufen
      //     oder 1mal für vert,
      //          1mal für uv's,
      //          1mal für col

      uint16 nParts = (uint16)v_parts.size();
      for (uint16 ui = 0; ui < nParts; ui++)
      {
        // ---------------------------------------------------------------------------------
        // 2017-05-07
        // Problem, nur auf Nvidia: es können keine "gemischten" Objekte gerendert werden,
        //                          also solche, die sowohl Textur, als auch Farbe enthalten
        // ---------------------------------------------------------------------------------
        //        p_render->vVAOs[ui].idVBO_pos = (GLuint)p_render->ui_numVBOpos;
        err = glGetError();
        glGenBuffers(1, &p_render->positionBuffer[ui_idVBO + ui]);
        err = glGetError();
        glBindBuffer(GL_ARRAY_BUFFER, p_render->positionBuffer[ui_idVBO + ui]);
        err = glGetError();
        glBufferData(GL_ARRAY_BUFFER, v_parts[ui].vertices.size() * sizeof(glm::vec3), &(v_parts[ui].vertices[0]), GL_STATIC_DRAW); // init data storage
        err = glGetError();
        if (v_parts[ui].b_textured)
        {
          //          p_render->vVAOs[ui].idVBO_tex = (GLuint)p_render->ui_numVBOtex;
          glGenBuffers(1, &p_render->uvBuffer[ui_idVBO + ui]);
          glBindBuffer(GL_ARRAY_BUFFER, p_render->uvBuffer[ui_idVBO + ui]);
          glBufferData(GL_ARRAY_BUFFER, v_parts[ui].uvs.size() * sizeof(glm::vec2), &(v_parts[ui].uvs[0]), GL_STATIC_DRAW);
          err = glGetError();
          //          p_render->ui_numVBOtex++;
        }
        else
        {
          //          p_render->vVAOs[ui].idVBO_col = (GLuint)p_render->ui_numVBOcol;
          // ---------------------------------------------------------
          // für Farben sind im .obj-file keine Einzelwerte angegeben,
          // sondern nur der r,g,b-Wert
          // also hier "on the fly" erstellen!
          // ---------------------------------------------------------
          //          pf_Colors = new GLfloat[v_parts[ui].vertices.size()*sizeof(glm::vec3)]; // color.r/g/b

          // Hack!! hier sollten tatsächlich die Farben 'rein
          glGenBuffers(1, &p_render->colorBuffer[ui_idVBO + ui]);
          glBindBuffer(GL_ARRAY_BUFFER, p_render->colorBuffer[ui_idVBO + ui]);
          //          glBufferData(GL_ARRAY_BUFFER, v_parts[ui].vertices.size()*sizeof(glm::vec3), &(v_parts[ui].vertices[0]), GL_STATIC_DRAW); // init data storage
          glBufferData(GL_ARRAY_BUFFER, v_parts[ui].cols.size() * sizeof(glm::vec3), &(v_parts[ui].cols[0]), GL_STATIC_DRAW); // init data storage
          //          p_render->ui_numVBOcol++;
        }
        //        p_render->ui_numVBOpos++;

        //        assert(err != glGetError());
        if ((err = glGetError()) != GL_NO_ERROR)
        {
          // Process/log the error.
//          ui = 1;
        }
      }
    }

//    void PartsToVAOs(Vec3f vPos = Vec3f(0.0f, 0.0f, 0.0f))
    void PartsToVAOs(glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f))
    {
      // --> die infos erstmal am Objekt speichern !?
      uint16 nParts = (uint16)v_parts.size();

      if (nParts > 0) this->name = v_parts[0].name;

      for (uint16 ui = 0; ui < nParts; ui++)
      {
        proj::c_VAO vao;
        vao.Name = v_parts[ui].name;
        if (v_parts[ui].b_textured)
        {
          vao.t_Shade = proj::SHADER_TEXTURE;
          // noee                    vao.ui_idTexture = p_render->vGLTexture.size();
          // push texture to vGLTextures
          // noee                    std::string sTextureFilename = v_parts[ui].s_Texture;
          //                    assert(sObjectDirectory.compare("")!=0);
          //                    std::string sTextureFullpath = sObjectDirectory + "\\" + v_parts[ui].s_Texture;
          //                    p_render->vGLTexture.push_back(ldrBMP.loadBMP_custom(sTextureFullpath.c_str()));
          vao.ui_idTexture = v_parts[ui].idGLTexture; // id egal, nur größer 0 ??? 2do: check
        }
        else
        {
          vao.t_Shade = proj::SHADER_COLOR_FLAT;
        }
        vao.uiVertexCount = (uint16)v_parts[ui].vertices.size();
        vao.pos = pos;
        this->vaoID = p_render->vVAOs.size(); // 2do: easier (add in the obj.Create etc...) or merge vVAO and vObj??
        p_render->vVAOs.push_back(vao);
      }
    }
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

    void LoadParts(glm::vec3 pos, glm::vec3 dir = glm::vec3(0.0f, 1.0f, 0.0f), float fScale = 1.0f, float fZ = 0.0f) // load OBJ 'n texture
    {
      CLoader_OBJ ldr;
      // i) load geometry and material list
      bool res = ldr.loadOBJParts(sObjectFullpath.c_str(), v_mat, v_parts, fScale, fZ, dir);
      assert(res == true);


      // BBox
      aabb = ldr.aabb;
      aabb.min_point += pos;
      aabb.max_point += pos;

#if(B_ADD_BBOX_VAO == 1)
      obj::CCube2 m_cube;
      m_cube.p_render = p_render;
      proj::c_VAO vao = m_cube.Create("bbox", aabb.min_point, aabb.max_point);
      p_render->vVAOs.push_back(vao);
#endif

//      set_aabb(ldr.aabb.min_point + glm::vec3(vPos.x, vPos.y, vPos.z),
//               ldr.aabb.max_point + glm::vec3(vPos.x, vPos.y, vPos.z), true);


      const size_t last_slash_idx = sObjectFullpath.rfind('\\');
      if (std::string::npos != last_slash_idx)
      {
        sObjectDirectory = sObjectFullpath.substr(0, last_slash_idx);
      }
      else
      {
        sObjectDirectory = "";
      }

      // ii) load textures from MAT-list
      CIMGLoader ldrIMG;
      for (unsigned int ui = 0; ui < v_mat.size(); ui++)
      {
        assert(sObjectDirectory.compare("") != 0);
        if (!v_mat[ui].map_Kd.empty())
        {
          std::string sTextureFullpath = sObjectDirectory + "\\" + v_mat[ui].map_Kd;
          // Uffz, aus irgendwelchen Gruenden kann die folgende Textur nicht geladen werden
          if (v_mat[ui].map_Kd.compare("VRATA_KO.JPG")==0) sTextureFullpath = sObjectDirectory + "\\" + "Vrata_kr.jpg";
          v_mat[ui].idGLTexture = ldrIMG.loadIMG(sTextureFullpath.c_str());
//          p_render->vGLTexture.push_back(v_mat[ui].idGLTexture); // redundant!
          p_render->tex_map.insert(std::pair<std::string, GLuint>(sTextureFullpath, v_mat[ui].idGLTexture)); // redundant
        }
      }

      // iii) assign Texture-ID's to Parts
      for (unsigned int i = 0; i < v_parts.size(); i++)
      {
        if (v_parts[i].b_textured)
        {
          for (unsigned int j = 0; j < v_mat.size(); j++)
          {
            if (v_parts[i].s_Material.compare(v_mat[j].s_Material)==0)
            {
              v_parts[i].idGLTexture = v_mat[j].idGLTexture;
            }
          }
        }
        else
        {
          // Farbe --> VBO "on the fly" bauen, s. PartsToVBO()
        }
      }

    } // LoadParts


  }; // class CObjectWavefront



}
