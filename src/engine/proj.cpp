
#include "stdafx.h"

#include "proj.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include "Timer.h"
#include <glm/gtc/type_ptr.hpp> // f. value_ptr()

// FPS
// http://www.songho.ca/misc/timer/timer.html
// Windows --> (also doch lieber GlfW benutzen)
Timer timer;

#include <iostream>  // file io
#include <fstream>

#define VBOADD_GROUNDPLANE 0
#define VBOADD_SCENE 0
#define VBOADD_CURBSTONE 0
#define VBOADD_GUARDRAIL 0
#define VBOADD_TUNNEL 0
#define VBOADD_TRAFFICSIGNS 0
#define VBOADD_REDCAR 0
#define VBOADD_CONFERENCEROOM 0
#define VBOADD_ANTONS_VILLAGE 0
#define VBOADD_SCENE_OBJS 0            // load from obj.txt
#define VBOADD_BILLBOARDS 0            // shall be more than 1 type of billboard
#define VBOADD_20_RANDOM_HOLZSTAPEL 0
#define VBOADD_CONTICAR 0
#define VBOADD_JEEP 0
#define VBOADD_BARRIERS 0
#define VBOADD_SPONZA 0

proj::Proj::Proj()
{
}

int proj::Proj::Init()
{
  m_render.Init(); // InitGL + Initshaders, kann auch später aufgerufen werden...

  // load geometry data (textures loaded within)
  // a) fill in all pos, color, uv infos into arrays
  Load_Objs_to_VBOs();

  // b) bind to VAO's
  m_render.Bind_VBOs_to_VAOs(); // now hand over VBO's to VAO's

  vTrajPosprev = glm::vec3(0.0f,0.0f,0.0f);

  hit_object_id = 0;

  return 0;
}

// Tokenizer, https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
//2do: move to own header file
std::vector<std::string> split(const char *str, char c = ' ')
{
  std::vector<std::string> result;

  do
  {
    const char *begin = str;

    while (*str != c && *str)
      str++;

    result.push_back(std::string(begin, str));
  } while (0 != *str++);

  return result;
}

int proj::Proj::Load_Scene_Objs()
{
  std::ifstream file("obj.txt");
  std::string line;

  int nobjs = 0;
  if (file)
  {
    while (std::getline(file, line))
    {
      std::vector<std::string> tokens = split(line.c_str(), ',');
      std::string sobj = tokens[0];
      glm::vec3 pos;
      pos.x = (float)atof(tokens[1].c_str());
      pos.y = (float)atof(tokens[2].c_str());
      pos.z = (float)atof(tokens[3].c_str());

      // ===== 2do: move outside =====
/*      int nVAOs = m_render.vVAOs.size();
      holzstapel[n_holz_gestapelt].setRender(&m_render);
      holzstapel[n_holz_gestapelt].sObjectFullpath = "..\\data\\virtualroad\\von_Anton\\planken.obj";
      holzstapel[n_holz_gestapelt].Load(0.4f, 0.0f, Vec3f(pos.x, pos.y, pos.z)); // scaled
//      m_render.Bind_NEW__VBOs_to_VAOs(nVAOs);
      vObjects.push_back(holzstapel[n_holz_gestapelt]); // 2do: wieviel Speicherverbrauch?
      n_holz_gestapelt++;
*/      // ===== 2do: move outside =====
      obj::CBillboard bb;
      bb.p_render = &m_render;
      proj::c_VAO vao = bb.Create(pos.x, pos.y, pos.z);
      m_render.vVAOs.push_back(vao);


      nobjs++;
    }
    file.close();
  }
  return nobjs;
}


