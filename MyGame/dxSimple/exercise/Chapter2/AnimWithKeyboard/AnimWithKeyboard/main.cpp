/************************************************************************
* Animation triggered by key WASD                                                                  
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>

#include "sprite.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

/************************************************************************
* External functions and global variables                                                                  
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool GameInit(HWND hwnd);
extern void GameUpdate(HWND hwnd);
extern bool GameExit(HWND hwnd);

HDC gHDC;
HDC gMemDC;
HDC gBufDC;
HBITMAP gBmpBackground;
HBITMAP gBmpUp;
HBITMAP gBmpDown;
HBITMAP gBmpLeft;
HBITMAP gBmpRight;

CSprite gCharacter;
DWORD gPrevTickCount;
DWORD gCurrTickCount;

int gSpriteIndex = 0;
const int walkInterval = 10;

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
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':
			gCharacter.mDirection = CSprite::UP;
			gCharacter.mPosY -= 10;
			if (gCharacter.mPosY <= 10)
			{
				gCharacter.mPosY = 10;
			}
			break;
		case 'A':
			gCharacter.mDirection = CSprite::LEFT;
			gCharacter.mPosX -= 10;
			if (gCharacter.mPosX <= 10)
			{
				gCharacter.mPosX = 10;
			}
			break;
		case 'S':
			gCharacter.mDirection = CSprite::DOWN;
			gCharacter.mPosY += 10;
			if (gCharacter.mPosY >= 790)
			{
				gCharacter.mPosY =790;
			}
			break;
		case 'D':
			gCharacter.mDirection = CSprite::RIGHT;
			gCharacter.mPosX += 10;
			if (gCharacter.mPosY >= 590)
			{
				gCharacter.mPosY = 590;
			}			
			break;
			

		case VK_ESCAPE:
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;
		default:
			return 0;
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
	gBmpUp= (HBITMAP)LoadImage(nullptr, _T("go1.bmp"), IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);
	gBmpDown = (HBITMAP)LoadImage(nullptr, _T("go2.bmp"), IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);
	gBmpLeft = (HBITMAP)LoadImage(nullptr, _T("go3.bmp"), IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);
	gBmpRight = (HBITMAP)LoadImage(nullptr, _T("go4.bmp"), IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);

	PlaySound(_T("ÑÒÌï¤µ¤æ¤ê - Thank You For Everything.wav"), nullptr, SND_LOOP | SND_ASYNC | SND_FILENAME);

	GameUpdate(hwnd);

	return true;
}

void GameUpdate(HWND hwnd)
{
	SelectObject(gBufDC, gBmpBackground);
	BitBlt(gMemDC, 0, 0, 800, 600, gBufDC, 0, 0, SRCCOPY);

	switch (gCharacter.mDirection)
	{
	case CSprite::UP:
		SelectObject(gBufDC, gBmpUp);
		break;
	case CSprite::DOWN:
		SelectObject(gBufDC, gBmpDown);
		break;
	case CSprite::LEFT:
		SelectObject(gBufDC, gBmpLeft);
		break;
	case CSprite::RIGHT:
		SelectObject(gBufDC, gBmpRight);
		break;
	default:
		break;
	}	
	TransparentBlt(gMemDC, gCharacter.mPosX, gCharacter.mPosY, gCharacter.mWidth, gCharacter.mHeight,
		gBufDC, gSpriteIndex*gCharacter.mWidth, 0, gCharacter.mWidth, gCharacter.mHeight,
		RGB(0, 0, 0));
	/**
	 *	The upper function occlusions all black color in the original image, so 
	 *	we could use 0-1 bitmap to make a better occlusion like the following:
	 *	*/
	BitBlt(gMemDC, gCharacter.mPosX, gCharacter.mPosY, gCharacter.mWidth, gCharacter.mHeight,
		gBufDC, gSpriteIndex*gCharacter.mWidth, gCharacter.mHeight, SRCAND);
	BitBlt(gMemDC, gCharacter.mPosX, gCharacter.mPosY, gCharacter.mWidth, gCharacter.mHeight,
		gBufDC, gSpriteIndex*gCharacter.mWidth, 0, SRCPAINT);

	if (++gSpriteIndex >= 8)
	{
		gSpriteIndex = 0;
	}

	BitBlt(gHDC, 0, 0, 800, 600, gMemDC, 0, 0, SRCCOPY);
}
	
bool GameExit(HWND hwnd)
{
	DeleteObject(gBmpBackground);
	DeleteObject(gBmpUp);
	DeleteObject(gBmpDown);
	DeleteObject(gBmpLeft);
	DeleteObject(gBmpRight);

	DeleteDC(gBufDC);
	DeleteDC(gMemDC);
	ReleaseDC(hwnd, gHDC);
	return true;
}
