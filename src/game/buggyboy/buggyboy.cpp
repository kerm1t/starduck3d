// nebulus.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "stdafx.h"
#include "buggyboy.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include <windows.h>                            // Header File For Windows
#include <windowsx.h>                           // GET_X_LPARAM, GET_Y_LPARAM

#define GLEW_STATIC
#include <GL/glew.h>

#include <process.h> // _beginthread

#include <iostream>  // file io
#include <fstream>

#include <typeinfo>

//#include "img_bitmap4.hpp" // text overlay

#define MAX_LOADSTRING 100

// Globale Variablen:
HINSTANCE        hInst;                         // Aktuelle Instanz
TCHAR            szTitle[MAX_LOADSTRING];       // Titelleistentext
TCHAR            szWindowClass[MAX_LOADSTRING]; // Klassenname des Hauptfensters

// Vorwärtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM             MyRegisterClass(HINSTANCE hInstance);
BOOL             InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

HGLRC            hRC=NULL;                      // Permanent Rendering Context
HDC              hDC=NULL;                      // Private GDI Device Context
HWND             hWnd=NULL;                     // Holds Our Window Handle

// 2do: think about add variables to which classes

proj::Proj m_proj;

Camera m_cam; // 2do: besser dort als m_cam und hier p_cam-> m_proj.m_render.p_cam

int win_h,win_w;
int mouse_x,mouse_y;
int mouse_left, mouse_right;

int iT = 1;
bool bCamStickToTrack = FALSE;

static float framesPerSecond = 0.0f;            // This will store our fps
static float lastTime = 0.0f;                   // This will hold the time from the last frame

bool b_WM_resized = false;
bool b_program_stopped = false;
bool b_add_obj = false;

bool b_del_obj = false;
int i_del_obj = -1;

bool b_exit_done = false;

bool b_test = false;

#define ED_OBJ_BARRIER      1
#define ED_OBJ_BB_BANNER    2
#define ED_OBJ_BB_FLAG      3
#define ED_OBJ_BB_WOODPILE  4
#define ED_OBJ_BB_CONCRETE  5
#define ED_OBJ_BB_TREE      6
int editor_Obj = ED_OBJ_BB_BANNER;

void CalculateFrameRate()
{
  float currentTime = GetTickCount() * 0.001f;
  ++framesPerSecond;
  if( currentTime - lastTime > 1.0f )
  {
    lastTime = currentTime;
    framesPerSecond = 0.0;
  }
}

