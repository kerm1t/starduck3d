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

#include "glm.hpp"
#include "math.h"

#include "vtf_typedef.h"

#include "proj_render.h"     // besser: proj_ext...
#include "mdl_wavefront.hpp" // load model
#include "img_bitmap.hpp"    // load texture

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx    

    class CBaseObject
    {
    public:
        proj::Render * p_render;
    };

    // 2do: richtiges naming verwenden ui_XXX, ...
    class CObject : CBaseObject // with parts!
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
//        glm::vec3 direction; // position - prev.position

        CObject(proj::Render * p_rnd)
        {     // <-- inline, sonst Linker error!
            position = glm::vec3(0.0f,0.0f,0.0f); // <--- ist das ok so ?

            p_render = p_rnd;
        };    // <-- inline

        void Load(float fScale = 1.0f, float fZ = 0.0f) // load OBJ 'n texture
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
//                    GLuint idGLTexture;
                    assert(sObjectDirectory.compare("")!=0);
                    std::string sTextureFullpath = sObjectDirectory + "\\" + v_parts[ui].s_Texture;
                    v_parts[ui].idGLTexture = ldrBMP.loadBMP_custom(sTextureFullpath.c_str());
                    p_render->vGLTexture.push_back(v_parts[ui].idGLTexture); // redundant!
                }
            }
        }

        // vertices,
        // textures --> OpenGL
        void BuffersToOpenGL(Vec3f vPos = Vec3f(0.0f,0.0f,0.0f))
        {
            unsigned int ui_idVBO = p_render->vVAOs.size();
            for (unsigned int ui=0; ui < v_parts.size(); ui++)
            {
                glGenBuffers(1, &p_render->positionBuffer[ui_idVBO+ui]);
                glBindBuffer(GL_ARRAY_BUFFER, p_render->positionBuffer[ui_idVBO+ui]);
                glBufferData(GL_ARRAY_BUFFER, v_parts[ui].vertices.size()*sizeof(glm::vec3), &(v_parts[ui].vertices[0]), GL_STATIC_DRAW); // init data storage
                if (v_parts[ui].b_textured)
                {
                    glGenBuffers(1, &p_render->uvBuffer[ui_idVBO+ui]);
                    glBindBuffer(GL_ARRAY_BUFFER, p_render->uvBuffer[ui_idVBO+ui]);
                    glBufferData(GL_ARRAY_BUFFER, v_parts[ui].uvs.size()*sizeof(glm::vec2), &(v_parts[ui].uvs[0]), GL_STATIC_DRAW);
                }
                else
                {
                    // Hack!! hier sollten tatsächlich die Farben 'rein
                    glGenBuffers(1, &p_render->colorBuffer[ui_idVBO+ui]);
                    glBindBuffer(GL_ARRAY_BUFFER, p_render->colorBuffer[ui_idVBO+ui]);
                    glBufferData(GL_ARRAY_BUFFER, v_parts[ui].uvs.size()*sizeof(glm::vec3), &(v_parts[ui].vertices[0]), GL_STATIC_DRAW);
                }
            }

            // --> die infos erstmal am Objekt speichern !?
            for (unsigned int ui=0; ui < v_parts.size(); ui++)
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
