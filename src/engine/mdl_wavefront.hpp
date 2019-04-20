// -------------------------------------------------------
//
// Read Wavefront Obj file (e.g. exported with Blender)
// uses state machine
//
// from:  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
//
// OBJ file format, s. http://paulbourke.net/dataformats/obj/, http://www.cs.utah.edu/~boulos/cs3505/obj_spec.pdf
// MTL file format, s.below
//
// Syntax:
// -------
//
// mtllib cbred.mtl <--- Lightwave / OBJ material format s. http://paulbourke.net/dataformats/mtl/
// # Coordinates for object 'CBRedB'
//
// g <group_name>   e.g. CBRedB  (optional!)
// o <object_name>  e.g. CBRedB  (optional!)
// v  - Vertex (x,y,z)
// vt - Texture vertex (x,y)
// vn - Normal vertex (x,y,z)
// usemtl CBRed --> get "newmtl CBRed" from .mtl-file
// f  - face (triangle) combines V,VT,VN indices
//                               V,VN (not textured)
//                               V,VT (e.g. normals in extra bumpmap)
// s off (smooth shading off)
//
// maybe later replace with: https://github.com/syoyo/tinyobjloader
// -------------------------------------------------------

#include "stdafx.h"
#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>    // For std::remove()

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

//#define GLM_FORCE_RADIANS // 2do, replace all deg. by rad.!!, then remove this line
#include "glm/glm.hpp"
#include "../common/types.h"

#include "geometry.hpp"

// blender model from:  http://www.tutorialsforblender3d.com/Models/index_Models.html

namespace obj // constructor, functions are **implicitly** inline, s. http://stackoverflow.com/questions/16441036/when-using-a-header-only-in-c-c
{             // how to put all into.h file --> s. Vec3f.hxx    
  enum state { os_none, os_v, os_f, os_usemtl };

  class CMaterial; // forward declaration
  class CPart
  {
  public:
    // texture
    std::string            name;
    bool                   b_textured;
    std::string            s_Texture;  // deprecated, to be replaced by material
    GLuint                 idGLTexture; 
    std::string            s_Material;
//    CMaterial            * p_Mat;
    glm::vec3              Ka;
    glm::vec3              Kd;
    glm::vec3              Ks;
    // read the model -->
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals; // won't be used at the moment. (for bumpmaps??)
    std::vector<glm::vec3> cols;    // ... if not textured
  };

  class CMaterial
  {
  public:
    std::string s_Material;
    float       Ns;     // specular exponent, focus of the specular highlight
    glm::vec3   Ka;     //  ambient reflectivity (rgb)
    glm::vec3   Kd;     //  diffuse reflectivity (rgb) <-- use this in the first place
    glm::vec3   Ks;     // specular reflectivity (rgb)
    glm::vec3   Ke;     // ? (not in Paul Bourke's description)
    float       Ni;     // optical density [0.001 - 10]
    float       d;      // dissolve factor 1.0f = fully opaque 
    int         illum;  // illumination model
    /*
        0   Color on and Ambient off
        1   Color on and Ambient on
        2   Highlight on
        3   Reflection on and Ray trace on
        4   Transparency: Glass on
            Reflection: Ray trace on
        5   Reflection: Fresnel on and Ray trace on
        6   Transparency: Refraction on
            Reflection: Fresnel off and Ray trace on
        7   Transparency: Refraction on
            Reflection: Fresnel on and Ray trace on
        8   Reflection on and Ray trace off
        9   Transparency: Glass on
            Reflection: Ray trace off
       10   Casts shadows onto invisible surfaces
    */
    std::string map_Kd; // texture's filename
    GLuint idGLTexture;

