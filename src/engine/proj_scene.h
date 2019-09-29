#pragma once

#include "proj_sceneloader.h"

namespace proj
{
  class Scene
  {
  public:
    // load this .scene file and 
    std::string c_Scene;
    std::string c_Cfg;

    // 2do: put the trajectory here, no encapsuled in sceneloader as of now
    int trajectory_len;
    
    // 2do: put the scene here

    proj::SceneLoader m_SceneLoader;

    Scene();
    int Load();
    // 2do: put the scene_to VBO here, currently in proj_render.cpp

  private:
  };
}
