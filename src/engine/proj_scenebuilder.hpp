// -----------------------
//
// Scene Builder
//
// add: guardrails, curbstones, ... to a Lane-scene definition
// -----------------------

#include "stdafx.h"
#pragma once

//#define GLM_FORCE_RADIANS // 2do, replace all deg. by rad.!!
#include "glm.hpp"

#include "obj_Guardrail.hpp" // <-- remove
#include "obj_Curbstone.hpp" // <-- remove
#include "obj_Trafficsign.hpp" // <-- remove
#include "obj_Tunnel.hpp" // <-- remove

namespace proj
{
  class SceneBuilder
  {
  public:
    proj::Render * p_render;
    proj::Scene * p_scene;

    obj::Trafficsign m_trafficsigns;
    obj::Guardrail   m_guardrail;
    obj::Curbstone   m_curbstone;
    obj::Tunnel      m_tunnel;

    SceneBuilder()
    {
    };

    proj::c_VAO CreateGuardrails() // ... align to road-border
    {
      SceneParam &rc_Param = p_scene->m_SceneLoader;

      int iLine = 2; // = Road
      const std::vector<S_MarkerPoint> &rc_Marker = rc_Param.m_c_Markers[iLine];

      m_guardrail.p_render = p_render;
      m_guardrail.Init(rc_Marker.size() * 2);

      int iObj=0;
      for (unsigned int iMarker=0;iMarker<((unsigned int)rc_Marker.size()-1);iMarker++) // no. of markersteps (typically > 500)
      {
        if (rc_Marker[iMarker].b_Visible)
        {
          S_Point3D p0 = rc_Marker[iMarker].s_Left;
          S_Point3D p3 = rc_Marker[iMarker+1].s_Left;

          Vec3f col = Vec3f(0.6f,0.6f,0.6f);

          m_guardrail.nBufferStart = iObj++*3*3*4; // <-- iTriPerObj
          m_guardrail.Add(/*1,*/  glm::vec3((float)p0.rl_X,(float)p0.rl_Y,(float)p0.rl_Z),
                                  glm::vec3((float)p3.rl_X,(float)p3.rl_Y,(float)p3.rl_Z), col);
        }
      }

      for (unsigned int iMarker=0;iMarker<((unsigned int)rc_Marker.size()-1);iMarker++) // no. of markersteps (typically > 500)
      {
        if (rc_Marker[iMarker].b_Visible)
        {
          S_Point3D p0 = rc_Marker[iMarker].s_Right;
          S_Point3D p3 = rc_Marker[iMarker+1].s_Right;

          Vec3f col = Vec3f(0.6f,0.6f,0.6f);

          m_guardrail.nBufferStart = iObj++*3*3*4; // <-- iTriPerObj
          m_guardrail.Add(/*1,*/ glm::vec3((float)p0.rl_X,(float)p0.rl_Y,(float)p0.rl_Z),
                                 glm::vec3((float)p3.rl_X,(float)p3.rl_Y,(float)p3.rl_Z), col);
        }
      }

      m_guardrail._ToVBO();
      return m_guardrail.VAO();
    }


    proj::c_VAO CreateCurbstones() // ... align to road border
    {
      SceneParam &rc_Param = p_scene->m_SceneLoader;

      int iLine = 2; // = Road
      const std::vector<S_MarkerPoint> &rc_Marker = rc_Param.m_c_Markers[iLine];

      m_curbstone.p_render = p_render;
      m_curbstone.Init(rc_Marker.size() * 2); // 2 = L+R

      int iObj=0;
      // right side
      for (unsigned int iMarker=0;iMarker<rc_Marker.size()-1;iMarker++) // no. of markersteps (typically > 500)
      {
        if (rc_Marker[iMarker].b_Visible)
        {
          S_Point3D p0 = rc_Marker[iMarker].s_Left;
          S_Point3D p3 = rc_Marker[iMarker+1].s_Left;

          Vec3f col;
          if (iMarker%2==0) col = Vec3f(0.9f,0.0f,0.0f);
          else              col = Vec3f(0.9f,0.9f,0.9f);

          //                m_curbstone.iNdx = iMarker*3*3*4; // <-- iTriPerObj
          m_curbstone.nBufferStart = iObj++*3*3*4; // <-- iTriPerObj
          m_curbstone.Add(/*1,*/ glm::vec3((float)p0.rl_X,(float)p0.rl_Y,(float)p0.rl_Z),
                                 glm::vec3((float)p3.rl_X,(float)p3.rl_Y,(float)p3.rl_Z), col);
        }
      }
      //    }
      // left side
      for (unsigned int iMarker=0;iMarker<((unsigned int)rc_Marker.size()-1);iMarker++) // no. of markersteps (typically > 500)
      {
        if (rc_Marker[iMarker].b_Visible)
        {
          S_Point3D p3 = rc_Marker[iMarker].s_Right;  // right: vice-versa, so that inner side is convex
          S_Point3D p0 = rc_Marker[iMarker+1].s_Right;

          Vec3f col;
          if (iMarker%2==0) col = Vec3f(0.9f,0.0f,0.0f);
          else              col = Vec3f(0.9f,0.9f,0.9f);

          m_curbstone.nBufferStart = iObj++*3*3*4; // <-- iTriPerObj
          m_curbstone.Add(/*1,*/ glm::vec3((float)p0.rl_X,(float)p0.rl_Y,(float)p0.rl_Z),
                                 glm::vec3((float)p3.rl_X,(float)p3.rl_Y,(float)p3.rl_Z), col);
        }
      }

      m_curbstone.ToVBO();
      return m_curbstone.VAO();
    }


    proj::c_VAO CreateTunnel() // ... align to road border
    {
      SceneParam &rc_Param = p_scene->m_SceneLoader;

      int iLine = 2; // = Road
      const std::vector<S_MarkerPoint> &rc_Marker = rc_Param.m_c_Markers[iLine];

      m_tunnel.p_render = p_render;
      m_tunnel.Init(rc_Marker.size()); // Achtung --> zu viele Elemente initialisiert!!

      int iObj=0;
      for (unsigned int iMarker=50;iMarker<75;iMarker++) // no. of markersteps (typically > 500)
      {
        if (rc_Marker[iMarker].b_Visible)
        {
          S_Point3D p0 = rc_Marker[iMarker].s_Left;
          S_Point3D p3 = rc_Marker[iMarker+1].s_Left;
          S_Point3D p1 = rc_Marker[iMarker].s_Right;  // right: vice-versa, so that inner side is convex
          S_Point3D p2 = rc_Marker[iMarker+1].s_Right;

          Vec3f col;
          if (iMarker%2==0) col = Vec3f(0.85f,0.85f,0.85f);
          else              col = Vec3f(0.9f,0.9f,0.9f);

          m_tunnel.nBufferStart = iObj++*3*3*6; // <-- iTriPerObj
          m_tunnel.Add(glm::vec3((float)p0.rl_X,(float)p0.rl_Y,(float)p0.rl_Z+0.25f),
                       glm::vec3((float)p3.rl_X,(float)p3.rl_Y,(float)p3.rl_Z+0.25f),
                       glm::vec3((float)p1.rl_X,(float)p1.rl_Y,(float)p1.rl_Z+0.25f),
                       glm::vec3((float)p2.rl_X,(float)p2.rl_Y,(float)p2.rl_Z+0.25f), col);
        }
      }

      m_tunnel._ToVBO();
      return m_tunnel.VAO();
    }



  }; // class
} // namespace