/************************************************************************
* This program implements transparent animation using TransparentBlt.
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

//////////////////////////////////////////////////////////////////////////
// External functions and global vars  
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool GameInit(HWND hwnd);
extern void GameUpdate(HWND hwnd);
extern bool GameExit(HWND hwnd);

HDC gHDC;
HDC gMemoryDC;
HDC gBufferDC;
HBITMAP gBmpBackground;
HBITMAP gBmpSprite;
DWORD gPrevTickCount;
DWORD gCurrTickCount;
int gSpriteIndex = 0;
int gSpritePosX = 0;
const int G_SPRITE_MAX_INDEX = 11;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = (HICON)LoadImage(nullptr, _T("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = _T("CName");
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("CName"), _T("AnimMovingRight"), WS_OVERLAPPEDWINDOW,
		0, 0, 800, 600,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	PlaySound(_T("中島愛、May'n - ライオン.wav"), nullptr, SND_LOOP | SND_ASYNC | SND_FILENAME);

	if (!GameInit(hwnd))
	{
		MessageBox(nullptr, _T("Init Game Failed!"), _T("Error!"), MB_OK);
		return -1;
	}

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
			gCurrTickCount = GetTickCount();
			if (gCurrTickCount - gPrevTickCount >= 200.3f)
			{				
				gPrevTickCount = gCurrTickCount;
				GameUpdate(hwnd);
			}			
		}
	}
	
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		GameExit(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

bool GameInit(HWND hwnd)
{
	gHDC = GetDC(hwnd);
	gMemoryDC = CreateCompatibleDC(gHDC);
	gBufferDC = CreateCompatibleDC(gHDC);
	HBITMAP bmp;
	bmp = CreateCompatibleBitmap(gHDC, 800, 600); //建一个和窗口兼容的空的位图对象

	SelectObject(gMemoryDC, bmp);//将空位图对象放到mdc中

	gBmpBackground = (HBITMAP)LoadImage(nullptr, _T("bg.bmp"), IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	gBmpSprite = (HBITMAP)LoadImage(nullptr, _T("goright.bmp"), IMAGE_BITMAP, 480, 108, LR_LOADFROMFILE);
	
	GameUpdate(hwnd);
	return true;
}

void GameUpdate(HWND hwnd)
{
	SelectObject(gBufferDC, gBmpBackground);
	BitBlt(gMemoryDC, 0, 0, 800, 600, gBufferDC, 0, 0, SRCCOPY);

	SelectObject(gBufferDC, gBmpSprite);
	TransparentBlt(gMemoryDC, gSpritePosX , 400, 60, 108, gBufferDC, gSpriteIndex * 60, 0, 60, 108, RGB(255, 0, 0));

	BitBlt(gHDC, 0, 0, 800, 600, gMemoryDC, 0, 0, SRCCOPY);

	if (++gSpriteIndex >= 8)
	{
		gSpriteIndex = 0;
	}
	gSpritePosX += 10;
	if (gSpritePosX >= 700)
	{
		gSpritePosX = 0;
	}
}

bool GameExit(HWND hwnd)
{
	DeleteObject(gBmpBackground);
	DeleteObject(gBmpSprite);
	DeleteDC(gBufferDC);
	DeleteDC(gMemoryDC);
	ReleaseDC(hwnd, gHDC);
	return true;
}