#pragma once

#include "proj_render.h"
//#include "proj_scene.h"
#include "proj_m3uloader.hpp"

#include "obj_Trafficsign.hpp"    // <-- move to proj (or even scene) later
#include "proj_scenebuilder.hpp"
#include "obj_Moving.hpp"

namespace proj
{
    class Proj
    {
    public:
        bool bPause;

        Proj();
        void LoadMoving();
        int LoadVAOs();

		void LoadTextures();
        
		int Init();
        int DoIt(); // call m_render to draw next frame

        proj::Render m_render;

        proj::M3uLoader m_m3u;
        
        proj::Scene m_scene;
        
        obj::Trafficsign m_trafficsigns;

        proj::SceneBuilder m_scenebuilder; // <-- Guardrail / Curbstone
        
        // moving
        obj::Moving m_moving[2];
		glm::vec3 vTrajPosprev; // <--- wird benutzt für ... bewegen des Fahrzeugs, s.proj.cpp->DoIt()

    private:
    };
}
