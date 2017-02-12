#include "stdafx.h"
#include "proj_scene.h"

proj::Scene::Scene()
{
}

int proj::Scene::Load()
{
  m_SceneLoader.sl_ReadScene(c_Scene);
  m_SceneLoader.sl_ReadParameters(c_Cfg);

  trajectory_len = m_SceneLoader.m_c_Trajectory.size();

  return 0;
}