// OpenGL calls moved to own thread
// s. http://stackoverflow.com/questions/9833852/opengl-game-loop-multithreading
// ... this also means that you can't call any OpenGL functions from the windowing thread
void RenderThread(void *args)
{
  // Renderloop now
  while ((true) && (!b_program_stopped)) // do not interfere with freeing of ressources (Imgui, ...)
  {


    // ===== output Text to Overlay =====
    if (m_proj.gamestate == proj::gsPlay)
    {
      s_bmp4 bmp4; // overlay
      CBMP4 BMP4;
      BMP4.BMP(bmp4, 100, 80);                           // create empty bitmap
      std::string s = std::to_string(m_proj.score);
      m_proj.fnt.word(s, bmp4, 10, 0);
      BMP4.BMP_texID(bmp4, m_proj.id_tex_overlay);       // bmp to texture now
      delete bmp4.data;
    }
    else if (m_proj.gamestate == proj::gsHelp)
    {
      s_bmp4 bmp4; // overlay
      CBMP4 BMP4;
      BMP4.BMP(bmp4, 200, 150);                          // create empty bitmap
      std::string s = std::to_string(m_proj.score);
      m_proj.fnt.word("HELP", bmp4, 10, 0);
      m_proj.fnt.word("1-BARRIER", bmp4, 10, 16);
      m_proj.fnt.word("2-BANNER", bmp4, 10, 32);
      m_proj.fnt.word("3-FLAG", bmp4, 10, 48);
      m_proj.fnt.word("4-WOODPILE", bmp4, 10, 64);
      m_proj.fnt.word("5-CONCRETE", bmp4, 10, 80);
      m_proj.fnt.word("6-TREE", bmp4, 10, 96);
      m_proj.fnt.word("Y-DELETE", bmp4, 10, 112);
      m_proj.fnt.word("Z-SAVE", bmp4, 10, 128);
      BMP4.BMP_texID(bmp4, m_proj.id_tex_overlay);       // bmp to texture now
      delete bmp4.data;
    }
    // ===== output Text to Overlay =====



    if (b_add_obj)
    {
      int nVAOs = m_proj.m_render.vVAOs.size();

      // place objects orthogonal to viewing direction
      glm::vec3 cam_dir = m_proj.m_render.p_cam->At - m_proj.m_render.p_cam->Pos;
      glm::vec3 obj_dir = -cam_dir;
      obj_dir.z = 0.0f; // being used for obj's BBox, thus z-comp always same
      glm::vec3 obj_pos = glm::vec3(m_proj.m_render.Cursor.x, m_proj.m_render.Cursor.y, m_proj.m_render.p_cam->Pos.z-CAM_Z); // cursor is just a little bit ahead of the camera pos.

      if ((editor_Obj == ED_OBJ_BB_BANNER) ||
          (editor_Obj == ED_OBJ_BB_FLAG) ||
          (editor_Obj == ED_OBJ_BB_WOODPILE) ||
          (editor_Obj == ED_OBJ_BB_CONCRETE) ||
          (editor_Obj == ED_OBJ_BB_TREE))
      {
        obj::CBillboard* bb = new obj::CBillboard;
        bb->p_render = &m_proj.m_render;
        proj::c_VAO vao;
        if      (editor_Obj == ED_OBJ_BB_BANNER)   vao = bb->Create("Banner", "tx_Banner", obj_pos, obj_dir);
        else if (editor_Obj == ED_OBJ_BB_FLAG)     vao = bb->Create("Flag", "tx_Flag", obj_pos, obj_dir, 0.9, 1.5);
        else if (editor_Obj == ED_OBJ_BB_WOODPILE) vao = bb->Create("Woodpile", "tx_Woodpile", obj_pos, obj_dir, 1.4, 1.4);
        else if (editor_Obj == ED_OBJ_BB_CONCRETE) vao = bb->Create("Concrete", "tx_Concrete", obj_pos, obj_dir);
        else /*if (editor_Obj == ED_OBJ_BB_TREE)*/ vao = bb->Create("Tree", "tx_Tree", obj_pos, obj_dir, 1.0f);
        m_proj.m_render.vVAOs.push_back(vao);
        bb->vVaoID.push_back(nVAOs);
#if(B_ADD_BBOX_VAO == 1)
        bb->vVaoID.push_back(nVAOs + 1); // 2do: easier (add in the obj.Create etc...)
#endif
        m_proj.m_render.Bind_NEW__VBOs_to_VAOs(nVAOs);
        m_proj.vObjects.push_back(bb); // 2do: wieviel Speicherverbrauch?
      }
      if (editor_Obj == ED_OBJ_BARRIER)
      {
        obj::CGL_ObjectWavefront* barrier1 = new obj::CGL_ObjectWavefront(&m_proj.m_render);
        barrier1->sObjectFullpath = "..\\data\\virtualroad\\barrier\\bboy_barrier3.obj";
        barrier1->Load(obj_pos, obj_dir, 1.0f, 0.0f);
        barrier1->vVaoID.push_back(nVAOs);
#if(B_ADD_BBOX_VAO == 1)
        barrier1->vVaoID.push_back(nVAOs + 1); // 2do: easier (add in the obj.Create etc...)
#endif
        m_proj.m_render.Bind_NEW__VBOs_to_VAOs(nVAOs);
        m_proj.vObjects.push_back(barrier1); // 2do: wieviel Speicherverbrauch?
      }

      b_add_obj = false;
    } // if (b_add_obj)

    if ((b_del_obj) && (i_del_obj > -1))
    {
      // a) remove from v_object
      // b) remove vao(s): i) obj, ii) bbox, iii) parts
      for (int i = 0; i < m_proj.vObjects[i_del_obj]->vVaoID.size(); i++)
      {
        int vaoID = m_proj.vObjects[i_del_obj]->vVaoID[i];
        m_proj.m_render.vVAOs[vaoID].b_doDraw = false;
      }
//      int vaoID = m_proj.vObjects[i_del_obj]->vaoID;
//      m_proj.m_render.vVAOs[vaoID].b_doDraw = false;
//      if (typeid(m_proj.vObjects[i_del_obj]) == typeid(obj::CGL_ObjectParts))
//      {
//      }
      b_del_obj = false;
    }

    float slowdown = 15.0f;
    if (GetAsyncKeyState(VK_UP))
    {
      m_cam.MoveFwd(1.0f / slowdown);
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
      m_cam.MoveBack(1.0f / slowdown);
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
      m_cam.StrafeLeft(1.0f / 30.0f);
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
      m_cam.StrafeRight(1.0f / 30.0f);
    }

    if (b_WM_resized)
    {
      m_proj.m_render.ReSizeGLScene(win_w,win_h);
      b_WM_resized = false;
    }

    // 2do: mouse-move camera

    if (bCamStickToTrack)
    {
      // Camera fixed to vehicle
    }
    else
    {
      // Camera user controlled
      m_cam.change_Aspect(win_w,win_h);
      // mouse-move camera
      m_cam.Look_with_Mouse(glm::vec2(mouse_x, -5.0f + (float)mouse_y/(float)win_h*10.0f));
    }

    m_cam.update_View(); // View = Pos,At,Norm

    m_proj.DoIt(); // render code
  }
  _endthread();

  // Exit(), s. LRESULT CALLBACK WndProc(...)
}