int proj::Proj::Load_Objs_to_VBOs() // load individual objects to different V{A|B}O's to be able to manipulate 'em later
{ 
  proj::c_VAO vao;
  CBMPLoader ldrBMP;
  CIMGLoader ldrIMG;

  m_render.FPS(); // <-- wenn ich das ins VAO fülle, gibt's nen Fehler (erst mit dem neuen ShaderFPS)
                  //     beim LoadObjects(s.u.) call


  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Conti",ldrBMP.loadBMP_custom("..\\data\\virtualroad\\conti.bmp")));


#if (VBOADD_GROUNDPLANE == 1)
  m_groundplane.p_render = &m_render;
  vao = m_groundplane.Create();
  m_render.vVAOs.push_back(vao);
#endif

  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Road", ldrBMP.loadBMP_custom("..\\data\\buggyboy\\bboy_road_vert4.bmp")));
  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Water", ldrBMP.loadBMP_custom("..\\data\\buggyboy\\bboy_water.bmp")));



  // i) Load Scene VAOs
#if (VBOADD_SCENE == 1)
  m_scene.Load();
  m_render.p_Scene = &m_scene;
  m_render.Scene_to_VBO();
#endif


  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Banner", ldrIMG.loadIMG("..\\data\\buggyboy\\banner_t.png", true)));


#if (VBOADD_SCENE_OBJS == 1)
  Load_Scene_Objs();
#endif



#if (VBOADD_BILLBOARDS == 1)
// a) place billboard here, as it needs texture id = 4 -> 2do: simplify tex-ID
  obj::CBillboard bb;
  bb.p_render = &m_render;
  vao = bb.Create(10.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs

  vao = bb.Create(20.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs
#endif



  // ii) Load VAOs depending on scene
  m_scenebuilder.p_render = &m_render;
  m_scenebuilder.p_scene = &m_scene;

#if (VBOADD_GUARDRAIL == 1)
  vao = m_scenebuilder.CreateGuardrails(); // "align" to road
  m_render.vVAOs.push_back(vao);
#endif
#if (VBOADD_CURBSTONE == 1)
  vao = m_scenebuilder.CreateCurbstones(); // "align" to road
  m_render.vVAOs.push_back(vao);
#endif
#if (VBOADD_TUNNEL == 1)
  vao = m_scenebuilder.CreateTunnel();     // "align" to road
  m_render.vVAOs.push_back(vao);
#endif


  // iii) Load individual VAO's
#if (VBOADD_TRAFFICSIGNS == 1)
  m_trafficsigns.p_render = &m_render;

  vao = m_trafficsigns.Add(m_render.vVAOs,  12.5f,0.5f,0.0f,  1.0f,0.0f,0.0f);
  m_render.vVAOs.push_back(vao);
  vao = m_trafficsigns.Add(m_render.vVAOs, -12.5f,0.5f,0.0f,  1.0f,0.0f,0.0f);
  m_render.vVAOs.push_back(vao);
#endif


  GLenum err = glGetError();

#if (VBOADD_REDCAR == 1)
  obj::CGL_ObjectWavefront car(&m_render);
//  car.sObjectFullpath = "..\\data\\virtualroad\\LowPoly_Car\\CBRed_loadBMP.obj";
  car.sObjectFullpath = "d:\\X\\untitled.obj";
  car.Load(0.04f, 0.0f, Vec3f(-5.0f, -1.0f, 0.0f)); // scaled
#endif

#if (VBOADD_CONFERENCEROOM == 1)
  obj::CGL_ObjectWavefront room(&m_render);
  room.sObjectFullpath = "..\\data\\conference_room\\conference.obj";
  room.Load(0.003f, 0.0f, Vec3f(-5.0f, -1.0f, 0.0f)); // scaled
  vObjects.push_back(room);
#endif

#if (VBOADD_ANTONS_VILLAGE == 1)
  obj::CGL_ObjectWavefront anton(&m_render);
  anton.sObjectFullpath = "..\\data\\virtualroad\\erstes_projekt2.obj";
  anton.Load(4.4f, 0.0f, Vec3f(12.0f, 12.0f, 0.0f)); // scaled
#endif

//  obj::CGL_ObjectWavefront holzstapel(&m_render);
//  holzstapel.sObjectFullpath = "..\\data\\virtualroad\\von_Anton\\planken.obj";
//  holzstapel.Load(0.4f, 0.0f, Vec3f(rand() % 100, rand() % 100, 0.5f)); // scaled

#if (VBOADD_20_RANDOM_HOLZSTAPEL == 1)
//#define N_HOLZSTAPEL (40+20) // temp: 20 can be set later by Mouseklick
//  obj::CGL_ObjectWavefront holzstapel[N_HOLZSTAPEL];
  for (unsigned int ui = 0; ui < 20; ui++)
  {
    holzstapel[ui].setRender(&m_render);
    holzstapel[ui].sObjectFullpath = "..\\data\\virtualroad\\von_Anton\\planken.obj";
    holzstapel[ui].Load(0.4f, 0.0f, Vec3f(-100.0f+rand() % 200, -100.0f+rand() % 200, 0.5f)); // scaled
  }
  n_holz_gestapelt = 20;
#else
  n_holz_gestapelt = 0;
#endif

#if (VBOADD_CONTICAR == 1)
  obj::CGL_ObjectWavefront car2(&m_render);
  car2.sObjectFullpath = "..\\data\\virtualroad\\conticar4.obj";
  car2.Load(.6f, 0.0f, Vec3f(20.0f, 6.0f, 0.7f)); // scaled
//  car2.sObjectFullpath = "..\\data\\virtualroad\\lowpoly_jeep3\\jeep.obj";
//  car2.Load(.6f, 0.0f, Vec3f(20.0f, 6.0f, 0.7f)); // scaled
  vObjects.push_back(car2); // 2do: wieviel Speicherverbrauch?
#endif

#if (VBOADD_JEEP == 1)
  obj::CGL_ObjectWavefront car3(&m_render);
  car3.sObjectFullpath = "..\\data\\virtualroad\\Jeep\\Jeep_openair.obj";
  car3.Load(0.4f, 0.0f, Vec3f(10.0f, 3.0f, 0.0f)); // scaled
  vObjects.push_back(car3); // 2do: wieviel Speicherverbrauch?
#endif

// b) place billboard here .. ok

#if (VBOADD_BARRIERS == 1)
  obj::CGL_ObjectWavefront barrier1(&m_render);
  barrier1.sObjectFullpath = "..\\data\\virtualroad\\barrier\\bboy_barrier3.obj";
  barrier1.Load(1.0f,0.0f,Vec3f(0.0f,0.0f,0.0f));
  vObjects.push_back(barrier1); // 2do: wieviel Speicherverbrauch?
  for (unsigned int ui=1;ui<5;ui++)
  {
//    barrier1.setPos()
    barrier1.PartsToVBOs(Vec3f(10.0f+(float)ui*4.0f, 0.0f, 0.0f));
    barrier1.PartsToVAOs(Vec3f(10.0f + (float)ui*4.0f, 0.0f, 0.0f));
//    vObjects.push_back(barrier1); // 2do: wieviel Speicherverbrauch?
  }
#endif

#if (VBOADD_SPONZA == 1)
  // currently loads too long
  obj::CGL_ObjectWavefront sponza(&m_render);
  sponza.sObjectFullpath = "..\\data\\sponza\\sponza.obj";
//  sponza.sObjectFullpath = "..\\..\\sponza\\sponza.obj";
  sponza.Load(1.0f, 0.0f, Vec3f(12.0f, 12.0f, 0.0f)); // scaled
#endif

  // c) place billboard here: das letzte wird nicht gezeichnet !?
  obj::CBillboard bb;
  bb.p_render = &m_render;
  vao = bb.Create(10.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs

  vao = bb.Create(20.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs
  
  vao = bb.Create(30.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs

  vao = bb.Create(40.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs

  obj::CGL_ObjectWavefront barrier1(&m_render);
  barrier1.sObjectFullpath = "..\\data\\virtualroad\\barrier\\bboy_barrier3.obj";
  barrier1.Load(1.0f, 0.0f, Vec3f(0.0f, 0.0f, 0.0f));
  vObjects.push_back(barrier1); // 2do: wieviel Speicherverbrauch?


  err = glGetError();

  assert(m_render.vVAOs.size()<m_render.VBOCOUNT);

  return 0;
}

// https://www.opengl.org/discussion_boards/showthread.php/126012-converting-window-coordinates-to-world-coordinates
glm::vec3 proj::Proj::Mouse2Dto3D(int x, int y)
{
  GLint viewport[4]; //var to hold the viewport info
  GLdouble modelview[16]; //var to hold the modelview info
  GLdouble projection[16]; //var to hold the projection matrix info
  GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

  glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
  glGetDoublev(GL_PROJECTION_MATRIX, projection); //get the projection matrix info
  glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info

  winX = (float)x;
  winY = (float)viewport[3] - (float)y;
  //  winZ = 1.0; // The near clip plane is at z = 0 and the far clip plane is at z = 1
  glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ); // http://nehe.gamedev.net/article/using_gluunproject/16013/

                                                                         //get the world coordinates from the screen coordinates
//  const float *p_Model = (const float*)glm::value_ptr(m_render.p_cam->Model);
//  for (int i = 0; i < 16; ++i) modelview[i] = p_Model[i];
//  const float *p_Persp = (const float*)glm::value_ptr(m_render.p_cam->Projection);
//  for (int i = 0; i < 16; ++i) projection[i] = p_Persp[i];
  gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);

  
  m_render.Cursor.x = m_render.p_cam->At.x + worldX; // <-- OpenGL Cursor
  m_render.Cursor.y = m_render.p_cam->At.y + worldY;
  m_render.Cursor.z = 0.1f;// worldZ;
//  m_render.Cursor.r = 255;
//  m_render.Cursor.g = 0;
//  m_render.Cursor.b = 0;
//  m_render.Cursor.x = worldX; // <-- OpenGL Cursor
//  m_render.Cursor.y = worldY;
//  m_render.Cursor.z = worldZ;

  return glm::vec3(worldX, worldY, worldZ);
}


int proj::Proj::DoIt()
{
  GLenum err = GL_NO_ERROR;

  timer.stop();
  timer.start();

  wglMakeCurrent(m_render.hDC,m_render.hRC); // ;-) now Tab-switching in MTS possible


  hit_object_id = m_phys.collision_check(vObjects, m_render.Cursor);


  m_render.DrawVAOs_NEU();          // Draw The Scene
  err = glGetError();


  // --------- IMGUI ---------
  ImGui_ImplWin32_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();
  ImGui::Begin("Papa:");
  ImGui::Text("Hallo Anton.");
  ImGui::Text("mouse: %f,%f",io.MousePos.x,io.MousePos.y);
  ImGui::Text("loaded: %s", m_scene.c_Scene.c_str());
  static int viewmode;
  ImGui::RadioButton("Std",     &viewmode, 0);
  ImGui::RadioButton("Physics", &viewmode, 1);
  static bool b_wireframe;
  ImGui::Checkbox("wireframe", &b_wireframe);
  ImGui::Checkbox("culling", &m_render.b_culling);
  ImGui::RadioButton("Free", &m_render.p_cam->iStickToObj,0);
  ImGui::RadioButton("Jeep1", &m_render.p_cam->iStickToObj,1);
  ImGui::RadioButton("Jeep2", &m_render.p_cam->iStickToObj,2);
  for (unsigned int ui = 0; ui < m_render.vVAOs.size(); ui++) m_render.vVAOs[ui].b_Wireframe = (int)b_wireframe;
//  static int vw;
  ImGui::SliderFloat("view width", &(float)m_render.p_cam->zFar, 10.0, 200.0);
  float v[3] = { m_render.p_cam->Pos.x,m_render.p_cam->Pos.y,m_render.p_cam->Pos.z };
  ImGui::InputFloat3("cam.pos", v);
  float vAt[3] = { m_render.p_cam->At.x,m_render.p_cam->At.y,m_render.p_cam->At.z };
  ImGui::InputFloat3("cam.at", vAt);
  float vDir[3] = { m_render.p_cam->At.x,m_render.p_cam->At.y,m_render.p_cam->At.z };
  ImGui::InputFloat3("cam.dir", vDir);
  ImGui::SliderFloat("cam.y", &(float)m_render.f_camy, -3.0f, 3.0f);


  if (io.MouseDown)
  {
    glm::vec3 mouse3d = Mouse2Dto3D((int)io.MousePos.x, (int)io.MousePos.y);
//    std::cout << mouse3d.x << "," << mouse3d.y << "," << mouse3d.z << std::endl;
  }
  float vCursor2D[3] = { winX,winY,winZ };
  ImGui::InputFloat3("Cursor2d", vCursor2D);
  float vCursor[3] = { m_render.Cursor.x,m_render.Cursor.y,m_render.Cursor.z };
  ImGui::InputFloat3("Cursor", vCursor);
  ImGui::End();



  ImGui::Begin("Render");

  float FPS = 1000.0f / (float)timer.getElapsedTimeInMilliSec();
  m_render.aFPS[m_render.idxFPS++ % FPS_LOWPASS] = FPS;
  FPS = 0.0; for (int i = 0; i < FPS_LOWPASS; i++) { FPS += m_render.aFPS[i]; } FPS = FPS / FPS_LOWPASS;
  ImGui::Text("%.0f FPS", FPS);

  static int selected = -1;
  if (ImGui::TreeNode("Obj's (VAOs!) rendered"))
  {
    ImGui::Columns(5, "cols"); // no. of columns
    ImGui::Separator();
    ImGui::Text("#"); ImGui::NextColumn();
    ImGui::Text("VAO"); ImGui::NextColumn();  // 2do: show object
    ImGui::Text("#Vtx"); ImGui::NextColumn();
    ImGui::Text("Pos"); ImGui::NextColumn();
    ImGui::Text("idTxt"); ImGui::NextColumn();
    ImGui::Separator();
    for (unsigned int i = 0; i < m_render.vVAOs.size(); i++)
    {
      ImGui::Text("%d",i); ImGui::NextColumn();
      ImGui::Text(m_render.vVAOs[i].Name.c_str()); ImGui::NextColumn();
      ImGui::Text("%d",m_render.vVAOs[i].uiVertexCount); ImGui::NextColumn();
      ImGui::Text("%.2f,%.2f,%.2f",m_render.vVAOs[i].vPos.x, m_render.vVAOs[i].vPos.y, m_render.vVAOs[i].vPos.z); ImGui::NextColumn();
      ImGui::Text("%d", m_render.vVAOs[i].ui_idTexture); ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::TreePop();
  }
  ImGui::End();

  
  
  ImGui::Begin("Physics");
  
  int pp = m_phys.player_scene_pos(m_scene, m_render.Cursor);
  ImGui::Text("Track nearest: %d", pp);
  std::string s_obj = "";
  if (hit_object_id >= 0) s_obj = vObjects[hit_object_id].name;
  ImGui::Text("Hit object: %d (%s)", hit_object_id, s_obj.c_str());
  
  //  static int selected = -1;
  if (ImGui::TreeNode("Obj's collision checked"))
  {
    ImGui::Columns(2, "cols"); // no. of columns
    ImGui::Separator();
    ImGui::Text("#"); ImGui::NextColumn();
    ImGui::Text("VAO"); ImGui::NextColumn();  // 2do: show object
//    ImGui::Text("#Vtx"); ImGui::NextColumn();
//    ImGui::Text("Pos"); ImGui::NextColumn();
//    ImGui::Text("idTxt"); ImGui::NextColumn();
    ImGui::Separator();
    for (unsigned int i = 0; i < vObjects.size(); i++)
    {
      ImGui::Text("%d", i); ImGui::NextColumn();
      ImGui::Text(vObjects[i].name.c_str()); ImGui::NextColumn();
//      ImGui::Text("%d", m_render.vVAOs[i].uiVertexCount); ImGui::NextColumn();
//      ImGui::Text("%.2f,%.2f,%.2f", m_render.vVAOs[i].vPos.x, m_render.vVAOs[i].vPos.y, m_render.vVAOs[i].vPos.z); ImGui::NextColumn();
//      ImGui::Text("%d", m_render.vVAOs[i].ui_idTexture); ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::TreePop();
  }
//  ImGui::SetNextTreeNodeOpen(true);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  // --------- IMGUI ---------

  err = glGetError();

  SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

  return 0;
}
