
#include "stdafx.h"

#include "proj.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include "Timer.h"

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


  m_render.FPS(); // <-- wenn ich das ins VAO fülle, gibt's nen Fehler (erst mit dem neuen ShaderFPS)
                  //     beim LoadObjects(s.u.) call


//  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("D:\\__OpenGL_files\\arial_font.bmp")); <-- 2do: errorhandler + log visualisieren
///  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\virtualroad\\arial_font.bmp"));
  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\virtualroad\\conti.bmp"));


  // DUMMY
  m_render.Triangles_to_VBO(Vec3f( 0.0f,0.0f,0.0f));
  m_render.Triangles_to_VBO(Vec3f( 9.0f,3.0f,0.5f));
  m_render.Triangles_to_VBO(Vec3f(12.0f,6.0f,0.5f));
  m_render.Triangles_to_VBO(Vec3f(14.0f,6.0f,0.5f));
  

  m_groundplane.p_render = &m_render;
  vao = m_groundplane.Create();
  m_render.vVAOs.push_back(vao);


  // i) Load Scene VAOs
  m_scene.Load();
  m_render.m_Scene = &m_scene;
  m_render.Scene_to_VBO();

//  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\virtualroad\\road_tex_256x256.bmp"));
  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\buggyboy\\bboy_road_vert4.bmp"));
  m_render.vGLTexture.push_back(ldrBMP.loadBMP_custom("..\\data\\buggyboy\\bboy_water.bmp"));



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
///  obj::CGL_ObjectWavefront car(&m_render);
///  car.sObjectFullpath = "..\\data\\virtualroad\\LowPoly_Car\\CBRed_loadBMP.obj";
///  car.Load(0.04f, 0.0f, Vec3f(-5.0f, -1.0f, 0.0f)); // scaled
//  car.sObjectFullpath = "..\\data\\conference_room\\conference.obj";
//  car.Load(0.003f, 0.0f, Vec3f(-5.0f, -1.0f, 0.0f)); // scaled
//  vObjects.push_back(car);


//  obj::CGL_ObjectWavefront car2(&m_render);
//  car2.sObjectFullpath = "..\\data\\virtualroad\\conticar4.obj";
//  car2.Load(1.0f, 0.0f, Vec3f(20.0f, 6.0f, 1.1f)); // scaled


  obj::CGL_ObjectWavefront car3(&m_render);
  car3.sObjectFullpath = "..\\data\\virtualroad\\Jeep\\Jeep.obj";
  car3.Load(0.4f, 0.0f, Vec3f(10.0f, 3.0f, 0.0f)); // scaled



  obj::CGL_ObjectWavefront barrier1(&m_render);
  barrier1.sObjectFullpath = "..\\data\\virtualroad\\barrier\\bboy_barrier3.obj";
  barrier1.Load();
  for (unsigned int ui=1;ui<5;ui++)
  {
    barrier1.PartsToVBOs(Vec3f((float)ui*1.0f, 0.0f, 0.0f));
    barrier1.PartsToVAOs(Vec3f((float)ui*1.0f, 0.0f, 0.0f));
  }


  assert(m_render.vVAOs.size()<m_render.VBOCOUNT);

  return 0;
}

int proj::Proj::DoIt()
{
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




  // --------- IMGUI ---------
  ImGui_ImplWin32_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();
  ImGui::Begin("Papa:");
  ImGui::Text("mouse: %f,%f",io.MousePos.x,io.MousePos.y);
  ImGui::Text("Hallo Anton.");
  float FPS = 1000.0f / (float)timer.getElapsedTimeInMilliSec();
  ImGui::Text("%.1f FPS", FPS);
  static bool b_wireframe;
  ImGui::Checkbox("wireframe:", &b_wireframe);
  for (unsigned int ui = 0; ui < m_render.vVAOs.size(); ui++) m_render.vVAOs[ui].b_Wireframe = (int)b_wireframe;
//  static int vw;
  ImGui::SliderFloat("view width", &(float)m_render.p_cam->zFar, 10.0, 200.0);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  // --------- IMGUI ---------


  SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

  return 0;
}
