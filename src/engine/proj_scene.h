#pragma once

#include "proj_sceneloader.h"

namespace proj
{
  class Scene
  {
  public:
    int trajectory_len;

    Scene();
    int Load();

    std::string c_Scene;
    std::string c_Cfg;

    proj::SceneLoader m_SceneLoader;
  private:
  };
}
