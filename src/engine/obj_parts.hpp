#pragma once

#include "mdl_wavefront.hpp"

// wrapper around mdl_wavefront (etc.) to load obj with parts
// and to create OpenGL VBO's & VAO's
namespace obj
{
  class CObj_parts: public CPart
  {
    
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
          if (v_mat[ui].map_Kd.compare("VRATA_KO.JPG") == 0) sTextureFullpath = sObjectDirectory + "\\" + "Vrata_kr.jpg";
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
            if (v_parts[i].s_Material.compare(v_mat[j].s_Material) == 0)
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


    void PartsToVBOs()
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
        // --------------------------------------------------------------------------------------
        // ist die Frage, ob das richtig ist, hier immer dieselbe Buffer-ID zu verwenden, s. hier
        // https://stackoverflow.com/questions/11580940/glgenbuffers-freeing-memory
        // hmm, anscheinend werden so alle Objekte im selben Buffer abgelegt
        // --------------------------------------------------------------------------------------

        // 2do: buffer wieder freigeben m. glDeleteBuffers (s. link oben)
        glGenBuffers(1, &p_render->positionBuffer[ui_idVBO + ui]);
        glBindBuffer(GL_ARRAY_BUFFER, p_render->positionBuffer[ui_idVBO + ui]);
        glBufferData(GL_ARRAY_BUFFER, v_parts[ui].vertices.size() * sizeof(glm::vec3), &(v_parts[ui].vertices[0]), GL_STATIC_DRAW); // init data storage
        if (v_parts[ui].b_textured)
        {
          glGenBuffers(1, &p_render->uvBuffer[ui_idVBO + ui]);
          glBindBuffer(GL_ARRAY_BUFFER, p_render->uvBuffer[ui_idVBO + ui]);
          glBufferData(GL_ARRAY_BUFFER, v_parts[ui].uvs.size() * sizeof(glm::vec2), &(v_parts[ui].uvs[0]), GL_STATIC_DRAW);
        }
        else
        {
          glGenBuffers(1, &p_render->colorBuffer[ui_idVBO + ui]);
          glBindBuffer(GL_ARRAY_BUFFER, p_render->colorBuffer[ui_idVBO + ui]);
          glBufferData(GL_ARRAY_BUFFER, v_parts[ui].cols.size() * sizeof(glm::vec3), &(v_parts[ui].cols[0]), GL_STATIC_DRAW); // init data storage
        }

        if ((err = glGetError()) != GL_NO_ERROR)
        {
          // Process/log the error.
          //          ui = 1;
        }
      }
    }

    void PartsToVAOs(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f))
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
  };

}