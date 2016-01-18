/************************************************************************
* Wrapper of general win32 window.
* A common procedure of creating a window is :
* 1. GD::CWindow m_wc;
* 2. m_wc.Init();
* 3. m_wc.Update();
* //  [1/4/2016 Zhaoyu.Wang]                                                                  
************************************************************************/
#include "win32Utility.h"
#include "tchar.h"

using namespace GD;

/**
 *	Use GetTickCount() instead of timeGetTime()
 *	//  [1/6/2016 Zhaoyu.Wang]
 *	*/
//#pragma comment (lib, "winmm.lib")

CWindow::CWindow(HINSTANCE hInstance, PFuncWndProc WndProc)
{
	m_wc.cbClsExtra = 0;
	m_wc.cbWndExtra = 0;
	m_wc.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	m_wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	m_wc.hIcon = (HICON)::LoadImage(nullptr, _T("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	m_wc.hInstance = hInstance;
	m_wc.lpfnWndProc = WndProc;
	m_wc.lpszClassName = _T("DefaultClassName");
	m_wc.lpszMenuName = _T("DefaultMenuName");
	m_wc.style = CS_HREDRAW | CS_VREDRAW;
}

bool CWindow::Init(PFuncWndProc wndProc)
{
	if (!::RegisterClass(&m_wc))
	{
		::MessageBox(nullptr, _T("RegisterClass()-Failed"), _T("Error"), MB_OK);
		return false;
	}

	m_hwnd = ::CreateWindow(m_wc.lpszClassName, _T("GD Engine"), WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, nullptr, nullptr, m_wc.hInstance, nullptr);

	return true;
}

bool CWindow::Init(PFuncWndProc wndProc, LPCWSTR windowName, int windowWidth, int windowHeight)
{
	if (!::RegisterClass(&m_wc))
	{
		::MessageBox(nullptr, _T("RegisterClass()-Failed"), _T("Error"), MB_OK);
		return false;
	}

	m_hwnd = :: CreateWindow(m_wc.lpszClassName, windowName, WS_OVERLAPPEDWINDOW, 0, 0, windowWidth, windowHeight, nullptr, nullptr, m_wc.hInstance, nullptr);

	return true;
}

void CWindow::Update(PFuncDisplay display, PFuncGamePlay runGame)
{
	/**
	 *	Maybe the nCmdShow parameter should be used here?
	 *	*/
	::ShowWindow(m_hwnd, SW_SHOW);
	::UpdateWindow(m_hwnd);
	EnterMsgLoop(display, runGame);
}

bool CWindow::EnterMsgLoop(PFuncDisplay display, PFuncGamePlay runGame)
{
	static MSG msg{0};
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			static DWORD prevTime = 0.0f;
			static DWORD currTime = 0.0f;
			currTime = ::GetTickCount();
			float deltaTime = float(currTime - prevTime);
			if (deltaTime >= GD::DISPLAY_INTERVAL)
			{
				/**
				 *	 Update the screen with 60FPS
				 *	 */
				prevTime = currTime;

				/**
				 *	May something be preProcessed here?
				 *	*/
				display(deltaTime);
			}
			else
			{
				/**
				 *	 Deal with game logic while not rendering
				 *	 */
				runGame(deltaTime);
			}
		}
	}
	return true;
}

HWND CWindow::GetHwnd()
{
	return m_hwnd;
}