    void Init()
    {
      s_Material = "";
      Ns = 0.0f;
      Ka = glm::vec3(0.0f,0.0f,0.0f);
      Kd = glm::vec3(0.0f,0.0f,0.0f);
      Ks = glm::vec3(0.0f,0.0f,0.0f);
      Ke = glm::vec3(0.0f,0.0f,0.0f);
      Ni = 0.0f;
      d  = 0.0f;
      illum = 0;
      map_Kd = ""; // texture's filename
    }
  };

  class CLoader_OBJ
  {
  public:
    s_AABB aabb;

    // http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    std::string load_fast(const char *filename)
    {
      std::ifstream in(filename, std::ios::in | std::ios::binary);
      if (in)
      {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
      }
      throw(errno);
    }
    
    std::vector<std::string> split(const char *str, char c = ' ')
    {
      std::vector<std::string> result;

      do
      {
        const char *begin = str;

        while (*str != c && *str)
          str++;

        result.push_back(std::string(begin, str));
      } while (0 != *str++);

      return result;
    }

    bool loadMaterials(const char * path, std::vector <CMaterial> & out_v_CMaterials)
    {
      uint16 nMaterialsRead = 0;
      CMaterial material;
      char buf[255];

      std::ifstream file;
      file.open(path);

      std::string line;

      while (file.good())
      {
        std::getline(file,line);
        line.erase(std::remove(line.begin(), line.end(), '\t'), line.end()); // remove TAB's
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end()); // remove Spaces

        if (line.compare(0,6,"newmtl") == 0)
        {
          if (nMaterialsRead++ > 0)
          {
            out_v_CMaterials.push_back(material);
            material.Init();
          }
          sscanf(line.c_str(), "newmtl %s\n", &buf);
          material.s_Material = buf;
        }
        else if (line.compare(0,2,"Ns") == 0)
        {
          float fNs;
          sscanf(line.c_str(), "Ns %f\n", &fNs);
          material.Ns = fNs;
        }
        else if (line.compare(0,2,"Ka") == 0) // ambient ?
        {
          glm::vec3 Ka;
          sscanf(line.c_str(), "Ka %f %f %f\n", &Ka.x, &Ka.y, &Ka.z);
          material.Ka = Ka;
        }
        else if (line.compare(0,2,"Kd") == 0) // diffuse ?
        {
          glm::vec3 Kd;
          sscanf(line.c_str(), "Kd %f %f %f\n", &Kd.x, &Kd.y, &Kd.z);
          material.Kd = Kd;
        }
        else if (line.compare(0,2,"Ks") == 0) // specular ?
        {
          glm::vec3 Ks;
          sscanf(line.c_str(), "Ks %f %f %f\n", &Ks.x, &Ks.y, &Ks.z);
          material.Ks = Ks;
        }
        else if (line.compare(0,2,"Ni") == 0)
        {
          float fNi;
          sscanf(line.c_str(), "Ni %f\n", &fNi);
          material.Ni = fNi;
        }
        else if (line.compare(0,1,"d") == 0)
        {
          float fd;
          sscanf(line.c_str(), "d %f\n", &fd);
          material.d = fd;
        }
        else if (line.compare(0,5,"illum") == 0)
        {
          int illum;
          sscanf(line.c_str(), "illum %d\n", &illum);
          material.illum = illum;
        }
        else if (line.compare(0,6,"map_Kd") == 0) // 2do: map_Ka
        {
          sscanf(line.c_str(), "map_Kd %s\n", buf);
          material.map_Kd = buf;
        }
      }
      file.close();

      out_v_CMaterials.push_back(material);

      return true;
    }

