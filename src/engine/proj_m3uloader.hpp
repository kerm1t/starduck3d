#pragma once

#include <string>

namespace proj
{
  class M3uLoader
  {
    static const unchecktype(T_SLONG) sl_NOERROR = 0;
    static const unchecktype(T_SLONG) sl_ERROR_READ_M3U = 1;

    int iCfg;
  public:
    std::string c_dll_path;
    std::string c_m3ufile;
    std::string c_m3u_content_relpath;
    std::string c_m3u_cfg; // common config can be defined

    std::vector<std::string> m_v_cfglist;

    M3uLoader()
    {
      char buffer[MAX_PATH];
//      GetModuleFileName((HINSTANCE)&__ImageBase, buffer, sizeof(buffer));
      c_dll_path = buffer;
      c_dll_path = c_dll_path.substr(0, c_dll_path.find_last_of('\\') + 1);

      iCfg = 0;
    }

    T_SLONG Load()
    {
      std::string c_m3u_filepath = c_dll_path;
      c_m3u_filepath.append(c_m3ufile);

      FILE *c_F;
      T_SLONG sl_Ret = sl_NOERROR;
      T_CHAR line[1024];

      fopen_s(&c_F, c_m3u_filepath.c_str(), "r");
      if (!c_F)
      {
        sl_Ret = sl_ERROR_READ_M3U;
        std::cout << "m3u-file %s cannot be loaded!\n", c_m3ufile.c_str();
        goto ERROR_EXIT;
      }

      while (fgets(line, sizeof line, c_F))
      {
        T_CHAR tmp[1024];
        sscanf_s(line, "%s", tmp, sizeof(tmp));
        //                sscanf(line, "%s:%d", tmp); "filename:repetitions"
        std::string c_Line = tmp;
        if (c_Line.substr(0, 1) != "#") m_v_cfglist.push_back(c_Line);
      }
      fclose(c_F);

      return sl_NOERROR;
    ERROR_EXIT:
      if (c_F) fclose(c_F);
      //    REPORT_ERROR(sl_Ret, m_ach_ModuleName);
      return sl_Ret;
    };

    std::string getCfg() // relative to .dll
    {
      std::string c_Cfg = c_m3u_cfg;
      return c_Cfg;
    }

    std::string getScene() // relative to .dll
    {
      std::string c_Scene = c_m3u_content_relpath;
      c_Scene.append(m_v_cfglist[iCfg]);
      return c_Scene;
    }

    void Next()
    {
      iCfg++;
    }
  };
}
