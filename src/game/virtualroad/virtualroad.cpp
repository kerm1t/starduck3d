// nebulus.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "stdafx.h"
#include "virtualroad.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include <windows.h>                            // Header File For Windows
#include <windowsx.h>                           // GET_X_LPARAM, GET_Y_LPARAM

#define GLEW_STATIC
#include <GL/glew.h>

#include <process.h> // _beginthread

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

proj::Proj m_proj;

Camera m_cam;

int win_h,win_w;
int mouse_x,mouse_y;

int iT = 1;
bool bCamStickToTrack = FALSE;

static float framesPerSecond = 0.0f;            // This will store our fps
static float lastTime = 0.0f;                   // This will hold the time from the last frame

bool b_WM_resized = false;
bool b_program_stopped = false;

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
void RenderThread(void *args)
{
  while ((true) && (!b_program_stopped)) // do not interfere with freeing of ressources (Imgui, ...)
  {
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeysDown[65] == true) // A
    {
      std::cout << "A pressed" << std::endl;
    }
    if (GetAsyncKeyState(VK_UP))
    {
      m_cam.MoveFwd();
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
      m_cam.MoveBack();
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
      m_cam.TurnLeft();
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
      m_cam.TurnRight();
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
      glm::vec3 vVehDirNorm = glm::normalize(m_proj.m_moving[1].direction);
      glm::float32 fZtmp = m_cam.Pos[2];
      m_cam.Pos    = m_proj.m_moving[1].position - vVehDirNorm*12.0f;
      m_cam.Pos[2] = fZtmp;
      m_cam.At     = m_proj.m_moving[1].position;
    }
    else
    {
      // Camera user controlled
//      m_proj.m_render.get_xyz_Hack(iT,m_cam.Pos[0],m_cam.Pos[1],m_cam.Pos[2],m_cam.At[0],m_cam.At[1],m_cam.At[2]);

      m_cam.change_Aspect(win_w,win_h);
      // mouse-move camera
      m_cam.Look_with_Mouse(glm::vec2(mouse_x, mouse_y));
    }

    m_cam.update_View(); // View = Pos,At,Norm

    m_proj.DoIt(); // render code
  }
  _endthread();

  if (b_program_stopped)
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
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


  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  ImGui_ImplWin32_Init(hWnd);


  // ---------------- register mouse ----------------
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
  // ---------------- register mouse ----------------



  m_proj.m_scene.c_Scene = "..\\data\\virtualroad\\editor.scene";
  m_proj.m_scene.c_Cfg = "..\\data\\virtualroad\\editor.cfg";

  m_proj.m_render.width = win_w;
  m_proj.m_render.height = win_h;
  hDC = m_proj.m_render.GL_attach_to_DC(hWnd); // <== NeHe    

  glewExperimental = GL_TRUE; // <-- Nutzen?
  glewInit(); // <-- takes a little time

  m_proj.Init();	// <-- Texture erst nach glewInit() laden!!
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

  wcex.style			= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= WndProc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= hInstance;
  wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GLSHOOT));
  wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GLSHOOT);
  wcex.lpszClassName	= szWindowClass;
  wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

//
//  FUNKTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ZWECK:  Verarbeitet Meldungen vom Hauptfenster.
//
//  WM_COMMAND	- Verarbeiten des Anwendungsmenüs
//  WM_PAINT	- Zeichnen des Hauptfensters
//  WM_DESTROY	- Beenden-Meldung anzeigen und zurückgeben
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  int zDelta;
  int pt_x,pt_y;

  glm::vec3 vVehDirNorm,vVehDirOrth;

  switch (message)
  {
    // ----------------------------------------------------------------------------
    // problem: we need relative mouse movement, that's why we utilize WM_INUT here
    // s. https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/taking-advantage-of-high-dpi-mouse-movement
    // good explaination here:
    // s. https://gamedev.stackexchange.com/questions/52976/exclusive-mouse-movement-with-wm-input
    // there is DirectX / DirectInput, but that requires additional "weapons" we don't want to use here
    // ----------------------------------------------------------------------------
  case WM_INPUT:
  {
    UINT dwSize = 40;
    static BYTE lpb[40];
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
    RAWINPUT* raw = (RAWINPUT*)lpb;
    if (raw->header.dwType == RIM_TYPEMOUSE)
    {
      int xPosRelative = raw->data.mouse.lLastX;
      int yPosRelative = raw->data.mouse.lLastY;
      mouse_x += xPosRelative;
      mouse_y += yPosRelative;
    }
    break;
  }
  case WM_MOUSEWHEEL: // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645617(v=vs.85).aspx
    zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (zDelta > 0) m_cam.Pos.z += 0.5; else m_cam.Pos.z -= 0.5; 
    break;
  case WM_MOUSEMOVE:
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645616(v=vs.85).aspx
    pt_x = GET_X_LPARAM(lParam); // LOWORD u. HIWORD fkt. nicht bei mehreren Monitoren!
    pt_y = GET_Y_LPARAM(lParam);
//    mouse_x = pt_x;
//    mouse_y = pt_y;
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
    //	case WM_PAINT:
    // ... painting by OpenGL
  case WM_KEYDOWN:
    switch (wParam)
    {
    case 32: // Space
      m_proj.m_render.b_splash_screen = !m_proj.m_render.b_splash_screen;
      break;
    case 37: // ARROW-LEFT
      if (bCamStickToTrack)
      {
        vVehDirNorm = glm::normalize(m_proj.m_moving[1].direction);
        vVehDirOrth = glm::vec3(-vVehDirNorm[1],vVehDirNorm[0],vVehDirNorm[2]);
        m_proj.m_moving[1].Move(vVehDirOrth*0.2f);
      }
      else
      {
        if (iT >= 1) iT--; else iT = m_proj.m_scene.trajectory_len-2;
      }
      break;
    case 39: // ARROW-RIGHT
      if (bCamStickToTrack)
      {
        vVehDirNorm = glm::normalize(m_proj.m_moving[1].direction);
        vVehDirOrth = glm::vec3(-vVehDirNorm[1],vVehDirNorm[0],vVehDirNorm[2]);
        m_proj.m_moving[1].Move(-vVehDirOrth*0.2f);
      }
      else
      {
        if (iT < m_proj.m_scene.trajectory_len-2) iT++; else iT = 0;
      }
      break;
    case 80: // P >> Pause ON/OFF
      m_proj.bPause = !(m_proj.bPause);
      break;
    case 87: // W
//      m_proj.m_render.iWireframe = 1-m_proj.m_render.iWireframe;
      m_cam.MoveFwd();
      break;
    case 65: // A
      m_cam.TurnLeft();
      m_cam.StrafeLeft();
      break;
    case 83: // S
      m_cam.MoveBack();
      break;
    case 68: // D
      m_cam.StrafeRight();
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
