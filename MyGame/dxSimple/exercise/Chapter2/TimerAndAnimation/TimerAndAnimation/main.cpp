/************************************************************************
* Initialize an timer and keep updating images to form an animation                                                                  
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <iostream>

#pragma comment(lib, "winmm.lib")

#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE		L"【致我们永不熄灭的游戏开发梦想】游戏动画技巧之 定时器动画显示 示例程序"	//为窗口标题定义的宏


/************************************************************************
* Declare external functions and global variables                                                                  
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern VOID CALLBACK OnTimerCallback(HWND hwnd, UINT message, UINT_PTR ptr, DWORD dW);
extern bool GameInit(HWND hwnd);
extern void GamePaint(HWND hwnd);
extern bool GameClearup(HWND hwnd);

HDC gHDC;
HDC gBufferDC;
HBITMAP gBmp[11];
int gTimerCount = 0;
const int G_TIMER_LIMIT = 11;

/************************************************************************
* Main Loop                                                                  
************************************************************************/
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbWndExtra = 0;
	wndClass.cbClsExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = (HICON)LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = _T("TimerAndAnimation");
	wndClass.lpszMenuName = _T("Menu");
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("TimerAndAnimation"), WINDOW_TITLE,				//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		GameInit(hwnd);
		break;

	case WM_TIMER:
		GamePaint(hwnd);
		break;
	case WM_DESTROY:
		GameClearup(hwnd);
		PostQuitMessage(0);
        break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

bool GameInit(HWND hwnd)
{
	SetTimer(hwnd, 1, 100, nullptr);
	PlaySound(_T("Kalafina - believe.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	/**
	 *	Initial DC
	 *	*/
	gHDC = GetDC(hwnd);
	gBufferDC = CreateCompatibleDC(gHDC);

	/**
	 *	Init images to form anim
	 *	*/	
	std::wstring images[11];
    
	for (int i = 0; i < 11; i++)
	{
		images[i] = _T("Loli");
		WCHAR buffer[20];
		wsprintf(buffer, L"%d.bmp", i);
		images[i] += buffer;
		gBmp[i] = (HBITMAP)LoadImage(nullptr, images[i].c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_DEFAULTSIZE);
	}

	
	GamePaint(hwnd);

	return true;
}

//VOID CALLBACK OnTimerCallback(HWND hwnd, UINT message, UINT_PTR ptr, DWORD dW)
//{
//	SelectObject(gBufferDC, gBmp[gTimerCount]);
//	BitBlt(gHDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, gBufferDC, 0, 0, SRCCOPY);
//	if (++gTimerCount >= G_TIMER_LIMIT)
//	{
//		gTimerCount = 0;
//	}
//}

void GamePaint(HWND hwnd)
{
	SelectObject(gBufferDC, gBmp[gTimerCount]);
	BitBlt(gHDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, gBufferDC, 0, 0, SRCCOPY);
	if (++gTimerCount >= G_TIMER_LIMIT)
	{
		gTimerCount = 0;
	}

}
bool GameClearup(HWND hwnd)
{
	KillTimer(hwnd, 01);
	for (int i = 0; i < 11; i++)
	{
		DeleteObject(gBmp[i]);
	}
	DeleteDC(gBufferDC);
	ReleaseDC(hwnd, gHDC);

	return true;
}

