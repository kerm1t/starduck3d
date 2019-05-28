#pragma once


#include "obj.hpp"
#include "obj_Trafficsign.hpp" // <-- move to proj (or even scene) later
#include "obj_Moving.hpp"
#include "obj_Groundplane.hpp"
#include "obj_billboard.hpp"
#include "obj_simple.hpp"


#include "proj_render.h"
#include "proj_m3uloader.hpp"
#include "proj_scenebuilder.hpp"

#include "physics.hpp"


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

    obj::CCube m_cube;

    proj::Scene m_scene;

    obj::CTrafficsign m_trafficsigns;
// wo ist guardrail etc. ??

    proj::CSceneBuilder m_scenebuilder; // <-- add to the scene: Guardrails / Curbstones

    // moving
//    obj::Moving m_moving[2];
//    obj::CObject car[2];

//#define N_HOLZSTAPEL (40+20) // temp: 20 can be set later by Mouseklick
//    obj::CGL_ObjectWavefront holzstapel[N_HOLZSTAPEL];
//    int n_holz_gestapelt;

    std::vector <obj::CObject *> vObjects; // vector of references not possible

    glm::vec3 vTrajPosprev; // <--- wird benutzt für ... bewegen des Fahrzeugs, s.proj.cpp->DoIt()

    proj::Render m_render;

    proj::physics m_phys;

    int hit_object_id;
    int touch_object_id;

    /* ===== FUNCTIONS ===== */

    int Init();
    void Exit();

    int DoIt(); // call m_render to draw next frame

    void draw_ImGui();

  private:
    /* ===== FUNCTIONS ===== */

//    void LoadMoving();
///    int PlayerPos();

    int Load_Scene_Objs();

    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    glm::vec3 Mouse2Dto3D(int x, int y); // 2do: move to editor.hpp

    int Load_Objs_to_VBOs();
  };
}