void Exit()
{
//  if (b_program_stopped)
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
///?    ImGui::DestroyContext();
    m_proj.Exit();
    b_exit_done = true;
  }
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO: Hier Code einfügen.
  MSG msg;
  HACCEL hAccelTable;

  // Globale Zeichenfolgen initialisieren
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);    // Fenstertitel etc. siehe .rc
  LoadString(hInstance, IDC_GLSHOOT, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Anwendungsinitialisierung ausführen:
  if (!InitInstance (hInstance, nCmdShow))
  {
    return FALSE;
  }

  RECT rect;
  if(GetWindowRect(hWnd, &rect))
  {
    win_w = rect.right - rect.left;
    win_h = rect.bottom - rect.top;
  }
  mouse_x = (int)(win_w/2.0f); // init
  mouse_y = (int)(win_h/2.0f);


  // Application init


  // (1) init ImGui, s. imgui.cpp
  // (2) "draw" ImGui elements --> s. proj.cpp DoIt()
  // (3) close ImGui --> s. RenderThread()
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  ImGui_ImplWin32_Init(hWnd);



  // ---------------- register mouse (for relative movement) ----------------
  // s. https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/taking-advantage-of-high-dpi-mouse-movement
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
  RAWINPUTDEVICE Rid[1];
  Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
  Rid[0].dwFlags = RIDEV_INPUTSINK;
  Rid[0].hwndTarget = hWnd;
  RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
  // ---------------- register mouse (for relative movement) ----------------



  m_proj.m_scene.c_Scene = "..\\data\\buggyboy\\editor.scene";
  m_proj.m_scene.c_Cfg   = "..\\data\\buggyboy\\editor.cfg";

  m_proj.m_render.width = win_w;
  m_proj.m_render.height = win_h;
  hDC = m_proj.m_render.GL_attach_to_DC(hWnd); // <== NeHe    

  glewExperimental = GL_TRUE; // <-- Nutzen?
  glewInit(); // <-- takes a little time

  m_proj.Init();  // <-- Texture erst nach glewInit() laden!!
                  // a) data loading + b) data description c) render.Init()

  // erst hier...
  m_cam.p_unif_MVPMatrix = &m_proj.m_render.sh1_unif_MVPMatrix;
  m_cam.change_Aspect(win_w, win_h); // be sure that extrinsics (Pos,At) are filled here
  m_cam.init_MVP(); // ! :-)
  m_proj.m_render.p_cam = &m_cam;

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GLSHOOT));

  _beginthread(RenderThread, 0, 0);

  // Hauptnachrichtenschleife:
  while (GetMessage(&msg, NULL, 0, 0))
  {
    wglMakeCurrent(NULL,NULL); // <-- no other access to OpenGL here!! --> only in RenderThread 

    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // --> Projection moved to RenderThread, otherwise no autonomous object-movement possible
  }

  b_program_stopped = true; // === Program stopped ===> clean up ... (s. Renderthread)

  return (int) msg.wParam;
}

