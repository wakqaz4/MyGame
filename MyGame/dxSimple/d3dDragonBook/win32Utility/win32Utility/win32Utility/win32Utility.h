/************************************************************************
* Header file of simple wrapper of win32 APIs                                                                  
************************************************************************/
#ifndef _WIN32_UTILITY
#define _WIN32_UTILITY

#include <windows.h>

namespace GD
{
	/**
	 *	Update the frame every 16.67 ms
	 *	*/
	const float DISPLAY_INTERVAL = 16.67f;

	typedef LRESULT (CALLBACK *PFuncWndProc)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	typedef void (*PFuncDisplay)(float deltaTime);
	typedef void (*PFuncGamePlay)(float deltaTime);

	class CWindow
	{
	public:
		
		CWindow(HINSTANCE, PFuncWndProc);
		CWindow(WNDCLASS& wc){ m_wc = wc; }
		~CWindow()
		{ 
			UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
		}

		bool Init(PFuncWndProc wndProc);		
		bool Init(PFuncWndProc wndProc, LPCWSTR windowName, int windowWidth, int windowHeight);
		void Update(PFuncDisplay display, PFuncGamePlay runGame);
		bool EnterMsgLoop(PFuncDisplay display, PFuncGamePlay runGame);	
		HWND GetHwnd();

	private:
		WNDCLASS m_wc;
		HWND m_hwnd;
	private:
		CWindow(){};
	};
}

#endif