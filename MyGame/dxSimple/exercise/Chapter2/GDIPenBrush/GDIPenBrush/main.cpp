/************************************************************************
* Include headers
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>

/************************************************************************
* Include libs                                                                  
************************************************************************/
#pragma comment(lib, "winmm.lib")

/************************************************************************
* Declare external functions                                                                   
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
extern bool GameInit(HWND hwnd);
extern void GamePaint(HWND hwnd);
extern bool GameCleanup(HWND hwnd);

/************************************************************************
* Define global variables: hdc, pen ,brush                                                                  
************************************************************************/
HDC gHdc = nullptr;
HPEN gHPen[7] = { 0 };
HBRUSH gHBrush[7]{0};
int gIPenStyle[7] = { PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT, PS_DASHDOTDOT, PS_NULL, PS_INSIDEFRAME };
int gIBrushStyle[7] = { HS_VERTICAL, HS_HORIZONTAL, HS_CROSS, HS_DIAGCROSS, HS_FDIAGONAL, HS_BDIAGONAL };

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = (HICON)LoadImage(nullptr, _T("newIcon.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = _T("CPenAndBrush");
	wndClass.lpszMenuName = _T("MenuName");
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return 0;
	}

	HWND hwnd = CreateWindow(_T("CPenAndBrush"), _T("For the fantasy!"), WS_OVERLAPPEDWINDOW,
		300, 300,
		800, 600,
		nullptr, nullptr, hInstance, nullptr);

	MoveWindow(hwnd, 300, 300, 800, 600, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	PlaySound(_T("日笠陽子 - 美しき残酷な世界.wav"), nullptr, SND_ASYNC | SND_FILENAME|SND_LOOP);

	MSG msg = { 0 };
	while ( msg.message != WM_QUIT )
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		//if (GetMessage(&msg, 0, 0, 0) != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}		
	}

	UnregisterClass(_T("CPenAndBrush"), wndClass.hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
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
		ValidateRect(hwnd, nullptr);
		break;
	case WM_DESTROY:
		GameCleanup(hwnd);
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		PlaySound(_T("日笠陽子 - 美しき残酷な世界.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
		break;

	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return 0;
}

bool GameInit(HWND hwnd)
{
	gHdc = GetDC(hwnd);
	srand((unsigned)time(nullptr));

	/**
	 *	Init pen and brush randomly
	 *	*/
	for (int i = 0; i < 7; i++)
	{
		gHPen[i] = CreatePen(gIPenStyle[i], 1, RGB(rand() % 256, rand() % 256, rand() % 256));
		if (i == 6)
		{
			gHBrush[i] = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
		}
		else
		{
			gHBrush[i] = CreateHatchBrush(gIBrushStyle[i], RGB(rand() % 256, rand() % 256, rand() % 256));
		}		
	}
	GamePaint(hwnd);
	ReleaseDC(hwnd, gHdc);
	return true;
}

void GamePaint(HWND hwnd)
{
	int y = 30;
	for (int i = 0; i < 7; i++)
	{
		SelectObject(gHdc, gHPen[i]);
		MoveToEx(gHdc, 30, y, nullptr);
		LineTo(gHdc, 100, y);
		y += 70;
	}

	int x1 = 120;
	int x2 = 190;
	for (int i = 0; i < 7; i++)
	{
		SelectObject(gHdc, gHBrush[i]);
		Rectangle(gHdc, x1, 70, x2, y);
		x1 += 90;
		x2 += 90;
	}
}
bool GameCleanup(HWND hwnd)
{
	for (int i = 0; i < 7; i++)
	{
		DeleteObject(gHPen[i]);
		DeleteObject(gHBrush[i]);
	}
	return true;
}
