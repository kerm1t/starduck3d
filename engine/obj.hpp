/*

Generic class for 3d-Objects consisting of parts (C) Anigators

2016-03-25, start of development

- shall be easy importable with OBJ file-structure

Use STL or not !? --> http://stackoverflow.com/questions/2226252/embedded-c-to-use-stl-or-not

*/

#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

//#define GLM_FORCE_RADIANS // 2do, replace all deg. by rad.!!, then remove this line
#include "glm.hpp"
#include "math.h"

#include "vtf_typedef.h"

#include "proj_render.h"     // besser: proj_ext...
#include "mdl_wavefront.hpp" // load model
#include "img_bitmap.hpp"    // load texture

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx    

  class CObject
  {
  public:
    void Load(float, float, Vec3f);     // includes ToVBO, ToVAO
    void LoadParts(float, float);       // load OBJ 'n texture
    virtual void PartsToVBOs(Vec3f) = 0;     // parts to VBOs
    virtual void PartsToVAOs(Vec3f) = 0;     // parts to VAOs

    proj::Render * p_render;
  };

  // 2do: richtiges naming verwenden ui_XXX, ...
  class CObjectWavefront : CObject // with parts!
  {
  public:
    // load .obj
    std::string sObjectFullpath;
    std::string sObjectDirectory;

    // parts --> move them? e.g. wheels
    std::vector <CPart> v_parts;

    // -------------------------------------------------------------
    // 2do: merge these and new Buffers for blender model reading !?
    // -------------------------------------------------------------

    glm::vec3 position;
    // glm::vec3 direction; // position - prev.position

    CObjectWavefront(proj::Render * p_rnd)
    {     // <-- inline, sonst Linker error!
      position = glm::vec3(0.0f,0.0f,0.0f); // <--- ist das ok so ?

      p_render = p_rnd;
    };    // <-- inline

    void Load(float fScale = 1.0f, float fZ = 0.0f, Vec3f vPos = Vec3f(0.0f, 0.0f, 0.0f)) // load OBJ 'n texture
    {
      LoadParts(fScale, fZ);
      PartsToVBOs(vPos);
      PartsToVAOs(vPos);
    }

    void LoadParts(float fScale = 1.0f, float fZ = 0.0f) // load OBJ 'n texture
    {
      CLoader_OBJ ldr;
      bool res = ldr.loadOBJParts(sObjectFullpath.c_str(), v_parts, fScale, fZ);
      assert(res == true);

      const size_t last_slash_idx = sObjectFullpath.rfind('\\');
      if (std::string::npos != last_slash_idx)
      {
        sObjectDirectory = sObjectFullpath.substr(0, last_slash_idx);
      }
      else
      {
        sObjectDirectory = "";
      }

      CBMPLoader ldrBMP;
      for (unsigned int ui=0; ui < v_parts.size(); ui++)
      {
        if (v_parts[ui].b_textured)
        {
//          GLuint idGLTexture;
          assert(sObjectDirectory.compare("")!=0);
          std::string sTextureFullpath = sObjectDirectory + "\\" + v_parts[ui].s_Texture;
          v_parts[ui].idGLTexture = ldrBMP.loadBMP_custom(sTextureFullpath.c_str());
          p_render->vGLTexture.push_back(v_parts[ui].idGLTexture); // redundant!
        }
      }
    }

    // vertices,
    // texture-uv's, colors --> OpenGL
    void PartsToVBOs(Vec3f vPos = Vec3f(0.0f, 0.0f, 0.0f))
    {
      GLenum err = GL_NO_ERROR;

      unsigned int ui_idVBO = p_render->vVAOs.size();

      // 2do: einmal glGenBuffers mit Anzahl vert + uv + col aufrufen
      //     oder 1mal für vert,
      //          1mal für uv's,
      //          1mal für col
      //      for (unsigned int ui = 0; ui < v_parts.size(); ui++)
      unsigned int max;
      max = v_parts.size();
//      if (max > 3) max = 3;
      for (unsigned int ui = 0; ui < max; ui++)
      {
        if (!v_parts[ui].b_textured) continue;
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
        glBufferData(GL_ARRAY_BUFFER, v_parts[ui].vertices.size()*sizeof(glm::vec3), &(v_parts[ui].vertices[0]), GL_STATIC_DRAW); // init data storage
        err = glGetError();
        if (v_parts[ui].b_textured)
        {
          //          p_render->vVAOs[ui].idVBO_tex = (GLuint)p_render->ui_numVBOtex;
          glGenBuffers(1, &p_render->uvBuffer[ui_idVBO + ui]);
          glBindBuffer(GL_ARRAY_BUFFER, p_render->uvBuffer[ui_idVBO + ui]);
          glBufferData(GL_ARRAY_BUFFER, v_parts[ui].uvs.size()*sizeof(glm::vec2), &(v_parts[ui].uvs[0]), GL_STATIC_DRAW);
          err = glGetError();
          //          p_render->ui_numVBOtex++;
        }
        else
        {
          //          p_render->vVAOs[ui].idVBO_col = (GLuint)p_render->ui_numVBOcol;
          // Hack!! hier sollten tatsächlich die Farben 'rein
          glGenBuffers(1, &p_render->colorBuffer[ui_idVBO + ui]);
          glBindBuffer(GL_ARRAY_BUFFER, p_render->colorBuffer[ui_idVBO + ui]);
          glBufferData(GL_ARRAY_BUFFER, v_parts[ui].uvs.size()*sizeof(glm::vec3), &(v_parts[ui].vertices[0]), GL_STATIC_DRAW);
          //          p_render->ui_numVBOcol++;
        }
        //        p_render->ui_numVBOpos++;

        //        assert(err != glGetError());
        if ((err = glGetError()) != GL_NO_ERROR)
        {
          //Process/log the error.
          ui = 1;
        }
      }
    }

    void PartsToVAOs(Vec3f vPos = Vec3f(0.0f, 0.0f, 0.0f))
    {
      // --> die infos erstmal am Objekt speichern !?
//      for (unsigned int ui = 0; ui < v_parts.size(); ui++)
      unsigned int max;
      max = v_parts.size();
      for (unsigned int ui = 0; ui < max; ui++)
      {
        if (!v_parts[ui].b_textured) continue;
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
          vao.ui_idTexture = v_parts[ui].idGLTexture;
        }
        else
        {
          vao.t_Shade = proj::SHADER_COLOR_FLAT;
        }
        vao.uiVertexCount = v_parts[ui].vertices.size();
        vao.vPos = vPos;
        p_render->vVAOs.push_back(vao);
      }

    }
  };

}
