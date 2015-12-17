//ABOUT2.C

/*--------------------------------------------------------------------------

ABOUT2.C --     About Box Demo Program No. 2

(c) Charles Petzold, 1998

---------------------------------------------------------------------------*/

#include <windows.h>

#include "resource.h"


LRESULT     CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,

	PSTR szCmdLine, int iCmdShow)

{

	static TCHAR          szAppName[] = TEXT("ABOUT1");

	MSG                                          msg;

	HWND                                         hwnd;

	WNDCLASS                             wndclass;



	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	wndclass.lpfnWndProc = WndProc;

	wndclass.cbClsExtra = 0;

	wndclass.cbWndExtra = 0;

	wndclass.hInstance = hInstance;

	wndclass.hIcon = LoadIcon(hInstance, szAppName);

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	wndclass.lpszMenuName = szAppName;

	wndclass.lpszClassName = szAppName;



	if (!RegisterClass(&wndclass))

	{

		MessageBox(NULL, TEXT("This program requires Windows NT!"),

			szAppName, MB_ICONERROR);

		return 0;

	}



	hwnd = CreateWindow(szAppName, TEXT("About Box Demo Program"),

		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT,

		CW_USEDEFAULT, CW_USEDEFAULT,

		NULL, NULL, hInstance, NULL);


	ShowWindow(hwnd, iCmdShow);

	UpdateWindow(hwnd);



	while (GetMessage(&msg, NULL, 0, 0))

	{

		TranslateMessage(&msg);

		DispatchMessage(&msg);

	}

	return msg.wParam;

}
LRESULT     CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
