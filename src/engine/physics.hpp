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
    int collision_check(const std::vector <obj::CObject *> vobj, const glm::vec3 coord)
    {
      int i_obj_bump = -1;
      for (unsigned int i = 0; i < vobj.size(); i++)
      {
        if (
            // min_point ist der min-Vektor (x,y,z), daher muss man die einzelnen Koordinaten nochmal auf min, max checken
            (coord.x > glm::min(vobj[i]->aabb.min_point.x, vobj[i]->aabb.max_point.x))
         && (coord.x < glm::max(vobj[i]->aabb.min_point.x, vobj[i]->aabb.max_point.x))
         && (coord.y > glm::min(vobj[i]->aabb.min_point.y, vobj[i]->aabb.max_point.y))
         && (coord.y < glm::max(vobj[i]->aabb.min_point.y, vobj[i]->aabb.max_point.y))
          // no z-check
          )
          i_obj_bump = i;
      }
      return i_obj_bump;
    }

    typedef glm::vec3 CVec3;

    int collision_check_bbox(const std::vector <obj::CObject *> vobj, const glm::vec3 pos, const glm::vec3 at)
    {
      int i_obj_bump = -1;
      for (unsigned int i = 0; i < vobj.size(); i++)
      {
        glm::vec3 hit;
        bool bHit = CheckLineBox(vobj[i]->aabb.min_point, vobj[i]->aabb.max_point, pos, pos + glm::normalize(at-pos)*10.0f, hit);
        if (
         bHit
          )
          i_obj_bump = i;
//        i_obj_bump = vobj[i].vaoID;
      }
      return i_obj_bump;
    }
    
    // http://www.3dkingdoms.com/weekly/weekly.php?a=3
    int inline GetIntersection(float fDst1, float fDst2, CVec3 P1, CVec3 P2, CVec3 &Hit) {
      if ((fDst1 * fDst2) >= 0.0f) return 0;
      if (fDst1 == fDst2) return 0;
      Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
      return 1;
    }

    int inline InBox(CVec3 Hit, CVec3 B1, CVec3 B2, const int Axis) {
      if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
      if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
      if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
      return 0;
    }

    // returns true if line (L1, L2) intersects with the box (B1, B2)
    // returns intersection point in Hit
    int CheckLineBox(CVec3 B1, CVec3 B2, CVec3 L1, CVec3 L2, CVec3 &Hit)
    {
      if (L2.x < B1.x && L1.x < B1.x) return false;
      if (L2.x > B2.x && L1.x > B2.x) return false;
      if (L2.y < B1.y && L1.y < B1.y) return false;
      if (L2.y > B2.y && L1.y > B2.y) return false;
      if (L2.z < B1.z && L1.z < B1.z) return false;
      if (L2.z > B2.z && L1.z > B2.z) return false;
      if (L1.x > B1.x && L1.x < B2.x &&
        L1.y > B1.y && L1.y < B2.y &&
        L1.z > B1.z && L1.z < B2.z)
      {
        Hit = L1;
        return true;
      }
      if ((GetIntersection(L1.x - B1.x, L2.x - B1.x, L1, L2, Hit) && InBox(Hit, B1, B2, 1))
        || (GetIntersection(L1.y - B1.y, L2.y - B1.y, L1, L2, Hit) && InBox(Hit, B1, B2, 2))
        || (GetIntersection(L1.z - B1.z, L2.z - B1.z, L1, L2, Hit) && InBox(Hit, B1, B2, 3))
        || (GetIntersection(L1.x - B2.x, L2.x - B2.x, L1, L2, Hit) && InBox(Hit, B1, B2, 1))
        || (GetIntersection(L1.y - B2.y, L2.y - B2.y, L1, L2, Hit) && InBox(Hit, B1, B2, 2))
        || (GetIntersection(L1.z - B2.z, L2.z - B2.z, L1, L2, Hit) && InBox(Hit, B1, B2, 3)))
        return true;

      return false;
    }

  }; // class physics

}
