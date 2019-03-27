#include "stdafx.h"
#pragma once
/*
2do:
- mark track-part ego is over
- collision check of ego vs. all objects (moving, static)

*/

#include "glm/glm.hpp"

#include "proj_scene.h"
namespace proj
{
  class physics
  {
  public:

    // not allowed to have just a function in h class, needs to be inside a class!
    int player_scene_pos(proj::Scene scene, glm::vec3 Cursor)
    {
      float dist_min = 9999.9f;
      float dist;
      int iTrack_nearest = -1;
      for (int i = 0; i < scene.trajectory_len; i++)
      {
        glm::vec3 v3 = glm::vec3(scene.m_SceneLoader.m_c_Trajectory[i].s_Pos.rl_X,
          scene.m_SceneLoader.m_c_Trajectory[i].s_Pos.rl_Y,
          0.0f);
        dist = glm::length(v3 - Cursor);
        if (dist < dist_min)
        {
          dist_min = dist;
          iTrack_nearest = i;
        }
      }
      return iTrack_nearest;
    } // PlayerPos

    // return 0 = no collision | otherwise index of object
    int collision_check(std::vector <obj::CObject> vobj, glm::vec3 Cursor)
    {
      int i_obj_bump = -1;
      for (unsigned int i = 0; i < vobj.size(); i++)
      {
        if (
            (Cursor.x > vobj[i].aabb.min_point.x) && (Cursor.x < vobj[i].aabb.max_point.x)
         && (Cursor.y > vobj[i].aabb.min_point.y) && (Cursor.y < vobj[i].aabb.max_point.y)
//         && (Cursor.z > vobj[i].aabb.min_point.z) && (Cursor.z < vobj[i].aabb.max_point.z)
          )
          i_obj_bump = i;
      }
      return i_obj_bump;
    }

  }; // class physics

}
