
#include "stdafx.h"

#include "proj.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include "Timer.h"
#include <glm/gtc/type_ptr.hpp> // f. value_ptr()

#include <gl/gl.h> // 2do: include somewhere in render...

// FPS
// http://www.songho.ca/misc/timer/timer.html
// Windows --> (also doch lieber GlfW benutzen)
Timer timer;

#include <iostream>  // file io
#include <fstream>

#define VBOADD_GROUNDPLANE 1
#define VBOADD_SCENE 1
#define VBOADD_CURBSTONE 0
#define VBOADD_GUARDRAIL 0
#define VBOADD_TUNNEL 0
#define VBOADD_TRAFFICSIGNS 1
#define VBOADD_REDCAR 0
#define VBOADD_HOUSE 0
#define VBOADD_CONFERENCEROOM 0
#define VBOADD_ANTONS_VILLAGE 0
#define VBOADD_SCENE_OBJS 1            // load from obj.txt
#define VBOADD_BILLBOARDS 0            // shall be more than 1 type of billboard
#define VBOADD_20_RANDOM_HOLZSTAPEL 0
#define VBOADD_CONTICAR 0
#define VBOADD_BLACKJEEP 0
#define VBOADD_JEEP 1
#define VBOADD_BARRIERS 0
#define VBOADD_SPONZA 0

//#define TEX_TRANSPARENT true // 2do: move to e.g. GFX_defines

