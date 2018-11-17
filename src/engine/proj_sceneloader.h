// -----------------------
//
// Scene Loader
//
// loads a Lane-scene definition
// -----------------------

#pragma once

#include <assert.h>
#include <exception>    // std::exception
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

typedef signed long T_SLONG;
typedef unsigned long T_ULONG;
typedef unsigned int T_UINT32;
typedef unsigned int T_UWORD;
typedef unsigned char T_UBYTE;
typedef char T_CHAR;
//typedef double T_REAL; // Software, ...
typedef GLfloat T_REAL; // OpenGL + Glew  ;-)
typedef bool T_BOOL;

#ifdef LANE_GEN_EXPORTS
#define DLL __declspec(dllexport)
#else
#define DLL
#endif

struct DLL RGBcolor
{
  RGBcolor() : u_Red(0), u_Green(0), u_Blue(0)
  { }

  RGBcolor(T_UINT32 u_RedValue, T_UINT32 u_GreenValue, T_UINT32 u_BlueValue) : u_Red(u_RedValue),
    u_Green(u_GreenValue),
    u_Blue(u_BlueValue)
  { }

  inline bool operator == (const RGBcolor &color) const
  {
    return ( (color.u_Red==u_Red) && (color.u_Green==u_Green) && (color.u_Blue==u_Blue) );
  }

  inline bool operator != (const RGBcolor &color) const
  {
    return ( (color.u_Red!=u_Red) || (color.u_Green!=u_Green) || (color.u_Blue!=u_Blue) );
  }

  T_UINT32 u_Red;
  T_UINT32 u_Green;
  T_UINT32 u_Blue;
};

struct DLL S_Point3D
{
  T_REAL rl_X;
  T_REAL rl_Y;
  T_REAL rl_Z;

  T_REAL & operator[](T_SLONG sl_i)
  { 
    switch(sl_i)
    {
    case 0: return rl_X;
    case 1: return rl_Y;
    case 2: return rl_Z;
    default:
      assert( 0 && "never reach");
      throw std::exception( "never reach");
    }
  }

  const T_REAL & operator[](T_SLONG sl_i) const
  { 
    switch(sl_i)
    {
    case 0: return rl_X;
    case 1: return rl_Y;
    case 2: return rl_Z;
    default:
      assert( 0 && "never reach");
      throw std::exception( "never reach");
    }
  }

  S_Point3D & operator+=(const S_Point3D &rs_ToAdd) 
  {
    T_SLONG sl_i;
    for(sl_i = 0; sl_i < 3; sl_i++)
    {
      (*this)[sl_i] += rs_ToAdd[sl_i];
    }
    return *this;
  }

  S_Point3D & operator-=(const S_Point3D &rs_ToAdd) 
  {
    T_SLONG sl_i;
    for(sl_i = 0; sl_i < 3; sl_i++)
    {
      (*this)[sl_i] -= rs_ToAdd[sl_i];
    }
    return *this;
  }

  S_Point3D & operator*=(const T_REAL &rrl_a) 
  {
    T_SLONG sl_i;
    for(sl_i = 0; sl_i < 3; sl_i++)
    {
      (*this)[sl_i] *= rrl_a;
    }
    return *this;
  }
};

struct DLL S_MarkerPoint
{
  S_Point3D s_Left;
  S_Point3D s_Right;
  T_BOOL    b_Visible;

  T_SLONG sl_Print() const
  {
    std::cout << "MP = L[%5f, %5f, %5f], R[%5f, %5f, %5f], V%d\n", 
      s_Left[0], s_Left[1], s_Left[2], s_Right[0], s_Right[1], s_Right[2], b_Visible;
    return 0;
  }
};

struct S_6Dof
{
  S_Point3D s_Pos;
  T_REAL rl_Roll;
  T_REAL rl_Pitch;
  T_REAL rl_Yaw;
  T_REAL rl_Speed;

  T_REAL & operator[](T_SLONG sl_i)
  { 
    switch(sl_i)
    {
    case 0: 
    case 1: 
    case 2: return s_Pos[sl_i];
    case 3: return rl_Roll;
    case 4: return rl_Yaw;
    case 5: return rl_Pitch;
    case 6: return rl_Speed;
    default:
      assert( 0 && "never reach");
      throw std::exception( "never reach");
    }
  }

