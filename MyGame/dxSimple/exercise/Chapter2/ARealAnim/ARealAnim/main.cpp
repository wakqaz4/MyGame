/************************************************************************
* This project implements a real animation using a series of images 
* which is updated in the main game loop. 
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>

#pragma comment(lib, "winmm.lib")

/************************************************************************
* Declare external functions and global variables                                                                  
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool GameInit(HWND hwnd);
extern void GamePaint(HWND hwnd);
extern bool GameCleanup(HWND hwnd);

HDC gHDC;
HDC gBufferDC;
HBITMAP gBmp[11]{0};
DWORD gCurrTickCount = 0;
DWORD gPrevTickCount = 0;
int gBmpIndex = 0;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = (HICON)LoadImage(NULL, _T("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = _T("CName");
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("CName"), nullptr, WS_OVERLAPPEDWINDOW,
		300, 300, 800, 600,
		nullptr, nullptr, hInstance, nullptr);

	if (!GameInit(hwnd))
	{
		MessageBox(nullptr, _T("Init Games Failed!"), _T("Error"), MB_OK);
		return -1;
	}
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
			gCurrTickCount = GetTickCount();
			if (gCurrTickCount - gPrevTickCount >= 100)
			{
				GamePaint(hwnd);
			}

        }
	}

	return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:

		break;
	case WM_DESTROY:
		GameCleanup(hwnd);
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
	gBufferDC = CreateCompatibleDC(gHDC);
	PlaySound(_T("moumoon - Hello,shooting-star.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);

	WCHAR imageName[30];
	for (int i = 0; i < 11; i++)
	{
		wsprintf(imageName, _T("%d.bmp"), i);
		gBmp[i] = (HBITMAP)LoadImage(nullptr, imageName, IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	}

	return true;
}

void GamePaint(HWND hwnd)
{
	SelectObject(gBufferDC, gBmp[gBmpIndex++>=10?gBmpIndex = 0:gBmpIndex]);
	BitBlt(gHDC, 0, 0, 800, 600, gBufferDC, 0, 0, SRCCOPY);
	gPrevTickCount = gCurrTickCount;
}

bool GameCleanup(HWND hwnd)
{
	for (int i = 0; i < 11; i++)
	{
		DeleteObject(gBmp[i]);
	}
	DeleteDC(gBufferDC);
	ReleaseDC(hwnd, gHDC);
	return true;
}