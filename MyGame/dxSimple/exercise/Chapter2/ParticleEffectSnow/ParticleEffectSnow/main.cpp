/************************************************************************
* Simulates snow particle effects
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

#define MAX_SNOW_NUM 80
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


int gSnowNum = 0;
struct SSnowSprite
{
	int x;
	int y;
	bool isActive;
};
SSnowSprite snow[MAX_SNOW_NUM];

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
	PlaySound(_T("NOIR3.wav"), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP);
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
	UnregisterClass(_T("CName"), hInstance);
	exit(0);
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
	gBmpSprite = (HBITMAP)LoadImage(nullptr, _T("snow.bmp"), IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);

	GetClientRect(hwnd, &gRect);
	srand(unsigned (time(NULL)));
	GameUpdate(hwnd, 0);

	return true;
}

void GameUpdate(HWND hwnd, DWORD tickMilliSecond)
{
	SelectObject(gBufDC, gBmpBackground);
	BitBlt(gMemDC, 0, 0, 800, 600, gBufDC, 0, 0, SRCCOPY);

	if (snow[gSnowNum].isActive == false)
	{
		snow[gSnowNum].isActive = true;
		snow[gSnowNum].x = rand()%gRect.right;
		snow[gSnowNum].y = 0;
		gSnowNum++;
	}
	for (int i = 0; i < gSnowNum; i++)
	{
		snow[i].x += (rand() % 12) - 6;
		snow[i].y += (rand() % 3) + 5;
		if (snow[i].x >= gRect.right)
		{
			snow[i].x = gRect.right - 10;
		}
		if (snow[i].y >= gRect.bottom)
		{
			snow[i].y = 0;
		}
		SelectObject(gBufDC, gBmpSprite);
		TransparentBlt(gMemDC, snow[i].x, snow[i].y, 30, 30, gBufDC, 0, 0, 30, 30, RGB(0, 0, 0));
	}

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
