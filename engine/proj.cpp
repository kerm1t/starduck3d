#include "stdafx.h"
#include "proj.h"
#include "Vec3f.hxx"

#include "img_bitmap.hpp"
#include "img_DDS.hpp"
#include "glm.hpp"

#include "mdl_blender.hpp" // wenn ich in proj.h includiere, dann Linker-Fehler
#include "proj_scenebuilder.hpp"

proj::Proj::Proj()
{
}

int proj::Proj::Init()
{
	m_render.Init(); // kann auch später aufgerufen werden...
	m_render.InitShaders();

	// load sample data / Textures
	LoadTextures();

	// load geometry data
	// a) fill in all pos, color, uv infos into arrays
	LoadVAOs();
	
	// b) bind to VAO's
	m_render.Bind_VAOs(); // now hand over VBO's to VAO's

	vTrajPosprev = glm::vec3(0.0f,0.0f,0.0f);

	return 0;
}

void proj::Proj::LoadMoving() // e.g. cars
{
    Vec3f p = Vec3f(2.0f,1.0f,0.0f);
    Vec3f d = Vec3f(3.0f,1.0f,0.0f);

	// a) object in the traditional way
	m_moving[0].vCount         = m_render.vCount;
    m_moving[0].vertexArray    = m_render.vertexArray;
    m_moving[0].positionBuffer = m_render.positionBuffer;
    m_moving[0].colorBuffer    = m_render.colorBuffer;
    m_moving[0].VBOindex       = VBO_MOVING1;

    m_moving[0].Init(1);
    m_moving[0].iNdx = 0;
    m_moving[0].Add_Cube(p.x,p.y,p.z,  d.x,d.y,d.z);
    m_moving[0].Fini();

	// b) Read Blender .obj file(s)
    m_moving[1].vCount         = m_render.vCount;
    m_moving[1].vertexArray    = m_render.vertexArray;
    m_moving[1].positionBuffer = m_render.positionBuffer;
    m_moving[1].colorBuffer    = m_render.colorBuffer;
    m_moving[1].uvBuffer       = m_render.uvBuffer;
    m_moving[1].VBOindex       = VBO_MOVING2;
//    bool res = loadOBJ("D:\\projects\\VC\\glShoot_VAO_Texture_Fixed\\cube.obj", m_moving[1].vertices, m_moving[1].uvs, m_moving[1].normals);
	bool res = loadOBJ("C:\\__glShoot_Files\\LowPoly_Car\\cbred_stripped.obj", m_moving[1].vertices, m_moving[1].uvs, m_moving[1].normals);
	// texture is applied while rendering
    m_moving[1].Fini_blend();
    m_moving[1].Move(glm::vec3(0.0f,0.0f,1.0f));
}

int proj::Proj::LoadVAOs() // load individual objects to different V{A|B}O's to be able to manipulate 'em later
{
    // DUMMY
    m_render.CreateVBO_2Triangles();

    // i) Load Scene VAOs
    m_scene.Load();
    m_render.m_Scene = &m_scene;
    m_render.CreateVBO_Scene(); // in render besser als in scene!

    // ii) Load VAOs depending on scene
    m_scenebuilder.p_render = &m_render;
    m_scenebuilder.p_scene = &m_scene;

	m_scenebuilder.LoadGuardrails(); // "align" to road
    
	m_scenebuilder.LoadCurbstones(); // "align" to road

    // iii) Load individual VAO's
    m_trafficsigns.vCount         = m_render.vCount;
    m_trafficsigns.vertexArray    = m_render.vertexArray;
    m_trafficsigns.positionBuffer = m_render.positionBuffer;
    m_trafficsigns.colorBuffer    = m_render.colorBuffer;
    m_trafficsigns.VBOindex       = VBO_TRAFFICSIGNS;
    m_trafficsigns.Add(/*1,*/  10.5,0.5,0.0,  1.0,0.0,0.0);

    // iv) Load VAOs for Moving objects
    LoadMoving();
    m_render.m_Moving[0] = &m_moving[0]; // <-- movement applied to vertexshader(offset) while drawing/rendering
    m_render.m_Moving[1] = &m_moving[1];

    return 0;
}

void proj::Proj::LoadTextures()
{
    m_render.Texture[TEX_ROADSURFACE] = loadBMP_custom("C:\\__glShoot_Files\\road_tex_256x256.bmp");
    m_render.Texture[TEX_TRAFFICSIGN] = loadBMP_custom("C:\\__glShoot_Files\\zeichen_triangle.bmp");
//    m_render.Texture[TEX_CAR] = loadDDS("D:\\projects\\VC\\glShoot_VAO_Texture_Improved\\uvmap.DDS");
//    m_render.Texture[TEX_CAR] = loadBMP_custom("D:\\projects\\VC\\glShoot_VAO_Texture_Fixed\\uvmap2.bmp");
    m_render.Texture[TEX_CAR] = loadBMP_custom("C:\\__glShoot_Files\\LowPoly_Car\\CBRedD.bmp");
}

int proj::Proj::DoIt()
{
    wglMakeCurrent(m_render.hDC,m_render.hRC); // ;-) now Tab-switching in MTS possible

// only f. fixed pipeline --> glTranslatef(0, 0, -2.9f);

// objects' movement --> has to be done in an own loop / !?thread!?
    if (!bPause)
    {
        m_moving[0].MoveTo(glm::vec3(3.0f,1.0f,0.0f)); // <-- 2do, kann ins Init()
//        m_moving[1].Move(glm::vec3(0.005f,0.0f,0.0f));
		m_moving[1].iTspeed++;
//		if (m_moving[1].iTspeed >= 50)
//		{
			m_moving[1].iTspeed = 0;
			if (m_moving[1].iT < m_scene.trajectory_len-2) m_moving[1].iT++; else m_moving[1].iT = 0;
			int iTx = m_moving[1].iT;
			glm::vec3 v3 = glm::vec3(m_scene.m_SceneLoader.m_c_Trajectory[iTx].s_Pos.rl_X,
									 m_scene.m_SceneLoader.m_c_Trajectory[iTx].s_Pos.rl_Y,
									 1.0f);
//			m_moving[1].MoveTo(v3);
			glm::vec3 vMove = v3-vTrajPosprev; // consider mouse controlled position change
			m_moving[1].Move(vMove);
			vTrajPosprev = v3;
/*
//	if (bCamStickToCar)
//	{
		glm::vec3 vVehDirNorm = glm::normalize(m_moving[1].direction);
		glm::float32 fZtmp = m_render.p_cam->Pos[2];
		m_render.p_cam->Pos = m_moving[1].position - vVehDirNorm*12.0f;
		m_render.p_cam->Pos[2] = fZtmp;
		m_render.p_cam->At = m_moving[1].position;
//	}
*///		}
    }

	m_render.DrawVAOs();          // Draw The Scene

    SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

    return 0;
}