proj::Proj::Proj()
{
  b_show_debug = true;
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

void proj::Proj::Exit()
{
  obj::CGL_ObjectWavefront* o = new obj::CGL_ObjectWavefront(&m_render);
  obj::CObject* oo = o;
  delete o;

  for (unsigned int i = 0; i < vObjects.size()-1; i++)
  {
    obj::CObject* o = vObjects[i];
    delete o;
  }
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

  int nVAOs = m_render.vVAOs.size();
  int nobjs = 0;
  if (file)
  {
    while (std::getline(file, line)) // 2do: read whole file at once
    {
      std::vector<std::string> tokens = split(line.c_str(), ',');
      std::string sobj = tokens[0]; // e.g. "planken"
      glm::vec3 obj_pos;
      obj_pos.x = (float)atof(tokens[1].c_str());
      obj_pos.y = (float)atof(tokens[2].c_str());
      obj_pos.z = (float)atof(tokens[3].c_str());
      glm::vec3 obj_dir;
      obj_dir.x = (float)atof(tokens[4].c_str());
      obj_dir.y = (float)atof(tokens[5].c_str());
      obj_dir.z = (float)atof(tokens[6].c_str());

      // ===== 2do: move outside =====
      if ((sobj.compare("Banner")   == 0) ||
          (sobj.compare("Flag")     == 0) ||
          (sobj.compare("Woodpile") == 0) ||
          (sobj.compare("Concrete") == 0) ||
          (sobj.compare("Tree")     == 0))
      {
        obj::CBillboard* bb = new obj::CBillboard;
        bb->p_render = &m_render;
        proj::c_VAO vao;
        if      (sobj.compare("Banner")   == 0) vao = bb->Create("Banner", "tx_Banner", obj_pos, obj_dir);
        else if (sobj.compare("Flag")     == 0) vao = bb->Create("Flag", "tx_Flag", obj_pos, obj_dir, 0.9, 1.5);
        else if (sobj.compare("Woodpile") == 0) vao = bb->Create("Woodpile", "tx_Woodpile", obj_pos, obj_dir, 1.4, 1.4);
        else if (sobj.compare("Concrete") == 0) vao = bb->Create("Concrete", "tx_Concrete", obj_pos, obj_dir);
        else if (sobj.compare("Tree")     == 0) vao = bb->Create("Tree", "tx_Tree", obj_pos, obj_dir, 1.0f);
//        bb->vVaoID.push_back(m_render.vVAOs.size()); // either (A)
        m_render.vVAOs.push_back(vao);
        bb->vVaoID.push_back(nVAOs++);                 //     or (B)
#if(B_ADD_BBOX_VAO == 1)
        bb->vVaoID.push_back(nVAOs + 1); // 2do: easier (add in the obj.Create etc...)
#endif
        vObjects.push_back(bb); // 2do: wieviel Speicherverbrauch?
      }
      if (sobj.compare("planken")   == 0)
      {
        obj::CGL_ObjectWavefront* barrier1 = new obj::CGL_ObjectWavefront(&m_render);
        barrier1->sObjectFullpath = "..\\data\\virtualroad\\barrier\\bboy_barrier3.obj"; // 2do: read texture only once!
        barrier1->Load(obj_pos, obj_dir, 1.0f, 0.0f);
        barrier1->vVaoID.push_back(nVAOs++);
#if(B_ADD_BBOX_VAO == 1)
        barrier1->vVaoID.push_back(nVAOs + 1); // 2do: easier (add in the obj.Create etc...)
#endif
        vObjects.push_back(barrier1); // 2do: wieviel Speicherverbrauch?
      }
      // ===== 2do: move outside =====

      nobjs++;
    }
    file.close();
  }
  return nobjs;
}

void proj::Proj::Save_Scene_Objs()
{
  std::ofstream file("obj.txt");

  for (int i = 0; i < vObjects.size(); i++)
  {
//    if (vObjects[i]->vVaoID.size() == 0) assert(sprintf("Save_Scene_Objs(): no VAO id for Obj[%d]", i));
    assert(vObjects[i]->vVaoID.size() > 0);
    int vaoID = vObjects[i]->vVaoID[0];
    if (m_render.vVAOs[vaoID].b_doDraw != false) // 2do: eine Eigenschaft des Objekts
    {
      file << vObjects[i]->name << ',' << vObjects[i]->pos.x << ',' << vObjects[i]->pos.y << ',' << vObjects[i]->pos.z
        << ',' << vObjects[i]->dir.x << ',' << vObjects[i]->dir.y << ',' << vObjects[i]->dir.z
        << '\n';
    }
  }

  file.close();
}

int proj::Proj::Load_Objs_to_VBOs() // load individual objects to different V{A|B}O's to be able to manipulate 'em later
{ 
  proj::c_VAO vao;
  CBMPLoader ldrBMP;
  CIMGLoader ldrIMG;

  m_render.FPS(); // <-- wenn ich das ins VAO fülle, gibt's nen Fehler (erst mit dem neuen ShaderFPS)
                  //     beim LoadObjects(s.u.) call


//  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Conti",ldrBMP.loadBMP_custom("..\\data\\virtualroad\\conti.bmp")));
//  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Conti", ldrIMG.loadIMG("..\\data\\virtualroad\\conti.png", true)));
  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Conti", ldrIMG.loadIMG("..\\data\\buggyboy\\overlay2.png", true)));


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


  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Banner",   ldrIMG.loadIMG("..\\data\\buggyboy\\banner_t.png", true)));
  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Flag",     ldrIMG.loadIMG("..\\data\\buggyboy\\flag.png", true)));
  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Woodpile", ldrIMG.loadIMG("..\\data\\buggyboy\\woodpile2.png", true)));
  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Concrete", ldrIMG.loadIMG("..\\data\\nebulus\\road_tex_256x256.bmp", false)));
//  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Concrete", ldrBMP.loadBMP_custom("..\\data\\nebulus\\road_tex_256x256.bmp")));
  m_render.tex_map.insert(std::pair<std::string, GLuint>("tx_Tree",     ldrIMG.loadIMG("..\\data\\buggyboy\\tree4.png", true)));


#if (VBOADD_SCENE_OBJS == 1)
  Load_Scene_Objs();
#endif



#if (VBOADD_BILLBOARDS == 1)
  obj::CBillboard* bb = new obj::CBillboard;
  bb->p_render = &m_render;
  vao = bb->Create("Banner","tx_Banner", glm::vec3(10, 10, 0), glm::vec3(0,1,0));
  bb->vVaoID.push_back(m_render.vVAOs.size());
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs
//  delete bb;

  bb = new obj::CBillboard; 
  bb->p_render = &m_render;
  vao = bb->Create("Woodpile", "tx_Woodpile", glm::vec3(20, 10, 0), glm::vec3(1, 0, 0));
  bb->vVaoID.push_back(m_render.vVAOs.size());
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs
//  delete bb;

  // wird nicht angezeigt (wg. z = 1?)
/*  vao = bb->Create("Concrete", "tx_Concrete", glm::vec3(10, 20, 1), glm::vec3(0, 0, 1));
  bb->vVaoID.push_back(m_render.vVAOs.size());
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs
*/#endif



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
  car.sObjectFullpath = "..\\data\\virtualroad\\LowPoly_Car\\CBRed_loadBMP.obj";
  car.Load(glm::vec3(-5,-1,0),glm::vec3(0,1,0),0.04f, 0.0f); // scaled
  vObjects.push_back(car); // 2do: wieviel Speicherverbrauch?
#endif

#if (VBOADD_HOUSE == 1)
  obj::CGL_ObjectWavefront house(&m_render);
  house.sObjectFullpath = "d:\\X\\untitled.obj";
  house.Load(glm::vec3(-5,-30,0), glm::vec3(0,1,0), 0.04f, 0.0f); // scaled
  vObjects.push_back(house); // 2do: wieviel Speicherverbrauch?
#endif

#if (VBOADD_CONFERENCEROOM == 1)
  obj::CGL_ObjectWavefront room(&m_render);
  room.sObjectFullpath = "..\\data\\conference_room\\conference.obj";
  room.Load(0.003f, 0.0f, Vec3f(-5.0f, -1.0f, 0.0f)); // scaled
  vObjects.push_back(room);
#endif

#if (VBOADD_ANTONS_VILLAGE == 1)
  obj::CGL_ObjectWavefront* anton = new obj::CGL_ObjectWavefront(&m_render);
  anton->sObjectFullpath = "..\\data\\virtualroad\\erstes_projekt2.obj";
  anton->Load(glm::vec3(12.0f, 12.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 4.4f, 0.0f); // scaled
  vObjects.push_back(anton);
#endif

//  obj::CGL_ObjectWavefront holzstapel(&m_render);
//  holzstapel.sObjectFullpath = "..\\data\\virtualroad\\von_Anton\\planken.obj";
//  holzstapel.Load(0.4f, 0.0f, Vec3f(rand() % 100, rand() % 100, 0.5f)); // scaled

#if (VBOADD_20_RANDOM_HOLZSTAPEL == 1)
//#define N_HOLZSTAPEL (40+20) // temp: 20 can be set later by Mouseklick
//  obj::CGL_ObjectWavefront holzstapel[N_HOLZSTAPEL];
  for (unsigned int ui = 0; ui < 20; ui++)
  {
//    holzstapel[ui].setRender(&m_render);
    obj::CGL_ObjectWavefront* holzstapel = new obj::CGL_ObjectWavefront(&m_render);
    holzstapel->sObjectFullpath = "..\\data\\virtualroad\\von_Anton\\planken.obj";
    holzstapel->Load(glm::vec3(-100.0f+rand() % 200, -100.0f+rand() % 200, 0.5f), glm::vec3(0,1,1), 0.4f, 0.0f); // scaled
    vObjects.push_back(holzstapel); // 2do: wieviel Speicherverbrauch?
  }
//  n_holz_gestapelt = 20;
#else
//  n_holz_gestapelt = 0;
#endif

#if (VBOADD_CONTICAR == 1)
  obj::CGL_ObjectWavefront car2(&m_render);
  car2.sObjectFullpath = "..\\data\\virtualroad\\conticar4.obj";
  car2.Load(glm::vec3(30,-2,.7f),glm::vec3(0,1,0),.6f, 0.0f); // scaled
  vObjects.push_back(car2); // 2do: wieviel Speicherverbrauch?
#endif

#if (VBOADD_BLACKJEEP == 1)
  obj::CGL_ObjectWavefront car3(&m_render);
  car3.sObjectFullpath = "..\\data\\virtualroad\\lowpoly_jeep3\\jeep.obj";
  car3.Load(glm::vec3(20,6,0), glm::vec3(0,-1,0), .4f, 0.0f); // scaled
  vObjects.push_back(car3); // 2do: wieviel Speicherverbrauch?
#endif

#if (VBOADD_JEEP == 1)
  obj::CGL_ObjectWavefront* car4 = new obj::CGL_ObjectWavefront(&m_render);
  car4->sObjectFullpath = "..\\data\\virtualroad\\Jeep\\Jeep_openair.obj";
  car4->Load(glm::vec3(10,3,0), glm::vec3(-1,0,0), 0.4f, 0); // scaled
  vObjects.push_back(car4); // 2do: wieviel Speicherverbrauch?
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

/*  // c) place billboard here: das letzte wird nicht gezeichnet !?
  obj::CBillboard bb;
  bb.p_render = &m_render;
  vao = bb.Create("tx_Banner",10.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs

  vao = bb.Create("tx_Banner",20.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs
  
  vao = bb.Create("tx_Banner",30.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs

  vao = bb.Create("tx_Banner",40.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);
  vObjects.push_back(bb); // zur Kollision, eigentlich redundant zu VAOs
  */
//  obj::CGL_ObjectWavefront* barrier1 = new obj::CGL_ObjectWavefront(&m_render);
//  barrier1->sObjectFullpath = "..\\data\\virtualroad\\barrier\\bboy_barrier4.obj";
//  barrier1->Load(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f);
//  vObjects.push_back(barrier1); // 2do: wieviel Speicherverbrauch?


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


  // a) check, ob ego (Fahrzeug) mit einem Objekte kollidiert, 2do: auch bbox benutzen
//  hit_object_id = m_phys.collision_check(vObjects, m_render.Cursor);
  hit_object_id = m_phys.collision_check(vObjects, m_render.p_cam->Pos);

  // b) check, ob ego (Fahrzeug) mit einem Objekte kollidiert
  touch_object_id = m_phys.collision_check_bbox(vObjects, m_render.p_cam->Pos, m_render.p_cam->At);
  touch_object_id = m_phys.collision_check_bbox(vObjects, m_render.p_cam->Pos, m_render.p_cam->At);
  if (touch_object_id < 0) m_render.touch_object_vaoId = -1; else m_render.touch_object_vaoId = vObjects[touch_object_id]->vVaoID[0]; // check, if this works with array

  m_render.DrawVAOs_NEU();          // Draw The Scene
  err = glGetError();



  ImGuiIO& io = ImGui::GetIO();
  if (io.MouseDown)
  {
    glm::vec3 mouse3d = Mouse2Dto3D((int)io.MousePos.x, (int)io.MousePos.y);
    //    std::cout << mouse3d.x << "," << mouse3d.y << "," << mouse3d.z << std::endl;
  }



  if (b_show_debug) draw_ImGui(); else io.WantCaptureMouse = false; // fix! -> deactivate wantcapturemouse

  err = glGetError();

  SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

  return 0;
}

void proj::Proj::draw_ImGui()
{
  char buf[100];

  // --------- IMGUI ---------
  ImGui_ImplWin32_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();
  ImGui::Begin("Virtualroad");
  ImGui::Text("mouse: %.1f,%.1f", io.MousePos.x, io.MousePos.y);
  ImGui::Text("loaded: %s", m_scene.c_Scene.c_str());
  static int viewmode;
  ImGui::RadioButton("Std", &viewmode, 0);
  ImGui::RadioButton("Physics", &viewmode, 1);

  ImGui::Checkbox("solid", &m_render.b_solid);
  ImGui::Checkbox("wireframe", &m_render.b_wireframe);
  ImGui::Checkbox("culling", &m_render.b_culling);
  ImGui::RadioButton("Free", &m_render.p_cam->iStickToObj, 0);
  ImGui::RadioButton("Jeep1", &m_render.p_cam->iStickToObj, 1);
  ImGui::RadioButton("Jeep2", &m_render.p_cam->iStickToObj, 2);

  ImGui::SliderFloat("view width", &(float)m_render.p_cam->zFar, 10.0, 200.0);
  float v[3] = { m_render.p_cam->Pos.x,m_render.p_cam->Pos.y,m_render.p_cam->Pos.z };
  ImGui::InputFloat3("cam.pos", v);
  float vAt[3] = { m_render.p_cam->At.x,m_render.p_cam->At.y,m_render.p_cam->At.z };
  ImGui::InputFloat3("cam.at", vAt);
  float vDir[3] = { m_render.p_cam->At.x,m_render.p_cam->At.y,m_render.p_cam->At.z };
  ImGui::InputFloat3("cam.dir", vDir);
  ImGui::SliderFloat("cam.y", &(float)m_render.f_camy, -3.0f, 3.0f);

/*
  if (io.MouseDown)
  {
    glm::vec3 mouse3d = Mouse2Dto3D((int)io.MousePos.x, (int)io.MousePos.y);
    //    std::cout << mouse3d.x << "," << mouse3d.y << "," << mouse3d.z << std::endl;
  }
  */  float vCursor2D[3] = { winX,winY,winZ };
  ImGui::InputFloat3("Cursor2d", vCursor2D);
  float vCursor[3] = { m_render.Cursor.x,m_render.Cursor.y,m_render.Cursor.z };
  ImGui::InputFloat3("Cursor", vCursor);
  ImGui::End();



  ImGui::Begin("Render");

  const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
  const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
  ImGui::Text("%s", renderer);

  float FPS = 1000.0f / (float)timer.getElapsedTimeInMilliSec();
  m_render.aFPS[m_render.idxFPS++ % FPS_LOWPASS] = FPS;
  FPS = 0.0; for (int i = 0; i < FPS_LOWPASS; i++) { FPS += m_render.aFPS[i]; } FPS = FPS / FPS_LOWPASS;
  ImGui::Text("%.0f FPS", FPS);

  static int selected = -1;
  sprintf(buf, "%d Obj's (VAOs!) rendered", m_render.vVAOs.size());
  if (ImGui::TreeNode(buf))
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
      ImGui::Text("%d", i); ImGui::NextColumn();
      ImGui::Text(m_render.vVAOs[i].Name.c_str()); ImGui::NextColumn();
      ImGui::Text("%d", m_render.vVAOs[i].uiVertexCount); ImGui::NextColumn();
      ImGui::Text("%.2f,%.2f,%.2f", m_render.vVAOs[i].pos.x, m_render.vVAOs[i].pos.y, m_render.vVAOs[i].pos.z); ImGui::NextColumn();
      ImGui::Text("%d", m_render.vVAOs[i].ui_idTexture); ImGui::NextColumn();
//      ImGui::Text("%s", m_render.tex_map.[m_render.vVAOs[i].ui_idTexture]);
    }
    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::TreePop();
  }
  ImGui::End();



  ImGui::Begin("Physics");

  unsigned int pp = m_phys.player_scene_pos(m_scene, m_render.Cursor);
  unsigned int nLine = 0;
  unsigned int nMarker = 2;
  m_phys.trajectory_id_to_line_marker(m_scene, pp, nLine, nMarker);
  if (nMarker < (m_scene.m_SceneLoader.m_c_Markers[nLine].size() - 1))
  {
    S_Point3D p0 = m_scene.m_SceneLoader.m_c_Markers[nLine][nMarker].s_Left;
    S_Point3D p1 = m_scene.m_SceneLoader.m_c_Markers[nLine][nMarker].s_Right;
    S_Point3D p2 = m_scene.m_SceneLoader.m_c_Markers[nLine][nMarker + 1].s_Right;
    S_Point3D p3 = m_scene.m_SceneLoader.m_c_Markers[nLine][nMarker + 1].s_Left;
    GLfloat z = .1f;
    m_render.Scenepos[0] = glm::vec3(p0.rl_X, p0.rl_Y, z);
    m_render.Scenepos[1] = glm::vec3(p1.rl_X, p1.rl_Y, z);
    m_render.Scenepos[2] = glm::vec3(p2.rl_X, p2.rl_Y, z);
    m_render.Scenepos[3] = glm::vec3(p3.rl_X, p3.rl_Y, z);
  }
  else
  {
    nLine = 1;
  }

  ImGui::Text("Track nearest: %d", pp);
  std::string s_obj = "";
  if (hit_object_id >= 0) s_obj = vObjects[hit_object_id]->name;
  ImGui::Text("Hit object: %d (%s)", hit_object_id, s_obj.c_str());
///////////
///////////
/////////// touched_obj_id v. vObj
///////////     vs.
/////////// touched_obj_id v. vVAOs
///////////
///////////
  if ((touch_object_id < 0) || (touch_object_id > vObjects.size()-1)) s_obj = ""; else s_obj = vObjects[touch_object_id]->name;
  ImGui::Text("Touched object: %d (%s)", touch_object_id, s_obj.c_str());

  //  static int selected = -1;
  sprintf(buf, "%d Obj's collision checked", vObjects.size());
  if (ImGui::TreeNode(buf))
  {
    ImGui::Columns(4, "cols"); // no. of columns
    ImGui::Separator();
    ImGui::Text("#"); ImGui::NextColumn();
    ImGui::Text("Obj"); ImGui::NextColumn();
    ImGui::Text("#Vao"); ImGui::NextColumn();
    ImGui::Text("VAOs"); ImGui::NextColumn();
                                              //    ImGui::Text("#Vtx"); ImGui::NextColumn();
                                              //    ImGui::Text("Pos"); ImGui::NextColumn();
                                              //    ImGui::Text("idTxt"); ImGui::NextColumn();
    ImGui::Separator();
    for (unsigned int i = 0; i < vObjects.size(); i++)
    {
      ImGui::Text("%d", i); ImGui::NextColumn();
      ImGui::Text(vObjects[i]->name.c_str()); ImGui::NextColumn();
      ImGui::Text("%d", vObjects[i]->vVaoID.size()); ImGui::NextColumn();
      std::string s = "";
      for (unsigned int j = 0; j < vObjects[i]->vVaoID.size(); j++)
      {
        s += std::to_string(vObjects[i]->vVaoID[j]) + ',';
      }
      ImGui::Text(s.c_str()); ImGui::NextColumn();
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
}
