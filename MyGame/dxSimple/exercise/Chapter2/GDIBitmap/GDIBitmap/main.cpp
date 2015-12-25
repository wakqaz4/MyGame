/************************************************************************
* This program intends to use Bitmap and Pen and Brush to create a naturo
* image with two brushes on left&right side, and one narrow line on the top, 
* with another fat line at the bottom.
**********************************************************************/
#include <windows.h>
#include <tchar.h>

/************************************************************************
* Include libs                                                                  
************************************************************************/
#pragma comment(lib, "winmm.lib")

/************************************************************************
* External functions                                                                  
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool GameInit(HWND);
extern void GamePaint(HWND);
extern bool GameCleanup(HWND);

/************************************************************************
* Global vars                                                                 
************************************************************************/

HDC gHDC;
HDC gBufferDC;
HPEN gHPen[2];
HBRUSH gHBrush[2];

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wndClass.hIcon = LoadIcon(nullptr, _T("icon.ico"));
	wndClass.hIcon = (HICON)LoadImage(nullptr, _T("icon.ico"), IMAGE_ICON, 0, 0,  LR_LOADFROMFILE);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = _T("BitmapPenBrush");
	wndClass.lpszMenuName = _T("Menu");
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("BitmapPenBrush"), _T("²âÊÔË«»º´æ¡¢Bitmap¡¢BitBlt¡¢Pen¡¢Brush"),
		WS_OVERLAPPEDWINDOW, 300, 300,
		800, 600,
		nullptr, nullptr, hInstance, nullptr);

	MoveWindow(hwnd, 300, 300, 800, 600, false);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	PlaySound(_T("²ÖÄ¾ÂéÒÂ - Time After Time~»¨Îè¤¦½Ö¤Ç~.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	MSG msg{ 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//Should write your GameUpdate here
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
	case WM_PAINT:
		gHDC = BeginPaint(hwnd, &ps);
		GamePaint(hwnd);
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, nullptr);
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
	GamePaint(hwnd);
	ReleaseDC(hwnd, gHDC);

	for (int i = 0; i < 2; i++)
	{
		gHPen[i] = CreatePen(PS_SOLID, 3, RGB(i*100, i*100 + 100, 200));
		
	}
	gHBrush[0] = CreateSolidBrush(RGB(0, 100, 200));
	gHBrush[1] = CreateHatchBrush(HS_DIAGCROSS, RGB(200, 100, 0));

	return true;
}

void GamePaint(HWND hwnd)
{
	HBITMAP bitmap;
	bitmap = (HBITMAP)LoadImage(nullptr, _T("Naruto.bmp"), IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	SelectObject(gBufferDC, bitmap);

	SelectObject(gBufferDC, gHPen[0]);
	MoveToEx(gBufferDC, 30, 50, nullptr);
	LineTo(gBufferDC, 700, 50);

	SelectObject(gBufferDC, gHPen[1]);
	MoveToEx(gBufferDC, 30, 500, nullptr);
	LineTo(gBufferDC, 700, 500);

	SelectObject(gBufferDC, gHBrush[0]);
	Rectangle(gBufferDC, 50, 100, 100, 500);

	SelectObject(gBufferDC, gHBrush[1]);
	RoundRect(gBufferDC, 700, 100, 750, 500, 5, 5);

	WCHAR text[] = _T("For the 3rd Empire!");
	TextOut(gBufferDC, 400, 300, text, wcslen(text));

	BitBlt(gHDC, 0, 0, 800, 600, gBufferDC, 0, 0, SRCCOPY);

	DeleteObject(bitmap);

}

bool GameCleanup(HWND hwnd)
{
	
	DeleteDC(gBufferDC);
	return true;
}