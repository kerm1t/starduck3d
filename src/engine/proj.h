#pragma once


#include "obj.hpp"
#include "obj_Trafficsign.hpp" // <-- move to proj (or even scene) later
#include "obj_Moving.hpp"
#include "obj_Groundplane.hpp"


#include "proj_render.h"
#include "proj_m3uloader.hpp"
#include "proj_scenebuilder.hpp"

namespace proj
{
  class Proj
  {
  public:
    bool bPause;

    /* ===== Constructor ===== */

    Proj();

    /* ===== Variables ===== */

    proj::M3uLoader m_m3u; // to load a series of scenes

    obj::CGroundplane m_groundplane;

    proj::Scene m_scene;

    obj::CTrafficsign m_trafficsigns;
// wo ist guardrail etc. ??

    proj::CSceneBuilder m_scenebuilder; // <-- add to the scene: Guardrails / Curbstones

    // moving
    obj::Moving m_moving[2];
//    obj::CObject car[2];

    std::vector <obj::CObject> vObjects;

    glm::vec3 vTrajPosprev; // <--- wird benutzt f�r ... bewegen des Fahrzeugs, s.proj.cpp->DoIt()

    proj::Render m_render;

    /* ===== FUNCTIONS ===== */

    int Init();

    int DoIt(); // call m_render to draw next frame

  private:
    /* ===== FUNCTIONS ===== */

//    void LoadMoving();

    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    glm::vec3 Mouse2Dto3D(int x, int y); // 2do: move to editor.hpp

    int Load_Objs_to_VBOs();
  };
}
