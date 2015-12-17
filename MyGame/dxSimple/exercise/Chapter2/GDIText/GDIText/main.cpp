/************************************************************************
* Include header files                                                                  
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>

/************************************************************************
* Include lib files (Windows multi media)                                                                 
************************************************************************/
#pragma comment(lib, "winmm.lib")


/************************************************************************
* Declare external functions                                                                  
************************************************************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
bool GameInit(HWND);
void GamePaint(HWND);
bool GameClearup(HWND);

/************************************************************************
* Declare global vars                                                                  
************************************************************************/
HDC gHdc;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(0, IDC_ARROW);
	wndClass.hIcon = (HICON)LoadImage(nullptr, _T("icon.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = _T("GDIText");
	wndClass.lpszMenuName = _T("WhateverMenu");
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(nullptr, _T("Creating class failed!"), _T("Error!"), MB_OK);
		return -1;
	}

	HWND hwnd = CreateWindow(_T("GDIText"), _T("WindowName"),
		WS_OVERLAPPEDWINDOW,
		0, 0,
		800, 600,
		nullptr, nullptr,
		hInstance, nullptr);

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
		else
		{

		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	
	switch (message)
	{
	case WM_CREATE:
		GameInit(hwnd);
		break;
	case WM_PAINT:
		gHdc = BeginPaint(hwnd, &ps);
		GamePaint(hwnd);
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

bool GameInit(HWND hwnd)
{
	srand((unsigned)time(nullptr));
	gHdc = GetDC(hwnd);
	GamePaint(hwnd);
	ReleaseDC(hwnd, gHdc);
	return true;
}
void GamePaint(HWND hwnd)
{
	//HFONT hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, L"微软雅黑");  //创建一种字体
	//SelectObject(gHdc, hFont);
	//SetBkMode(g_hdc, TRANSPARENT);    //设置输出文字背景色为透明
	WCHAR text1[] = _T("Line one");
	WCHAR text2[] = _T("two on the Line!");
	WCHAR text3[] = _T("第三行");

	SetTextColor(gHdc, RGB(rand() % 256, rand() % 256, rand() % 256));
	TextOut(gHdc, 30, 30, text1, wcslen(text1));

	SetTextColor(gHdc, RGB(rand() % 256, rand() % 256, rand() % 256));
	TextOut(gHdc, 30, 100, text2, wcslen(text2));
	
	SetTextColor(gHdc, RGB(rand() % 256, rand() % 256, rand() % 256));
	TextOut(gHdc, 30, 200, text3, wcslen(text3));

	//DeleteObject(hFont);
}

bool GameClearup(HWND hwnd)
{
	return true;
}