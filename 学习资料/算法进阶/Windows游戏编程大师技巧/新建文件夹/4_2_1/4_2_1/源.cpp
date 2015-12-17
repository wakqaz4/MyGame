#include <windows.h>
#include <windowsX.h>
#include <tchar.h>
#include <wingdi.h>
#define WINDOW_CLASS_NAME "win_class_name"
#define CHECK(x)	(x>300?(x-=300):x)
int ii = 0;
LRESULT CALLBACK WinProc(HWND hwnd,
	UINT msg,
	WPARAM wparam, 
	LPARAM lparam
	)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_CREATE:
		SetTimer(hwnd, 1, 1000, NULL);
		return 0;
		break;
	case WM_PAINT:		
		hdc = GetDC(hwnd);
/******************************************
//Draw random points to fill the window
		for (int index = 0; index < 10; index++)
		{
			int x = rand() % 400;
			int y = rand() % 400;
			COLORREF color = RGB(111, 111, 111);
			SetPixel(hdc, x, y, color);
		}
//End random points
/********************************************/
/********************************************
//Draw lines
		HPEN green_pen;
		green_pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		HGDIOBJ old_pen;
		old_pen = SelectObject(hdc, green_pen);
		//draw line
		MoveToEx(hdc, 10, 10, NULL);
		LineTo(hdc, 50, 50);
		LineTo(hdc, 10, 50);
		LineTo(hdc, 10, 10);
		//restore old pen
		//SelectObject(hdc, old_pen);
		DeleteObject(old_pen);
		DeleteObject(green_pen);
//End draw lines	
********************************************/
/********************************************
//Draw Rectangle
		//Create pen and brush
		HPEN blue_pen;
		blue_pen = CreatePen(PS_DASH, 3, RGB(0, 0, 255));
//		HBRUSH red_brush;
//		red_brush = CreateSolidBrush(RGB(255, 0, 0));
		//Select the pen and brush into context
		SelectObject(hdc, blue_pen);
//		SelectObject(hdc, red_brush);
		Rectangle(hdc, 10, 50, 50, 100);
		DeleteObject(blue_pen);
//		DeleteObject(red_brush);
********************************************/
/********************************************
		//Draw Ellipse and move right
		//Use Rectangle function to clear background 		
		//Create pen and brush
		HPEN blue_pen;
		blue_pen = CreatePen(PS_DASH, 3, RGB(0, 0, 255));
		HPEN default_pen;
		default_pen = CreatePen(PS_SOLID, 1, RGB(111, 111, 111));
		//		HBRUSH red_brush;
		//		red_brush = CreateSolidBrush(RGB(255, 0, 0));
		//Select the pen and brush into context
		SelectObject(hdc, blue_pen);
		//		SelectObject(hdc, red_brush);
		Ellipse(hdc, 10 + CHECK(ii), 10, 100 + ii, 100);
		ii++;
		Sleep(100);
		SelectObject(hdc, default_pen);
		Rectangle(hdc, 0, 0, 400, 400);
		DeleteObject(blue_pen);
		DeleteObject(default_pen);
		//		DeleteObject(red_brush);
********************************************/
		ReleaseDC(hwnd, hdc);
		return 0;
		break;
//TODO:WM_TIMER has a low priority, so if you dealt with WM_PAINT	
//in your programme, WM_TIMER will never be get responsion
	case WM_TIMER:											
		switch (wparam)
		{
		case 1:
			MessageBox(NULL, _T("Mister Neverdie"), _T("Mister Nevermore"), MB_OKCANCEL);
			break;
		default:
			break;
		}
		return 0;
		break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		return 0;
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow
	 )
{
	WNDCLASS wndclass;
	HWND hwnd;
	MSG msg;
	HDC hdc;

	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadCursor(NULL, IDI_APPLICATION);
	wndclass.hInstance = hinstance;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = _T(WINDOW_CLASS_NAME);
	wndclass.lpfnWndProc = WinProc;
	wndclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	
	if (!RegisterClass(&wndclass))
	{
		return 0;
	}
	if (!(hwnd = CreateWindowEx(NULL,
		_T(WINDOW_CLASS_NAME),
		_T("My Class"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0,
		500, 500,
		NULL, NULL,
		hinstance,
		NULL
		)))
	{
		return 0;
	}
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}