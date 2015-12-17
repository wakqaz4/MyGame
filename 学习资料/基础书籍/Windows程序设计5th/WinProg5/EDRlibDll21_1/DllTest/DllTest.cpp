//EDRTEST.C

/*---------------------------------------------------------------------------

EDRTEST.C -- Program using EDRLIB dynamic-link library

(c) Charles Petzold, 1998

----------------------------------------------------------------------------*/

#include <windows.h>

#include "EDRlibDll.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

typedef BOOL(CALLBACK* fp)(HDC, PRECT, PCWSTR);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,

	PSTR szCmdLine, int iCmdShow)

{

	static TCHAR szAppName[] = TEXT("StrProg");

	HWND                                 hwnd;

	MSG                                  msg;

	WNDCLASS							 wndclass;

//	LoadLibrary(TEXT("EDRlibDll21_1.Dll"));

	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	wndclass.lpfnWndProc = WndProc;

	wndclass.cbClsExtra = 0;

	wndclass.cbWndExtra = 0;

	wndclass.hInstance = hInstance;

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	wndclass.lpszMenuName = NULL;

	wndclass.lpszClassName = szAppName;



	if (!RegisterClass(&wndclass))

	{

		MessageBox(NULL, TEXT("This program requires Windows NT!"),

			szAppName, MB_ICONERROR);

		return 0;

	}



	hwnd = CreateWindow(szAppName, TEXT("DLL Demonstration Program"),

		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT,

		CW_USEDEFAULT, CW_USEDEFAULT,

		NULL, NULL, hInstance, NULL);


	ShowWindow(hwnd, iCmdShow);

	UpdateWindow(hwnd);

//	DllClass testClass;


	while (GetMessage(&msg, NULL, 0, 0))

	{

		TranslateMessage(&msg);

		DispatchMessage(&msg);

	}

	return msg.wParam;

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

{

	HDC                   hdc;

	PAINTSTRUCT           ps;

	RECT                  rect;
	void* m = ::LoadLibrary(LPCWSTR("EDRlibDll21_1.dll"));
	fp FuncOne;
	FuncOne = (fp)GetProcAddress((HMODULE)m, "EdrCenterText");


	switch (message)

	{

	case   WM_PAINT:

		hdc = BeginPaint(hwnd, &ps);



		GetClientRect(hwnd, &rect);



		FuncOne(hdc, &rect,

			TEXT("This string was displayed by a DLL"));



		EndPaint(hwnd, &ps);

		return 0;



	case   WM_DESTROY:

		PostQuitMessage(0);

		return 0;

	}

	return DefWindowProc(hwnd, message, wParam, lParam);

}
