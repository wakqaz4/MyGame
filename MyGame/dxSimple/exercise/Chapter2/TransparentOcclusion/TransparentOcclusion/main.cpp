/************************************************************************
* This is a program that realizes transparent occlusion                                                                  
************************************************************************/

#include <windows.h>
#include <tchar.h>
#include <time.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

/************************************************************************
* External global functions                                                                  
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool GameInit(HWND);
void GamePaint(HWND);
bool GameClearup(HWND);

/************************************************************************
* External global vars                                                                  
************************************************************************/
HDC gHDC;
HDC gBufferDC;
HDC gBufferDC2;
HBITMAP gBitmap;
HBITMAP gCharacter1;
HBITMAP gCharacter2;

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
	wndClass.lpszClassName = _T("CTransparentOcclusion");
	wndClass.lpszMenuName = _T("Menu");
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
    
	if (!RegisterClass(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("CTransparentOcclusion"), _T("TransparentOcclusion"), WS_OVERLAPPEDWINDOW,
		300, 300, 800, 600,
		nullptr, nullptr, hInstance, nullptr);

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
	case WM_PAINT:
		gHDC = BeginPaint(hwnd, &ps);
		GamePaint(hwnd);
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, nullptr);
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
	PlaySound(_T("‚èƒæ§≥§Œ§ﬂ - This game.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);

	gBitmap = (HBITMAP)LoadImage(nullptr, _T("bg.bmp"), IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	gCharacter1 = (HBITMAP)LoadImage(nullptr, _T("character1.bmp"), IMAGE_BITMAP, 640, 500, LR_LOADFROMFILE);
	gCharacter2 = (HBITMAP)LoadImage(nullptr, _T("character2.bmp"), IMAGE_BITMAP, 800, 500, LR_LOADFROMFILE);

	gHDC = GetDC(hwnd);
	gBufferDC = CreateCompatibleDC(gHDC);
	gBufferDC2 = CreateCompatibleDC(gHDC);
	GamePaint(hwnd);
	ReleaseDC(hwnd, gHDC);	

	return true;
}

void GamePaint(HWND hwnd)
{
	SelectObject(gBufferDC, gBitmap);
	BitBlt(gHDC, 0, 0, 800, 600, gBufferDC, 0, 0, SRCCOPY);

	SelectObject(gBufferDC, gCharacter1);
	BitBlt(gHDC, 0, 100, 320, 600, gBufferDC, 320, 0, SRCAND);
	BitBlt(gHDC, 0, 100, 320, 600, gBufferDC, 0, 0, SRCPAINT);

	SelectObject(gBufferDC, gCharacter2);
	BitBlt(gHDC, 400, 100, 400, 600, gBufferDC, 400, 0, SRCAND);
	BitBlt(gHDC, 400, 100, 400, 600, gBufferDC, 0, 0, SRCPAINT);

	WCHAR NPCName[] = _T("Œ·Õıƒ·¬ª:");
	WCHAR artContext[] = _T("Hey you!");

	HBITMAP bmp = CreateCompatibleBitmap(gBufferDC, 800, 600);	
	SelectObject(gBufferDC, bmp);

	HFONT font1 = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("ø¨ÃÂ"));
	SelectObject(gBufferDC, font1);	
	SetTextColor(gBufferDC, RGB(0, 0, 255));
	TextOut(gBufferDC, 10, 400, NPCName, wcslen(NPCName));

	HFONT font2 = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("TimesNewRoman"));
	SelectObject(gBufferDC, font2);
	SetTextColor(gBufferDC, RGB(255, 0, 0));
	TextOut(gBufferDC, 100, 400, artContext, wcslen(artContext));

	BitBlt(gHDC, 0, 0, 800, 600, gBufferDC, 0, 0, SRCPAINT);
	//SetBkColor(gBufferDC, RGB(255, 255, 255));
	//TransparentBlt(gHDC, 0, 0, 800, 600, gBufferDC, 0, 0, 800, 600, RGB(255, 255, 255));

}

bool GameClearup(HWND hwnd)
{
	DeleteObject(gBitmap);
	DeleteObject(gCharacter1);
	DeleteObject(gCharacter2);
	DeleteDC(gBufferDC);
	DeleteDC(gBufferDC2);
	return true;
}