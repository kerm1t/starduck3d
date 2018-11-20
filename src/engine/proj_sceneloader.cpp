#include "stdafx.h"
#include "proj_sceneloader.h"

proj::SceneParam::SceneParam()
{
}

proj::SceneParam::~SceneParam()
{
}

proj::SceneLoader::SceneLoader()
{
}

void proj::SceneLoader::sl_ReadColor(T_CHAR *line)
{
  T_CHAR tmp[32];
  T_CHAR tmp2[8];
  T_CHAR tmp3[8];
  T_CHAR tmp4[8];

  T_SLONG sl_R,sl_G,sl_B;

  sscanf(line, "%s %s %s %s = %s %s %s;", tmp,tmp,tmp,tmp,tmp2,tmp3,tmp4);
  sl_R = ::atoi(std::string(tmp2).c_str());
  sl_G = ::atoi(std::string(tmp3).c_str());
  sl_B = ::atoi(std::string(tmp4).c_str());
  *m_c_Colors.rbegin() = RGBcolor(sl_R, sl_G, sl_B); // Hack!
}

void proj::SceneLoader::sl_ReadTextureID(T_CHAR *line)
{
  T_CHAR tmp[32];
  T_CHAR tmp2[255];

  T_SLONG sl_TextureID;

  sscanf(line, "%s %s %s = %s;", tmp,tmp,tmp,tmp2);
  sl_TextureID = ::atoi(std::string(tmp2).c_str());
//  *m_TextureIDs.rbegin() = sl_TextureID; // Hack!
}

void proj::SceneLoader::sl_ReadTexture(T_CHAR *line)
{
  T_CHAR tmp[32];
  T_CHAR tmp2[255];

  std::string s_Texture;

  sscanf(line, "%s %s %s = %s;", tmp, tmp, tmp, tmp2);
  s_Texture = std::string(tmp2).c_str();
  *m_Textures.rbegin() = s_Texture; // reverse iterator
}

void proj::SceneLoader::sl_ReadMarkerpoint(T_CHAR *line, S_MarkerPoint &c_MP)
{
  T_CHAR tmp[64];
  T_CHAR tmp2[64];
  T_CHAR tmp3[64];
  T_CHAR tmp4[64];
  T_CHAR tmp5[64];
  T_CHAR tmp6[64];
  T_CHAR tmp7[64];

  sscanf(line, "{ %s , %s , %s , %s , %s , %s , %s }", tmp,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7);
  c_MP.s_Left.rl_X  = (T_REAL)::atof(std::string(tmp).c_str());
  c_MP.s_Left.rl_Y  = (T_REAL)::atof(std::string(tmp2).c_str());
  c_MP.s_Left.rl_Z  = (T_REAL)::atof(std::string(tmp3).c_str());
  c_MP.s_Right.rl_X = (T_REAL)::atof(std::string(tmp4).c_str());
  c_MP.s_Right.rl_Y = (T_REAL)::atof(std::string(tmp5).c_str());
  c_MP.s_Right.rl_Z = (T_REAL)::atof(std::string(tmp6).c_str());
  c_MP.b_Visible    = (T_REAL)::atof(std::string(tmp7).c_str()) > 0.5;
}

void proj::SceneLoader::sl_Read6_Dof(T_CHAR *line, S_6Dof &c_6Dof) // (e.g.) trajectory
{
  T_CHAR tmp[64];
  T_CHAR tmp2[64];
  T_CHAR tmp3[64];
  T_CHAR tmp4[64];
  T_CHAR tmp5[64];
  T_CHAR tmp6[64];
  T_CHAR tmp7[64];

  sscanf(line, "{ %s , %s , %s , %s , %s , %s , %s }", tmp,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7);
  c_6Dof.s_Pos.rl_X = (T_REAL)::atof(std::string(tmp).c_str());
  c_6Dof.s_Pos.rl_Y = (T_REAL)::atof(std::string(tmp2).c_str());
  c_6Dof.s_Pos.rl_Z = (T_REAL)::atof(std::string(tmp3).c_str());
  c_6Dof.rl_Roll    = (T_REAL)::atof(std::string(tmp4).c_str());
  c_6Dof.rl_Pitch   = (T_REAL)::atof(std::string(tmp5).c_str());
  c_6Dof.rl_Yaw     = (T_REAL)::atof(std::string(tmp6).c_str());
  c_6Dof.rl_Speed   = (T_REAL)::atof(std::string(tmp7).c_str());
}

