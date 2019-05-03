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
    unsigned int player_scene_pos(const proj::Scene scene, const glm::vec3 Cursor) // 2do: use pos instead of Cursor
    {
      float dist_min = 9999.9f;
      float dist;
      unsigned int iTrack_nearest = 0;
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

    void trajectory_id_to_line_marker(const proj::Scene scene, const int traj_id, unsigned int & iLine, unsigned int & iMark)
    {
//      unsigned int iLine;
//      unsigned int iMarker;
      unsigned int sz;
      unsigned int i = 0;

      if (traj_id == 0) { iLine = 0; iMark = 0; return; }

      sz = (unsigned int)scene.m_SceneLoader.m_c_Markers.size(); // number of marker vectors (lines)
      for (iLine = 0; iLine < sz; iLine++)  // das sollte nun segment heissen, es gibt ja keine Linien mehr in dem Sinne
      {                                     // die sind jetzt Teil der Textur
        
        const std::vector<S_MarkerPoint> &rc_Marker = scene.m_SceneLoader.m_c_Markers[iLine];
        if ((i + rc_Marker.size()) >= traj_id) break;
        i += rc_Marker.size();
      }
//      iMark = traj_id % i;
      iMark = traj_id - i;
    }


    // return 0 = no collision | otherwise index of object
    int collision_check(const std::vector <obj::CObject> vobj, const glm::vec3 Cursor) // 2do: use pos instead of Cursor
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
