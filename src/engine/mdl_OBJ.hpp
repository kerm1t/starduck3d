// -------------------------------------------------------
//
// Read Wavefront Obj file (e.g. exported with Blender)
//
// from:  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
//
// tried ASSIMP, but this needs DirectX-SDK
//
// OBJ file format, s. http://paulbourke.net/dataformats/obj/
// MTL file format, s.below
//
// Syntax:
// -------
//
// mtllib cbred.mtl <--- Lightwave / OBJ material format s. http://paulbourke.net/dataformats/mtl/
// # Coordinates for object 'CBRedB'
// v  - Vertex (x,y,z)
// vt - Texture vertex (x,y)
// vn - Normal vertex (x,y,z)
// f  - face (triangle) combines V,VT,VN indices
//
// usemtl CBRed <-- not used currently
//
// o <object_name>  e.g. CBRedB
// g <group_name>   e.g. CBRedB
// s off (smooth shading off)
//
// -------------------------------------------------------

#include "stdafx.h"
#pragma once

//#define GLM_FORCE_RADIANS <-- strange!
#include "glm.hpp"

// blender model from:  http://www.tutorialsforblender3d.com/Models/index_Models.html

bool loadOBJ(const char * path,
            std::vector <glm::vec3> & out_v,
            std::vector <glm::vec2> & out_vtex,
            std::vector <glm::vec3> & out_vnorm)
{
    printf("Loading OBJ file %s...\n", path);

    std::vector <unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector <glm::vec3> temp_vertices;
    std::vector <glm::vec2> temp_uvs;
    std::vector <glm::vec3> temp_normals;

    FILE * file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1)
    {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
 
        // else : parse lineHeader
        if (strcmp( lineHeader, "v" ) == 0)
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.z, &vertex.y);   // handgetuned f. Blender-Auto
            vertex = vertex * 0.04f;  // handgetuned f. Blender-Auto
			vertex.z -= 1.4f;         // handgetuned f. Blender-Auto
			temp_vertices.push_back(vertex);
        }
        else if (strcmp( lineHeader, "vt" ) == 0)
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
//            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);
        }
        else if (strcmp( lineHeader, "vn" ) == 0)
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp( lineHeader, "f" ) == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9)
            {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else
        {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }
    // For each vertex of each triangle
    for (unsigned int i=0; i<vertexIndices.size(); i++)
    {
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex     =     uvIndices[i];
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex-1];
        glm::vec2 uv     =      temp_uvs[uvIndex-1];
        glm::vec3 normal =  temp_normals[normalIndex-1];
        out_v    .push_back(vertex);
        out_vtex .push_back(uv);
        out_vnorm.push_back(normal);
    }
    return true;
}