T_SLONG proj::SceneLoader::sl_ReadScene(const std::string &rc_Scenefile)
{
  T_SLONG sl_Ret = sl_NOERROR;
  std::stringstream err;

  T_CHAR line[1024];
  T_CHAR tmp[308];
  T_CHAR tmp2[308];
  T_CHAR tmp3[308];

  S_MarkerPoint c_MP;
  S_6Dof c_6Dof;

  FILE *c_F;

  fopen_s(&c_F, rc_Scenefile.c_str(), "r");
  if (!c_F)
  {
    sl_Ret = sl_ERROR_READ_SCENE;
    std::cout << "scene-file %s cannot be loaded!\n", rc_Scenefile.c_str(); 
    goto ERROR_EXIT;
  }

  m_c_Colors.clear();
  m_c_Markers.clear();
  m_c_Trajectory.clear();
  m_Textures.clear();

  while (fgets(line, sizeof line, c_F))
  {
    sscanf(line, "%s %s %s", tmp, tmp2, tmp3);
    if (std::string(tmp) != "static")
    {
      sl_Ret = sl_ERROR_PARSE;
      goto ERROR_EXIT;
    }
    if (std::string(tmp2) == "unsigned")
    {
      sl_ReadColor(line);
    }
    else if (std::string(tmp2) == "uword")
    {
      sl_ReadTextureID(line);
    }
    else if (std::string(tmp2) == "string")
    {
      sl_ReadTexture(line);
    }
    else if (std::string(tmp2) == "S_MarkerPoint") // read all lines of markerpoint
    {
      // push back now and later set it with rbegin() ... hmmm
      m_c_Markers.push_back(std::vector<S_MarkerPoint>()); // new Marker-vector, can be multiple
      m_c_Colors.push_back(RGBcolor(1450, 1700, 1200)); // init colors, can be changed in subsequent line
//      m_TextureIDs.push_back(0); // init textureIDs, can be changed in subsequent line
      m_Textures.push_back("");
      fgets(line, sizeof line, c_F);
      while (*line != '}')
      {
        sl_ReadMarkerpoint(line,c_MP);
        m_c_Markers.rbegin()->push_back(c_MP);
        fgets(line, sizeof line, c_F);
      }
    }
    else if (std::string(tmp2) == "S_6Dof")
    {
//      m_c_Trajectory.clear();
      fgets(line, sizeof line, c_F);
      while (*line != '}')
      {
        sl_Read6_Dof(line,c_6Dof);
        m_c_Trajectory.push_back(c_6Dof);
        fgets(line, sizeof line, c_F);
      }
    }
    else
    {
      sl_Ret = sl_ERROR_PARSE;
      goto ERROR_EXIT;
    }
  }
  fclose(c_F);

  if (m_c_Trajectory.size() == 0)
  {
    std::cout << "scene-file %s cannot be loaded!\n", rc_Scenefile.c_str(); 
    sl_Ret = sl_ERROR_PARSE;
    goto ERROR_EXIT;
  }

  //    elapsedTime = timer.getElapsedTimeInSec();  // 21-23   sec
  //    13.7 sec mit neuem Read2dVector
  //     6.2 sec m. Read1DVector
  return sl_NOERROR;
ERROR_EXIT:
  if (c_F) fclose(c_F);
  //    REPORT_ERROR(sl_Ret, m_ach_ModuleName);
  return sl_Ret;
}

void proj::SceneLoader::if_set_param(T_CHAR *line, std::string s_find, T_REAL &f_val)
{
  T_CHAR key[1024];
  T_CHAR val[1024];
  sscanf(line,"%s",key);
  if (std::string(key) == s_find)
  {
    sscanf(line,"%s %*s %s",key,val);
    f_val = (T_REAL)::atof(std::string(val).c_str());
  }
}

void proj::SceneLoader::if_set_param_UL(T_CHAR *line, std::string s_find, T_ULONG &ul_val)
{
  T_CHAR key[1024];
  T_CHAR val[1024];
  sscanf(line,"%s",key);
  if (std::string(key) == s_find)
  {
    sscanf(line,"%s %*s %s",key,val);
    ul_val = ::atoi(std::string(val).c_str());
  }
}

void proj::SceneLoader::if_set_param_SL(T_CHAR *line, std::string s_find, T_SLONG &sl_val)
{
  T_CHAR key[1024];
  T_CHAR val[1024];
  sscanf(line,"%s",key);
  if (std::string(key) == s_find)
  {
    sscanf(line,"%s %*s %s",key,val);
    sl_val = ::atoi(std::string(val).c_str());
  }
}

void proj::SceneLoader::if_set_param_Str(T_CHAR *line, std::string s_find, std::string &c_val)
{
  T_CHAR key[1024];
  T_CHAR val[1024];
  sscanf(line,"%s",key);
  if (std::string(key) == s_find)
  {
    sscanf(line,"%s %*s %s",key,val);
    c_val = std::string(val);
  }
}

