// nebulus.cpp : Definiert den Einstiegspunkt f�r die Anwendung.
//

#include "stdafx.h"
#include "nebulus.h"
#include <windows.h>                            // Header File For Windows
#include <windowsx.h>                           // GET_X_LPARAM, GET_Y_LPARAM

#define GLEW_STATIC
#include <GL/glew.h>

#include "camera.hpp"
#include "proj.h"

#include <process.h> // _beginthread

#define MAX_LOADSTRING 100

// Globale Variablen:
HINSTANCE        hInst;                         // Aktuelle Instanz
TCHAR            szTitle[MAX_LOADSTRING];       // Titelleistentext
TCHAR            szWindowClass[MAX_LOADSTRING]; // Klassenname des Hauptfensters

// Vorw�rtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM             MyRegisterClass(HINSTANCE hInstance);
BOOL             InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

HGLRC            hRC=NULL;                      // Permanent Rendering Context
HDC              hDC=NULL;                      // Private GDI Device Context
HWND             hWnd=NULL;                     // Holds Our Window Handle

// render.positionBuffer ---> GLuint idVBO;
proj::Proj m_proj;

Camera m_cam;

int win_h,win_w;
int mouse_x,mouse_y;

int iT = 1;
bool bCamStickToCar = FALSE;

static float framesPerSecond = 0.0f;            // This will store our fps
static float lastTime = 0.0f;                   // This will hold the time from the last frame
//template <typename T> std::string tostr(const T& t) { std::ostringstream os; os<<t; return os.str(); }

bool b_WM_resized = false;

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
    while (true)
    {
        if (b_WM_resized)
        {
            m_proj.m_render.ReSizeGLScene(win_w,win_h);
            b_WM_resized = false;
        }

		if (bCamStickToCar)
		{
			// Camera fixed to vehicle
			glm::vec3 vVehDirNorm = glm::normalize(m_proj.m_moving[1].direction);
			glm::float32 fZtmp = m_cam.Pos[2];
			m_cam.Pos = m_proj.m_moving[1].position - vVehDirNorm*12.0f;
			m_cam.Pos[2] = fZtmp;
			m_cam.At = m_proj.m_moving[1].position;
		}
		else
		{
			// Camera user controlled
//			m_proj.m_render.get_xyz_Hack(iT,m_cam.Pos[0],m_cam.Pos[1],m_cam.Pos[2],m_cam.At[0],m_cam.At[1],m_cam.At[2]);
		}


		// mouse-move camera
		m_cam.width = win_w;
		m_cam.height = win_h;
		m_cam.Move_by_Mouse(glm::vec2(mouse_x, mouse_y));
		
		
		m_cam.updateView(); // View = Pos,At,Norm

        m_proj.DoIt(); // render code
    }
    _endthread();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Hier Code einf�gen.
    MSG msg;
    HACCEL hAccelTable;

    // Globale Zeichenfolgen initialisieren
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);    // Fenstertitel etc. siehe .rc
    LoadString(hInstance, IDC_GLSHOOT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	// Anwendungsinitialisierung ausf�hren:
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

	m_proj.m_scene.c_Scene = "C:\\__glShoot_Files\\editor.scene";
	m_proj.m_scene.c_Cfg   = "C:\\__glShoot_Files\\editor.cfg";

    m_proj.m_render.width = win_w;
    m_proj.m_render.height = win_h;
    hDC = m_proj.m_render.GL_attach_to_DC(hWnd); // <== NeHe    

    glewInit(); // <-- takes a little time

    m_proj.Init();	// <-- Texture erst nach glewInit() laden!!
					// a) data loading + b) data description c) render.Init()

	// erst hier...
	m_cam.p_MVPMatrixAttrib = &m_proj.m_render.MVPMatrixAttrib;
    m_proj.m_render.get_xyz_Hack(iT,m_cam.Pos[0],m_cam.Pos[1],m_cam.Pos[2],m_cam.At[0],m_cam.At[1],m_cam.At[2]);
	m_cam.Pos[2] = 4.0f;
	m_cam.changeAspect(win_w, win_h); // be sure that extrinsics (Pos,At) are filled here
	m_cam.init_MVP();
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

    return (int) msg.wParam;
}

//
//  FUNKTION: MyRegisterClass()
//
//  ZWECK: Registriert die Fensterklasse.
//
//  KOMMENTARE:
//
//    Sie m�ssen die Funktion verwenden, wenn Sie m�chten, dass der Code
//    mit Win32-Systemen kompatibel ist, bevor die RegisterClassEx-Funktion
//    zu Windows 95 hinzugef�gt wurde. Der Aufruf der Funktion ist wichtig,
//    damit die kleinen Symbole, die mit der Anwendung verkn�pft sind,
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
// s.o.  HWND hWnd;

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
//  WM_COMMAND	- Verarbeiten des Anwendungsmen�s
//  WM_PAINT	- Zeichnen des Hauptfensters
//  WM_DESTROY	- Beenden-Meldung anzeigen und zur�ckgeben
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
    case WM_MOUSEWHEEL: // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645617(v=vs.85).aspx
        zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta > 0) m_cam.Pos.z += 0.5; else m_cam.Pos.z -= 0.5; 
          break;
    case WM_MOUSEMOVE:
        // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645616(v=vs.85).aspx
        pt_x = GET_X_LPARAM(lParam); // LOWORD u. HIWORD fkt. nicht bei mehreren Monitoren!
        pt_y = GET_Y_LPARAM(lParam);
        mouse_x = pt_x;
        mouse_y = pt_y;
        break;
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Men�auswahl bearbeiten:
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
		case 37: // ARROW-LEFT
			if (bCamStickToCar)
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
			if (bCamStickToCar)
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
			m_proj.m_render.iWireframe = 1-m_proj.m_render.iWireframe;
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

// Meldungshandler f�r Infofeld.
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
