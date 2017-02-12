// -----------------------
//
// Scene Builder
//
// add: guardrails, curbstones, ...
// -----------------------

#include "stdafx.h"
#pragma once

#include "obj_Guardrail.hpp" // <-- remove
#include "obj_Curbstone.hpp" // <-- remove

namespace proj
{
    class SceneBuilder
    {
    public:
        proj::Render * p_render;
        proj::Scene * p_scene;

        obj::Trafficsign m_trafficsigns;
        obj::Guardrail m_guardrail;
        obj::Curbstone m_curbstone;
//        m_scene
//        m_guardrail

        void LoadGuardrails() // ... align to road-border
        {
            SceneParam &rc_Param = p_scene->m_SceneLoader;

            m_guardrail.vCount         = p_render->vCount;
            m_guardrail.vertexArray    = p_render->vertexArray;
            m_guardrail.positionBuffer = p_render->positionBuffer;
            m_guardrail.colorBuffer    = p_render->colorBuffer;
            m_guardrail.VBOindex = VBO_GUARDRAIL;
        //    m_guardrail.Init(1);

        //    int iV = 0;
        //    unsigned int sz = (unsigned int)rc_Param.m_c_Markers.size(); // number of marker vectors (lines)
        //    for (int iLine=0;iLine<sz;iLine++)
        //    {
            if (rc_Param.m_c_Markers.size() < 3) return;
            int iLine = 2; // = Road
                const std::vector<S_MarkerPoint> &rc_Marker = rc_Param.m_c_Markers[iLine];

                m_guardrail.Init(rc_Marker.size());

                float fMeters = 0.0f;
                for (unsigned int iMarker=0;iMarker<((unsigned int)rc_Marker.size()-1);iMarker++) // no. of markersteps (typically > 500)
                {
                    if (rc_Marker[iMarker].b_Visible)
                    {
                        S_Point3D p0 = rc_Marker[iMarker].s_Left;
                        S_Point3D p3 = rc_Marker[iMarker+1].s_Left;
                        Vec3f v0;//,v3,n3;
                        v0.x = (float)(p3.rl_X-p0.rl_X);
                        v0.y = (float)(p3.rl_Y-p0.rl_Y);
                        v0.z = (float)(p3.rl_Z-p0.rl_Z);
                        fMeters += Length(v0);

                        m_guardrail.iNdx = iMarker*3*3*4; // <-- iTriPerObj
                        m_guardrail.Add(/*1,*/  (float)p0.rl_X,(float)p0.rl_Y,(float)p0.rl_Z,  (float)p3.rl_X,(float)p3.rl_Y,(float)p3.rl_Z);
                    }
                }
        //    }

        //    m_guardrail.Add(1,  8.5,-0.5,0.0,  0.0,1.0,0.0);
            m_guardrail.Fini();
        }

        void LoadCurbstones() // ... align to road border
        {
            SceneParam &rc_Param = p_scene->m_SceneLoader;

            m_curbstone.vCount         = p_render->vCount;
            m_curbstone.vertexArray    = p_render->vertexArray;
            m_curbstone.positionBuffer = p_render->positionBuffer;
            m_curbstone.colorBuffer    = p_render->colorBuffer;
            m_curbstone.VBOindex = VBO_CURBSTONES;
        //    m_guardrail.Init(1);

        //    int iV = 0;
        //    unsigned int sz = (unsigned int)rc_Param.m_c_Markers.size(); // number of marker vectors (lines)
        //    for (int iLine=0;iLine<sz;iLine++)
        //    {
            if (rc_Param.m_c_Markers.size() < 3) return;
            int iLine = 2; // = Road
                const std::vector<S_MarkerPoint> &rc_Marker = rc_Param.m_c_Markers[iLine];
        
                m_curbstone.Init(rc_Marker.size()*2); // 2 = L+R

                int iObj=0;
                float fMeters = 0.0f;
				int iCurbColor=0;
// right side
				for (unsigned int iMarker=0;iMarker<rc_Marker.size()-1;iMarker++) // no. of markersteps (typically > 500)
                {
                    if (rc_Marker[iMarker].b_Visible)
                    {
                        S_Point3D p0 = rc_Marker[iMarker].s_Left;
                        S_Point3D p3 = rc_Marker[iMarker+1].s_Left;
                        Vec3f v0;//,v3,n3;
                        v0.x = (float)(p3.rl_X-p0.rl_X);
                        v0.y = (float)(p3.rl_Y-p0.rl_Y);
                        v0.z = (float)(p3.rl_Z-p0.rl_Z);
                        fMeters += Length(v0);
                
                        Vec3f col = Vec3f(0.6f,0.6f,0.6f); // grey
                
                        if (iCurbColor%2==0) col = Vec3f(0.9f,0.0f,0.0f);
                        else col = Vec3f(0.9f,0.9f,0.9f);

        //                m_curbstone.iNdx = iMarker*3*3*4; // <-- iTriPerObj
                        m_curbstone.iNdx = iObj++*3*3*4; // <-- iTriPerObj
                        m_curbstone.Add(/*1,*/  (float)p0.rl_X,(float)p0.rl_Y,(float)p0.rl_Z,  (float)p3.rl_X,(float)p3.rl_Y,(float)p3.rl_Z,  col);
						iCurbColor++;
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
                        Vec3f v0;//,v3,n3;
                        v0.x = (float)(p3.rl_X-p0.rl_X);
                        v0.y = (float)(p3.rl_Y-p0.rl_Y);
                        v0.z = (float)(p3.rl_Z-p0.rl_Z);
                        fMeters += Length(v0);
                
                        Vec3f col = Vec3f(0.6f,0.6f,0.6f); // grey
                
                        if (iMarker%2==0) col = Vec3f(0.9f,0.0f,0.0f);
                        else col = Vec3f(0.9f,0.9f,0.9f);

                        m_curbstone.iNdx = iObj++*3*3*4; // <-- iTriPerObj
                        m_curbstone.Add(/*1,*/  (float)p0.rl_X,(float)p0.rl_Y,(float)p0.rl_Z,  (float)p3.rl_X,(float)p3.rl_Y,(float)p3.rl_Z,  col);
                    }
                }

            m_curbstone.Fini();
        } // void LoadCurbStones
    }; // class
}