    void AddPart(std::vector <CPart> & out_v_CParts,
      char * p_tmp_Obj, char * p_tmp_Mat, std::vector <CMaterial> & v_Mat,
      std::vector <glm::vec3> & temp_vertices,
      std::vector <glm::vec2> & temp_uvs,
      std::vector <glm::vec3> & temp_normals,
      std::vector <unsigned int> & face_v,
      std::vector <unsigned int> & face_vt,
      std::vector <unsigned int> & face_vn)
    {
      CPart part;
      part.name       = p_tmp_Obj;
      part.s_Material = p_tmp_Mat;
      for (unsigned int i=0;i<v_Mat.size();i++)
      {
        if (part.s_Material.compare(v_Mat[i].s_Material) == 0)
        {
          part.Ka = v_Mat[i].Ka;
          part.Kd = v_Mat[i].Kd;
          part.Ks = v_Mat[i].Ks;
          part.s_Texture = v_Mat[i].map_Kd;
        }
      }
      part.b_textured = (part.s_Texture.size() > 0);

      // For each vertex of each triangle
      for (unsigned int i=0; i<face_v.size(); i++)
      {
        unsigned int vertexIndex = face_v[i];
        glm::vec3 vertex = temp_vertices[vertexIndex-1];
        part.vertices.push_back(vertex);

        // span bbox
        if (vertex.x < aabb.min_point.x) aabb.min_point.x = vertex.x; // min
        if (vertex.y < aabb.min_point.y) aabb.min_point.y = vertex.y;
        if (vertex.z < aabb.min_point.z) aabb.min_point.z = vertex.z;
        if (vertex.x > aabb.max_point.x) aabb.max_point.x = vertex.x; // max
        if (vertex.y > aabb.max_point.y) aabb.max_point.y = vertex.y;
        if (vertex.z > aabb.max_point.z) aabb.max_point.z = vertex.z;

        if (part.b_textured)
        {
          unsigned int uvIndex = face_vt[i];
          glm::vec2 uv = temp_uvs[uvIndex-1];
          uv.y = 1.0f - uv.y; // hack for imgANY loader
          part.uvs.push_back(uv);
        }
        else
        {
          // depending on illum
//          part.cols.push_back(part.Kd); // ...try... (Ks, Kd, how to combine??)
          unsigned int normalIndex = face_vn[i];
          float f = glm::dot(part.Ka,temp_normals[normalIndex - 1]);
          part.cols.push_back(f * part.Kd + part.Ks); // a bisserl gehackt hier, aber tut's erstmal
        }
        if (temp_normals.size() > 0) // 2017-07-24, hack
        {
          unsigned int normalIndex = face_vn[i];
          glm::vec3 normal = temp_normals[normalIndex-1];
          part.normals.push_back(normal);
        }
      }
      face_v.clear();   // check parts' size with & without
      face_vt.clear();  // check parts' size with & without
      face_vn.clear();  // check parts' size with & without
      out_v_CParts.push_back(part);
    }