T_SLONG proj::SceneLoader::sl_ReadParameters(const std::string &rc_Paramfile)
{
  T_SLONG sl_Ret = sl_NOERROR;

  T_CHAR line[1024];
  T_CHAR tmp[1024];
  T_REAL rl_SpeedFactor = 1.0;

  FILE *c_F;
  fopen_s(&c_F, rc_Paramfile.c_str(), "r");
  if (!c_F)
  {
    sl_Ret = sl_ERROR_READ_SCENE;
    std::cout << "cfg-file %s cannot be loaded!\n", rc_Paramfile.c_str(); 
    goto ERROR_EXIT;
  }

  while (fgets(line, sizeof line, c_F))
  {
    sscanf(line, "%s", tmp);
    if (std::string(tmp) == "<LANEGEN>")
    {
      while (fgets(line, sizeof line, c_F))
      {
        sscanf(line, "%s", tmp);
        if (std::string(tmp) == "<\\LANEGEN>") break;
        if (*line == ';') continue; // ignore comment line

        if_set_param_UL(line, "ImageWidth", m_ul_Width);
        if_set_param_UL(line, "ImageHeight", m_ul_Height);
        if_set_param_UL(line, "ImageBitsPerPixel", m_ul_BitsPerPixel);
        m_ul_BitsPerPixel = 12; //// default value

        if_set_param(line, "CamX", m_rl_CamX);
        if_set_param(line, "CamY", m_rl_CamY);
        if_set_param(line, "CamZ", m_rl_CamZ);
        if_set_param(line, "CamPhi", m_rl_CamPhi);     // Roll
        if_set_param(line, "CamTheta", m_rl_CamTheta); // Pitch
        if_set_param(line, "CamPsi", m_rl_CamPsi);     // Yaw

        if_set_param(line, "FocalLength", m_rl_FocalLength);
        if_set_param(line, "DistortionK1", m_rl_k1);
        if_set_param(line, "MainPointX", m_rl_MainPointX);
        if_set_param(line, "MainPointY", m_rl_MainPointY);

        if_set_param(line, "NoiseLevel", m_rl_NoiseLevel);
        if_set_param(line, "NoiseProbability", m_rl_NoiseProbability);

        if_set_param(line, "EgoCOGZ", m_rl_EgoCOGZ);
        if_set_param(line, "EgoRollOffset", m_rl_EgoRollOffset);
        if_set_param(line, "StepT", m_rl_StepT);
        if_set_param(line, "StepTrajectoryT", m_rl_StepTrajectoryT);
        /*
        if_set_param(line, "Obstacle", asl_Obstacle, 2);
        m_sl_ObstacleStartLine = asl_Obstacle[0];
        m_sl_ObstacleStopLine = asl_Obstacle[1];

        T_UINT16 a_ObstacleRGBcolor[3];
        if_set_param(line, "ObstacleColor", a_ObstacleRGBcolor, 3))
        {
        a_ObstacleRGBcolor[0] = 1450;
        a_ObstacleRGBcolor[1] = 1700;
        a_ObstacleRGBcolor[2] = 1200;
        }
        if_set_param(line, "ObstacleAlpha", m_uw_ObstacleAlpha);
        */
        if_set_param(line, "DepthBlending", m_rl_DepthBlending);
        if_set_param_SL(line, "AntiAliasing", m_sl_AntiAliasing);
        ////////                samples = m_sl_AntiAliasing; // AA-Multisample-FBO

        if_set_param_SL(line, "NrOfRuns", m_sl_NrOfRuns);
        //                if_set_param_Str(line, "SceneFile", c_Scenefile);
        //                if_set_param(line, "ComputeGroundTruth", m_b_ComputeGroundTruth);
        if_set_param_SL(line, "DrawHorizon", m_sl_DrawHorizon);
        if_set_param(line, "SpeedFactor", rl_SpeedFactor);
        v_ApplySpeedFactor(rl_SpeedFactor);

        if_set_param(line, "Axis2Axis", m_rl_Axis2Axis);
        if_set_param(line, "BackClipPlane", m_rl_BackClipPlane);
      }
    }
  }
  fclose(c_F);
  return sl_NOERROR;
ERROR_EXIT:
  if (c_F) fclose(c_F);
  //    REPORT_ERROR(sl_Ret, m_ach_ModuleName);
  return sl_Ret;
}

T_SLONG proj::SceneLoader::sl_ReadParametersAndScene(const std::string &rc_Paramfile)
{
  sl_ReadParameters(rc_Paramfile);
  sl_ReadScene(c_Scene_filename);
  return 0;
}

void proj::SceneLoader::v_ApplySpeedFactor(T_REAL rl_SpeedFactor)
{
  T_SLONG sl_i;
  for( sl_i = 0; sl_i < (T_SLONG)m_c_Trajectory.size(); sl_i++)
  {
    m_c_Trajectory[sl_i].rl_Speed *= rl_SpeedFactor;
  }
}