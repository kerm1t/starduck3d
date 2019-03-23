
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

  return 0;
}

/*void proj::Proj::LoadMoving() // e.g. cars
{
  Vec3f p = Vec3f(2.0f,1.0f,0.0f);
  Vec3f d = Vec3f(3.0f,1.0f,0.0f);

  // a) object in the traditional way
  ////	m_moving[0].vCount         = m_render.vCount;
  //    m_moving[0].vertexArray    = m_render.vertexArray;
  m_moving[0].positionBuffer = m_render.positionBuffer;
  m_moving[0].colorBuffer    = m_render.colorBuffer;
  m_moving[0].VBOindex       = m_render.vVAOs.size();// VBO_MOVING1;

  m_moving[0].Init(1);
  m_moving[0].iNdx = 0;
  m_moving[0].Add_Cube(p.x,p.y,p.z,  d.x,d.y,d.z);
  m_moving[0].Fini();

  c_VAO vao;
  vao.uiVertexCount = m_moving[0].nVertices;
  vao.t_Shade = SHADER_COLOR_FLAT;
  m_render.vVAOs.push_back(vao);
}
*/

int proj::Proj::Load_Objs_to_VBOs() // load individual objects to different V{A|B}O's to be able to manipulate 'em later
{ 
  proj::c_VAO vao;
  CBMPLoader ldrBMP;
  CIMGLoader ldrIMG;

  m_render.FPS(); // <-- wenn ich das ins VAO fülle, gibt's nen Fehler (erst mit dem neuen ShaderFPS)
                  //     beim LoadObjects(s.u.) call


  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\virtualroad\\conti.bmp"));


  // DUMMY
  m_render.Triangles_to_VBO(Vec3f( 0.0f,0.0f,0.0f));
  m_render.Triangles_to_VBO(Vec3f( 9.0f,3.0f,0.5f));
  m_render.Triangles_to_VBO(Vec3f(12.0f,6.0f,0.5f));
  m_render.Triangles_to_VBO(Vec3f(14.0f,6.0f,0.5f));
  

  m_groundplane.p_render = &m_render;
  vao = m_groundplane.Create();
  m_render.vVAOs.push_back(vao);


  // test bbox, 2do: wireframe e.g. GL_LINESTRIP
  m_cube.p_render = &m_render;
  vao = m_cube.Create({ 0,0,0 }, {2,2,2});
  m_render.vVAOs.push_back(vao);


  // i) Load Scene VAOs
  m_scene.Load();
  m_render.p_Scene = &m_scene;
  m_render.Scene_to_VBO();



//  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\virtualroad\\road_tex_256x256.bmp"));
  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\buggyboy\\bboy_road_vert4.bmp"));
  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\buggyboy\\bboy_water.bmp"));



//  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\buggyboy\\flag.bmp"));
  m_render.vGLTexture.push_back(ldrIMG.loadIMG("..\\data\\buggyboy\\banner_t.png", true));
  // a) place billboard here, as it needs texture id = 4 -> 2do: simplify tex-ID
  obj::CBillboard bb;
  bb.p_render = &m_render;
  vao = bb.Create(10.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);

  vao = bb.Create(20.0f, 10.0f, 0.0f);
  m_render.vVAOs.push_back(vao);

  // ii) Load VAOs depending on scene
  m_scenebuilder.p_render = &m_render;
  m_scenebuilder.p_scene = &m_scene;

//#define CURBSTONE
#ifdef GUARDRAIL
  vao = m_scenebuilder.CreateGuardrails(); // "align" to road
  m_render.vVAOs.push_back(vao);
#endif
#ifdef CURBSTONE
  vao = m_scenebuilder.CreateCurbstones(); // "align" to road
  m_render.vVAOs.push_back(vao);
#endif

  vao = m_scenebuilder.CreateTunnel();     // "align" to road
  m_render.vVAOs.push_back(vao);



  // iii) Load individual VAO's
  m_trafficsigns.p_render = &m_render;

  vao = m_trafficsigns.Add(m_render.vVAOs,  12.5f,0.5f,0.0f,  1.0f,0.0f,0.0f);
  m_render.vVAOs.push_back(vao);
  vao = m_trafficsigns.Add(m_render.vVAOs, -12.5f,0.5f,0.0f,  1.0f,0.0f,0.0f);
  m_render.vVAOs.push_back(vao);


  // iv) Load VAOs for Moving objects
//  m_render.m_Moving[0] = &m_moving[0]; // <-- movement applied to vertexshader(offset) while drawing/rendering
//  m_render.m_Moving[1] = &m_moving[1];
 
  GLenum err = glGetError();
/*  obj::CGL_ObjectWavefront car(&m_render);
//  car.sObjectFullpath = "..\\data\\virtualroad\\LowPoly_Car\\CBRed_loadBMP.obj";
  car.sObjectFullpath = "d:\\X\\untitled.obj";
  car.Load(0.04f, 0.0f, Vec3f(-5.0f, -1.0f, 0.0f)); // scaled
*/
/*  obj::CGL_ObjectWavefront room(&m_render);
  room.sObjectFullpath = "..\\data\\conference_room\\conference.obj";
  room.Load(0.003f, 0.0f, Vec3f(-5.0f, -1.0f, 0.0f)); // scaled
  vObjects.push_back(room);
  */

  obj::CGL_ObjectWavefront anton(&m_render);
  anton.sObjectFullpath = "..\\data\\virtualroad\\erstes_projekt2.obj";
  anton.Load(4.4f, 0.0f, Vec3f(12.0f, 12.0f, 0.0f)); // scaled

//  obj::CGL_ObjectWavefront holzstapel(&m_render);
//  holzstapel.sObjectFullpath = "..\\data\\virtualroad\\von_Anton\\planken.obj";
//  holzstapel.Load(0.4f, 0.0f, Vec3f(rand() % 100, rand() % 100, 0.5f)); // scaled

//#define N_HOLZSTAPEL (40+20) // temp: 20 can be set later by Mouseklick
//  obj::CGL_ObjectWavefront holzstapel[N_HOLZSTAPEL];
  for (unsigned int ui = 0; ui < 0; ui++)
  {
    holzstapel[ui].setRender(&m_render);
    holzstapel[ui].sObjectFullpath = "..\\data\\virtualroad\\von_Anton\\planken.obj";
    holzstapel[ui].Load(0.4f, 0.0f, Vec3f(-100.0f+rand() % 200, -100.0f+rand() % 200, 0.5f)); // scaled
  }
  n_holz_gestapelt = 0;

  obj::CGL_ObjectWavefront car2(&m_render);
//  car2.sObjectFullpath = "..\\data\\virtualroad\\conticar4.obj";
//  car2.Load(.6f, 0.0f, Vec3f(20.0f, 6.0f, 0.7f)); // scaled
  car2.sObjectFullpath = "..\\data\\virtualroad\\lowpoly_jeep3\\jeep.obj";
  car2.Load(.6f, 0.0f, Vec3f(20.0f, 6.0f, 0.7f)); // scaled


  obj::CGL_ObjectWavefront car3(&m_render);
  car3.sObjectFullpath = "..\\data\\virtualroad\\Jeep\\Jeep_openair.obj";
  car3.Load(0.4f, 0.0f, Vec3f(10.0f, 3.0f, 0.0f)); // scaled

// b) place billboard here .. ok

  obj::CGL_ObjectWavefront barrier1(&m_render);
  barrier1.sObjectFullpath = "..\\data\\virtualroad\\barrier\\bboy_barrier3.obj";
  barrier1.Load();
  for (unsigned int ui=1;ui<5;ui++)
  {
    barrier1.PartsToVBOs(Vec3f(10.0f+(float)ui*4.0f, 0.0f, 0.0f));
    barrier1.PartsToVAOs(Vec3f(10.0f + (float)ui*4.0f, 0.0f, 0.0f));
  }
/* currently loads too long
  obj::CGL_ObjectWavefront sponza(&m_render);
  sponza.sObjectFullpath = "..\\data\\sponza\\sponza.obj";
//  sponza.sObjectFullpath = "..\\..\\sponza\\sponza.obj";
  sponza.Load(1.0f, 0.0f, Vec3f(12.0f, 12.0f, 0.0f)); // scaled
  */

  // c) place billboard here: invisible

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

int proj::Proj::PlayerPos()
{
  float dist_min = 9999.9f;
  float dist;
  int iTrack_nearest = -1;
  for (int i = 0; i < m_scene.trajectory_len; i++)
  {
    glm::vec3 v3 = glm::vec3(m_scene.m_SceneLoader.m_c_Trajectory[i].s_Pos.rl_X,
                             m_scene.m_SceneLoader.m_c_Trajectory[i].s_Pos.rl_Y,
                             0.0f);
    dist = glm::length(v3 - m_render.Cursor);
    if (dist < dist_min)
    {
      dist_min = dist;
      iTrack_nearest = i;
    }
  }
  return iTrack_nearest;
}

int proj::Proj::DoIt()
{
  GLenum err = GL_NO_ERROR;

  timer.stop();
  timer.start();

  wglMakeCurrent(m_render.hDC,m_render.hRC); // ;-) now Tab-switching in MTS possible

  // only f. fixed pipeline --> glTranslatef(0, 0, -2.9f);

  // objects' movement --> has to be done in an own loop / !?thread!?
  if (!bPause) // key [p] pressed
  {
    m_moving[0].MoveTo(glm::vec3(3.0f,1.0f,0.0f)); // <-- 2do, kann ins Init()
//    m_moving[1].Move(glm::vec3(0.005f,0.0f,0.0f));
    m_moving[1].iTspeed++;
//    if (m_moving[1].iTspeed >= 10)
//    {
      m_moving[1].iTspeed = 0;
      if (m_moving[1].iT < m_scene.trajectory_len-2) m_moving[1].iT++; else m_moving[1].iT = 0;
      int iTx = m_moving[1].iT;
      glm::vec3 v3 = glm::vec3(m_scene.m_SceneLoader.m_c_Trajectory[iTx].s_Pos.rl_X,
                               m_scene.m_SceneLoader.m_c_Trajectory[iTx].s_Pos.rl_Y,
                               0.0f);
      //			m_moving[1].MoveTo(v3);
      glm::vec3 vMove = v3-vTrajPosprev; // consider mouse controlled position change
      m_moving[1].Move(vMove);
      vTrajPosprev = v3;

      if (m_render.p_cam->bStickToObject)
      {
        glm::vec3 vVehDirNorm  = glm::normalize(m_moving[1].direction);
        glm::float32 fZtmp     = m_render.p_cam->Pos[2];
        m_render.p_cam->Pos    = m_moving[1].position - vVehDirNorm*12.0f;
        m_render.p_cam->Pos[2] = fZtmp;
        m_render.p_cam->At     = m_moving[1].position;
    	}
//    }
  }

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
  float FPS = 1000.0f / (float)timer.getElapsedTimeInMilliSec();
  m_render.aFPS[m_render.idxFPS++ % FPS_LOWPASS] = FPS;
  FPS = 0.0; for (int i = 0; i < FPS_LOWPASS; i++) {
    FPS += m_render.aFPS[i];
  }FPS = FPS / FPS_LOWPASS;
  ImGui::Text("%.0f FPS", FPS);
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
  int pp = PlayerPos();
  ImGui::Text("Track nearest: %d", pp);
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

  ImGui::Begin("Objects");
  static int selected = -1;
  if (ImGui::TreeNode("Obj's rendered"))
  {
    ImGui::Columns(5, "cols"); // no. of columns
    ImGui::Separator();
    ImGui::Text("#"); ImGui::NextColumn();
    ImGui::Text("Obj."); ImGui::NextColumn();
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

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  // --------- IMGUI ---------

  err = glGetError();

  SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

  return 0;
}