//
//  FUNKTION: MyRegisterClass()
//
//  ZWECK: Registriert die Fensterklasse.
//
//  KOMMENTARE:
//
//    Sie müssen die Funktion verwenden, wenn Sie möchten, dass der Code
//    mit Win32-Systemen kompatibel ist, bevor die RegisterClassEx-Funktion
//    zu Windows 95 hinzugefügt wurde. Der Aufruf der Funktion ist wichtig,
//    damit die kleinen Symbole, die mit der Anwendung verknüpft sind,
//    richtig formatiert werden.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInstance;
  wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GLSHOOT));
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName  = MAKEINTRESOURCE(IDC_GLSHOOT);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassEx(&wcex);
}

//
//   FUNKTION: InitInstance(HINSTANCE, int)
//
//   ZWECK: Speichert das Instanzenhandle und erstellt das Hauptfenster.
//
//   KOMMENTARE:
//
//        In dieser Funktion wird das Instanzenhandle in einer globalen Variablen gespeichert, und das
//        Hauptprogrammfenster wird erstellt und angezeigt.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  //   HWND hWnd;

  hInst = hInstance; // Instanzenhandle in der globalen Variablen speichern

  hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

int ImGui_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (ImGui::GetCurrentContext() == NULL)
    return 0;

  ImGuiIO& io = ImGui::GetIO();
  switch (msg)
  {
  case WM_INPUT:
  {
    if (!io.WantCaptureMouse)
    {
      UINT dwSize = 40;
      static BYTE lpb[40];
      GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
      RAWINPUT* raw = (RAWINPUT*)lpb;
      if (raw->header.dwType == RIM_TYPEMOUSE)
      {
        int xPosRelative = raw->data.mouse.lLastX;
        int yPosRelative = raw->data.mouse.lLastY;
        mouse_x += xPosRelative; // we want this to be unlimited for rotation
//        mouse_y += yPosRelative; ... we want this to be limited by the upper and lower screen, as used to look up / down
                                 //    that is why this is set in regular WndProc() routine
        mouse_y += yPosRelative; //... we want this to be limited by the upper and lower screen, as used to look up / down

//        if ((xPosRelative > 0) && (yPosRelative > 0)) // hack! has to be inside Window!
                                                      //       yet (0,0) cannot be klicked now, maybe fix that later
        {
          // https://www.gamedev.net/forums/topic/655198-raw-input-mouse-problem/
          // Mouse Left: Add Object only, when not over IMGui element while clicking
          bool downState = (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) > 0;
          bool upState = (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) > 0;
          if (downState == true && upState == false)
          {
            mouse_left = 1;
          }
          if (upState == true)
          {
            mouse_left = 0;
          }
          if (mouse_left == 1)
          {
            // hat nicht funktioniert, hier Objekte hinzuzufügen.
            // habe jetzt in den Renderthread verschoben, klack, schon hat's funktioniert
            b_add_obj = true;
          }
          // Mouse Left: Add Object only, when not over IMGui element while clicking
        }
      }
    }
    break;
  }
  case WM_MOUSEMOVE:
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645616(v=vs.85).aspx
    int pt_x, pt_y;
    pt_x = GET_X_LPARAM(lParam); // LOWORD u. HIWORD fkt. nicht bei mehreren Monitoren!
    pt_y = GET_Y_LPARAM(lParam);
    io.MousePos = ImVec2((float)pt_x, (float)pt_y);
    if (!io.WantCaptureMouse)
    {
//      mouse_y = pt_y;
    }
    break;
  case WM_LBUTTONDOWN:
    mouse_left = 1;
    io.MouseDown[0] = true;
    break;
  case WM_LBUTTONUP:
    mouse_left = 0;
    io.MouseDown[0] = false;
    break;
  }
  return 1;
}

