#pragma once

#include "types.h"
#include "proj_render.h"
#include "proj_m3uloader.hpp"
#include "proj_scenebuilder.hpp"

#include "obj_Trafficsign.hpp" // <-- move to proj (or even scene) later
#include "obj_Moving.hpp"
#include "obj.hpp"

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

    proj::Scene m_scene;

    obj::Trafficsign m_trafficsigns;

    proj::SceneBuilder m_scenebuilder; // <-- add to the scene: Guardrails / Curbstones

    // moving
    obj::Moving m_moving[2];

    std::vector <obj::CObject> vObjects;

    glm::vec3 vTrajPosprev; // <--- wird benutzt für ... bewegen des Fahrzeugs, s.proj.cpp->DoIt()

    proj::Render m_render;

    /* ===== FUNCTIONS ===== */

    int Init();

    int DoIt(); // call m_render to draw next frame

  private:
    /* ===== FUNCTIONS ===== */

    void LoadMoving();

    int Load_Objs_to_VBOs();
  };
}