  const T_REAL & operator[](T_SLONG sl_i) const { 
    switch(sl_i)
    {
    case 0: 
    case 1: 
    case 2: return s_Pos[sl_i];
    case 3: return rl_Roll;
    case 4: return rl_Yaw;
    case 5: return rl_Pitch;
    case 6: return rl_Speed;
    default:
      assert( 0 && "never reach");
      throw std::exception( "never reach");
    }
  }
};

#define unchecktype(x) x // Hack! of hack

namespace proj
{
  class DLL SceneParam
  {
  public:
    SceneParam(void);
    virtual ~SceneParam(void);

    /* For all 6dof entries the DIN norm is used:
    x points to the front
    y points to the left
    z points up
    Phi:   Roll Angle (around x) [rad]
    Theta: Nick Angle (around y) [rad]
    Psi:   Gear Angle (around z) [rad]
    */

    // Camera external Parameters (relative to ego)
    T_REAL m_rl_CamX;
    T_REAL m_rl_CamY;
    T_REAL m_rl_CamZ;
    T_REAL m_rl_CamPhi;
    T_REAL m_rl_CamTheta;
    T_REAL m_rl_CamPsi;

    // Camera internal Parameters; we use the most simple camera model ...
    T_REAL m_rl_FocalLength;
    T_ULONG m_ul_Width;
    T_ULONG m_ul_Height;
    T_ULONG m_ul_BitsPerPixel;
    T_REAL m_rl_k1;
    T_REAL m_rl_MainPointX, m_rl_MainPointY;

    T_REAL m_rl_NoiseLevel;
    T_REAL m_rl_NoiseProbability;

    T_REAL m_rl_StepT;
    T_REAL m_rl_StepTrajectoryT;

    T_REAL m_rl_EgoCOGZ; // Center of gravity of vehicle, z
    T_REAL m_rl_EgoRollOffset;
    T_REAL m_rl_BackClipPlane;

    T_SLONG m_sl_ObstacleStartLine, m_sl_ObstacleStopLine;
    RGBcolor m_ObstacleColor;
    T_UWORD m_uw_ObstacleAlpha; // 0 -> opaque, 256 -> transparent

    T_SLONG m_sl_AntiAliasing;
    T_SLONG m_sl_DrawHorizon;
    T_SLONG m_b_ComputeGroundTruth;

    T_REAL m_rl_DepthBlending;
    T_REAL m_rl_Axis2Axis;

    // Lane Parameters
    std::vector<S_6Dof> m_c_Trajectory;

    std::vector<std::vector<S_MarkerPoint> > m_c_Markers;

    std::vector<RGBcolor> m_c_Colors;

    std::vector<T_UWORD> m_TextureIDs;

    RGBcolor m_RoadColor;

    // The speed in x-Direction

    T_SLONG m_sl_NrOfRuns;
  };

  class SceneLoader: public proj::SceneParam
  {
    static const unchecktype(T_SLONG) sl_NOERROR = 0;
    static const unchecktype(T_SLONG) sl_ERROR_PARAM_READ = 1;
    static const unchecktype(T_SLONG) sl_ERROR_PARSE = 2;
    static const unchecktype(T_SLONG) sl_ERROR_READ_SCENE = 3;
  public:
    SceneLoader();

    T_SLONG sl_ReadScene(const std::string &rc_Scenefile);
    T_SLONG sl_ReadParameters(const std::string &rc_Paramfile);
    T_SLONG sl_ReadParametersAndScene(const std::string &rc_Paramfile);
  private:
    void sl_ReadColor(T_CHAR *line);
    void sl_ReadTextureID(T_CHAR *line);
    void sl_ReadMarkerpoint(T_CHAR *line, S_MarkerPoint &c_MP);
    void sl_Read6_Dof(T_CHAR *line, S_6Dof &c_6Dof);

    void if_set_param(T_CHAR *line, std::string s_find, T_REAL &f_val);
    void if_set_param_UL(T_CHAR *line, std::string s_find, T_ULONG &ul_val);      
    void if_set_param_SL(T_CHAR *line, std::string s_find, T_SLONG &sl_val);
    void if_set_param_Str(T_CHAR *line, std::string s_find, std::string &c_val);
    void v_ApplySpeedFactor(T_REAL rl_SpeedFactor);
  protected:
    std::string c_Scene_filename;
  };
}

#undef DLL