    /*
      Load .obj:
      ----------
      ... contains mtllib "xxxx.mtl" --> Load .mtl (should be at start of .obj-file)
      ... load "parts"
      provide parts with appropriate Material (set pointer)
    */
    bool loadOBJParts(const char * filename, std::vector <CMaterial> & v_Mat,
      std::vector <CPart> & out_v_CParts, float fScale = 1.0f, float fZ = 0.0f)
    {
      state objstate = os_none;

      printf("Loading OBJ file %s...\n", filename);

      char temp_object[255];
      char mtllib[255];
      char temp_material[255];

//      std::vector <CMaterial> v_Mat;
      std::vector <glm::vec3> temp_vertices;
      std::vector <glm::vec2> temp_uvs;
      std::vector <glm::vec3> temp_normals;
      std::vector <unsigned int> face_v, face_vt, face_vn; // indices

      std::string file = load_fast(filename);
      std::vector<std::string> lines = split(file.c_str(), '\n');

      for (int iln=0; iln < lines.size(); iln++)
      {
        std::string line = lines[iln];

        if (line == "") continue;

        std::vector<std::string> tok = split(line.c_str(), ' ');
        if (tok[0] == "#") // comment
        {
          OutputDebugString(_T(line.c_str()));
          OutputDebugString(_T("\n"));
        }
        else if (tok[0] == "mtllib")
        {
          std::string s_path = filename;
          std::string s_dir;
          const size_t last_slash_idx = s_path.rfind('\\');
          if (std::string::npos != last_slash_idx)
          {
            s_dir = s_path.substr(0, last_slash_idx);
          }
          sscanf(line.c_str(), "mtllib %s\n", &mtllib);
          char buf[255];
          sprintf(buf, "%s\\%s", s_dir.c_str(), mtllib);
          // -------------------
          // Load Materials File
          // -------------------
          loadMaterials(buf, v_Mat);
        }
        else if (tok[0] == "v")
        {
          if ((objstate != os_none) && (objstate != os_v))
          {
            if (strlen(temp_object) == 0) strcpy(temp_object, mtllib); // Hack!
            AddPart(out_v_CParts,
              temp_object, temp_material, v_Mat,
              temp_vertices,
              temp_uvs,
              temp_normals,
              face_v,
              face_vt,
              face_vn);
          }
          objstate = os_v;
          glm::vec3 vertex;
          sscanf(line.c_str(), "v %f %f %f\n", &vertex.x, &vertex.z, &vertex.y);
          vertex = vertex * fScale; // Blender-OBJ tuning
          vertex.z -= fZ;           // Blender-OBJ tuning
          temp_vertices.push_back(vertex);
        }
        else if (tok[0] == "vt")
        {
          glm::vec2 uv;
          sscanf(line.c_str(), "vt %f %f\n", &uv.x, &uv.y);
          //            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
          temp_uvs.push_back(uv);
        }
        else if (tok[0] == "vn")
        {
          glm::vec3 normal;
          sscanf(line.c_str(), "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);
          temp_normals.push_back(normal);
        }
        else if (tok[0] == "usemtl")
        {
          // -------------------------------------------------------
          // if I comment in the following part,
          // there will be an access violation in the Sponza part 36
          // -------------------------------------------------------
          if (objstate == os_f)
          {
            AddPart(out_v_CParts,
              temp_object, temp_material, v_Mat,
              temp_vertices,
              temp_uvs,
              temp_normals,
              face_v,
              face_vt,
              face_vn);
          }
          objstate = os_usemtl;
          sscanf(line.c_str(), "usemtl %s\n", temp_material);
        }
        else if (tok[0] == "f") // face -- indices of v=vertices,vt=texture_uv,vn=normals
        {
          objstate = os_f; // obj-reader state now "f"
          unsigned int v[255], vt[255], vn[255];
          unsigned int nVert = 0;
          enum facetype { VTN = 7, VT = 3, VN = 5, N = 4, T = 2, V = 1}; // bitfield: 0 == V, 1 == T, 2 == N
          facetype ft;

          assert(nVert < 256);

          for (int itok = 1; itok < tok.size(); itok++) // start from 1 as first tok is "f"
          {
            int matches = sscanf(tok[itok].c_str(), "%d/%d/%d", &v[itok-1], &vt[itok - 1], &vn[itok - 1]);
            if (matches == 3)
            {
              ft = VTN;
              nVert++;
            }
            else
            {
              int matches = sscanf(tok[itok].c_str(), "%d/%d", &v[itok - 1], &vt[itok - 1]);
              if (matches == 2)
              {
                ft = VT;
                nVert++;
              }
              else
              {
                int matches = sscanf(tok[itok].c_str(), "%d//%d", &v[itok - 1], &vn[itok - 1]);
                if (matches == 2)
                {
                  ft = VN;
                  nVert++;
                }
                else
                {
                  int matches = sscanf(tok[itok].c_str(), "%d", &v[itok - 1]);
                  if (matches == 1)
                  {
                    ft = V;
                    nVert++;
                  }
                }
              }
            }
          } // for (int itok = 1; itok < tok.size(); itok++)

          switch (nVert)
          {
          case 3:
//            break;
            if (ft & V)
            {
              face_v.push_back(v[0]);
              face_v.push_back(v[1]);
              face_v.push_back(v[2]);
            }
            if (ft & T)
            {
              face_vt.push_back(vt[0]);
              face_vt.push_back(vt[1]);
              face_vt.push_back(vt[2]);
            }
            if (ft & N)
            {
              face_vn.push_back(vn[0]);
              face_vn.push_back(vn[1]);
              face_vn.push_back(vn[2]);
            }
            break;
          case 4:
//            break;
            if (ft & V)
            {
              face_v.push_back(v[0]);
              face_v.push_back(v[1]);
              face_v.push_back(v[3]);
              face_v.push_back(v[1]);
              face_v.push_back(v[2]);
              face_v.push_back(v[3]);
            }
            if (ft & T)
            {
              face_vt.push_back(vt[0]);
              face_vt.push_back(vt[1]);
              face_vt.push_back(vt[3]);
              face_vt.push_back(vt[1]);
              face_vt.push_back(vt[2]);
              face_vt.push_back(vt[3]);
            }
            if (ft & N)
            {
              face_vn.push_back(vn[0]);
              face_vn.push_back(vn[1]);
              face_vn.push_back(vn[3]);
              face_vn.push_back(vn[1]);
              face_vn.push_back(vn[2]);
              face_vn.push_back(vn[3]);
            }
            break;
          default:
            // not 3 and not 4 vertices -> more than 5
//            assert(false);
// https://stackoverflow.com/questions/23723993/converting-quadriladerals-in-an-obj-file-into-triangles
/*            int null = 0;
            for (unsigned int i = 1; i < nVert-2; i++) // triangle fan
            {
              if (ft & V)
              {
                face_v.push_back(v[null]);
                face_v.push_back(v[i]);
                face_v.push_back(v[i+1]);
              }
              if (ft & T)
              {
                face_vt.push_back(vt[null]);
                face_vt.push_back(vt[i]);
                face_vt.push_back(vt[i+1]);
              }
              if (ft & N)
              {
                face_vn.push_back(vn[null]);
                face_vn.push_back(vn[i]);
                face_vn.push_back(vn[i+1]);
              }
            }
            */
            if (ft & V)
            {
              face_v.push_back(v[0]);
              face_v.push_back(v[1]);
              face_v.push_back(v[2]);
            }
            if (ft & T)
            {
              face_vt.push_back(vt[0]);
              face_vt.push_back(vt[1]);
              face_vt.push_back(vt[2]);
            }
            if (ft & N)
            {
              face_vn.push_back(vn[0]);
              face_vn.push_back(vn[1]);
              face_vn.push_back(vn[2]);
            }
            for (unsigned int i = 3; i < nVert; ++i)
            {
              if (ft & V)
              {
                face_v.push_back(v[i-3]);
                face_v.push_back(v[i-1]);
                face_v.push_back(v[i]);
              }
              if (ft & T)
              {
                face_vt.push_back(vt[i-3]);
                face_vt.push_back(vt[i-1]);
                face_vt.push_back(vt[i]);
              }
              if (ft & N)
              {
                face_vn.push_back(vn[i-3]);
                face_vn.push_back(vn[i-1]);
                face_vn.push_back(vn[i]);
              }
            }
          } // switch (nVert)
        } // if (tok[0] == "f")
        else if ((tok[0] == "o") || (tok[0] == "g"))
        {
          int match = sscanf(line.c_str(), "o %s\n", temp_object);     // store object name
          if (match == 0) sscanf(line.c_str(), "g %s\n", temp_object); // store group name
        }
        else
        {
          // Probably a comment, eat up the rest of the line
          // [...]
        }
      }

      AddPart(out_v_CParts,
        temp_object, temp_material, v_Mat,
        temp_vertices,
        temp_uvs,
        temp_normals,
        face_v,
        face_vt,
        face_vn);

      return true;
    }
  };
}
