/************************************************************************
* A simple review of Win32.                                                                   
************************************************************************/
#include <windows.h>
#include <tchar.h>

#pragma comment(lib, "winmm.lib")

//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndclass;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
//	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hCursor = (HICON__*)(0x00010003);
	wndclass.hIcon = (HICON)::LoadImage(nullptr, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = _T("ClassName");
	wndclass.lpszMenuName = _T("MenuName");
	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndclass))
	{
		return 0;
	}

	HWND hwnd = CreateWindow(_T("ClassName"), _T("TestWindow"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

	MoveWindow(hwnd, 300, 300, 800, 600, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	

	MSG msg{ 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_CREATE:
		PlaySound(L"FirstBlood.wav", nullptr, SND_FILENAME | SND_ASYNC);
		break;

	case WM_PAINT:
		ValidateRect(hwnd, NULL);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return 0;
}