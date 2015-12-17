/************************************************************************
* Simulates a ball bouncing with gravity, where the ball is replaced with 
* an angry bird for fun!
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

/************************************************************************
* External functions and global variables
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool GameInit(HWND hwnd);
extern void GameUpdate(HWND hwnd, DWORD tickCount);
extern bool GameExit(HWND hwnd);

HDC gHDC;
HDC gMemDC;
HDC gBufDC;
HBITMAP gBmpBackground;
HBITMAP gBmpSprite;
RECT gRect;

DWORD gPrevTickCount;
DWORD gCurrTickCount;

int gPosX = 0;
int gPosY = 0;
int gSpeedX = 0;
int gSpeedY = 0;
int gGravity = 10;

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
			if (gCurrTickCount - gPrevTickCount >= 50.3f)
			{
				GameUpdate(hwnd, gCurrTickCount - gPrevTickCount);
				gPrevTickCount = gCurrTickCount;
				
			}
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;
		default:
			break;
		}
		break;
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
	gMemDC = CreateCompatibleDC(gHDC);
	gBufDC = CreateCompatibleDC(gHDC);
	HBITMAP bmp = CreateCompatibleBitmap(gHDC, 800, 600);
	SelectObject(gMemDC, bmp);

	gBmpBackground = (HBITMAP)LoadImage(nullptr, _T("bg.bmp"), IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	gBmpSprite = (HBITMAP)LoadImage(nullptr, _T("angrybird.bmp"), IMAGE_BITMAP, 120, 60, LR_LOADFROMFILE);
	
	PlaySound(_T("¥Ó¥ë¥¸ºþ.wav"), nullptr, SND_LOOP | SND_ASYNC | SND_FILENAME);

	gSpeedX = 1;
	gSpeedY = 0;
	GetClientRect(hwnd, &gRect);

	GameUpdate(hwnd, 0);

	return true;
}

void GameUpdate(HWND hwnd, DWORD tickMilliSecond)
{
	gPosX += gSpeedX;
	gSpeedY += gGravity;
	gPosY += gSpeedY;

	if (gPosY >= gRect.bottom - 60)
	{
//		gPosY = gRect.bottom - 60;
		gSpeedY = -gSpeedY;
	}

	SelectObject(gBufDC, gBmpBackground);
	BitBlt(gMemDC, 0, 0, 800, 600, gBufDC, 0, 0, SRCCOPY);
	/**
	*	The upper function occlusions all black color in the original image, so
	*	we could use 0-1 bitmap to make a better occlusion like the following:
	*	*/
	SelectObject(gBufDC, gBmpSprite);
	BitBlt(gMemDC, gPosX, gPosY, 60, 60,
		gBufDC, 60, 0, SRCAND);
	BitBlt(gMemDC, gPosX, gPosY, 60, 60,
		gBufDC, 0, 0, SRCPAINT);

	BitBlt(gHDC, 0, 0, 800, 600, gMemDC, 0, 0, SRCCOPY);
}

bool GameExit(HWND hwnd)
{
	DeleteObject(gBmpBackground);
	DeleteObject(gBmpSprite);	

	DeleteDC(gBufDC);
	DeleteDC(gMemDC);
	ReleaseDC(hwnd, gHDC);
	return true;
}