//
//  FUNKTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ZWECK:  Verarbeitet Meldungen vom Hauptfenster.
//
//  WM_COMMAND  - Verarbeiten des Anwendungsmenüs
//  WM_PAINT    - Zeichnen des Hauptfensters
//  WM_DESTROY  - Beenden-Meldung anzeigen und zurückgeben
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  int zDelta;

  glm::vec3 vVehDirNorm,vVehDirOrth;

  ImGui_WndProcHandler(hWnd, message, wParam, lParam);

  switch (message)
  {
    // ----------------------------------------------------------------------------
    // problem: we need relative mouse movement, that's why we utilize WM_INUT here
    // s. https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/taking-advantage-of-high-dpi-mouse-movement
    // good explaination here:
    // s. https://gamedev.stackexchange.com/questions/52976/exclusive-mouse-movement-with-wm-input
    // there is DirectX / DirectInput, but that requires additional "weapons" we don't want to use here
    // ----------------------------------------------------------------------------
//  in ImGUI WndProcHandler gezogen, dort lässt sich mittels WantCaptureMouse der Fokus steuern
//  case WM_INPUT: [...]
  case WM_MOUSEWHEEL: // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645617(v=vs.85).aspx
    zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (zDelta > 0) m_cam.Pos.z += 0.5; else m_cam.Pos.z -= 0.5; 
    break;
  case WM_MOUSEMOVE:
    break;
  case WM_LBUTTONDOWN:
    mouse_left = 1;
    // OpenGL (add objects) only in Renderthread
    break;
  case WM_LBUTTONUP:
    mouse_left = 0;
    break;
  case WM_COMMAND:
    wmId    = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    // Menüauswahl bearbeiten:
    switch (wmId)
    {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
//  case WM_PAINT: ... painting done by OpenGL
  case WM_KEYDOWN:
    switch (wParam)
    {
    case 27: // ESC
      m_proj.b_show_debug = !m_proj.b_show_debug;
      break;
    case 32: // Space
//      m_proj.m_render.b_splash_screen = !m_proj.m_render.b_splash_screen;
      b_test = !b_test;
      break;
    case 37: // ARROW-LEFT
      if (bCamStickToTrack)
      {
      }
      else
      {
        if (iT >= 1) iT--; else iT = m_proj.m_scene.trajectory_len-2;
      }
      break;
    case 39: // ARROW-RIGHT
      if (bCamStickToTrack)
      {
      }
      else
      {
        if (iT < m_proj.m_scene.trajectory_len-2) iT++; else iT = 0;
      }
      break;
    case 49: // 1
    case 97: // 1 NUM
      editor_Obj = ED_OBJ_BARRIER;
      b_add_obj = true;
      break;
    case 50: // 2
    case 98: // 2 NUM
      editor_Obj = ED_OBJ_BB_BANNER;
      b_add_obj = true;
      break;
    case 51: // 3
      editor_Obj = ED_OBJ_BB_FLAG;
      b_add_obj = true;
      break;
    case 52: // 4
      editor_Obj = ED_OBJ_BB_WOODPILE;
      b_add_obj = true;
      break;
    case 53: // 5
      editor_Obj = ED_OBJ_BB_CONCRETE;
      b_add_obj = true;
      break;
    case 54: // 6
      editor_Obj = ED_OBJ_BB_TREE;
      b_add_obj = true;
      break;
    case 72: // h
      if (m_proj.gamestate == proj::gsPlay) m_proj.gamestate = proj::gsHelp; else m_proj.gamestate = proj::gsPlay;
      break;
    case 80: // P >> Pause ON/OFF
      m_proj.bPause = !(m_proj.bPause);
      break;
    case 87: // W
      m_cam.MoveFwd(0.3f);
      break;
    case 65: // A
      m_cam.StrafeLeft(0.1f); // optional:  m_cam.TurnLeft();
      break;
    case 83: // S
      m_cam.MoveBack(0.3f);
      break;
    case 68: // D
      m_cam.StrafeRight(0.1f);
      break;
    case 88: // X
      // save scene
      m_proj.Save_Scene_Objs();
      break;
    case 89: // Y
      // delete latest added object
      b_del_obj = true;
      i_del_obj = m_proj.touch_object_id;
      break;
    }
    break;
  case WM_SIZE:
    win_w = LOWORD(lParam);
    win_h = HIWORD(lParam);
    // resize --> in the thread now, as from here no access to OpenGL-context
    b_WM_resized = true;
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    Exit(); // buggyboy: free mem etc.
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// Meldungshandler für Infofeld.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
