//MENUDEMO.C

/*---------------------------------------------------------------------

MENUDEMO.C --   Menu Demonstration

(c) Charles Petzold, 1998

-----------------------------------------------------------------------*/

#include <windows.h>

#include "resource.h"


#define ID_TIMER 1


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


TCHAR szAppName[] = TEXT("MENUDEMO");


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,

	PSTR szCmdLine, int iCmdShow)

{

	HWND     hwnd;

	MSG                           msg;

	WNDCLASS              wndclass;



	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	wndclass.lpfnWndProc = WndProc;

	wndclass.cbClsExtra = 0;

	wndclass.cbWndExtra = 0;

	wndclass.hInstance = hInstance;

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);

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



	hwnd = CreateWindow(szAppName, TEXT("Menu Demonstration"),

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


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

{

	static int idColor[5] = { WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH,

		DKGRAY_BRUSH, BLACK_BRUSH };

	static int iSelection = ID_BACKGROUND_WHITE;

	HMENU                         hMenu;



	switch (message)

	{

	case   WM_COMMAND:

		hMenu = GetMenu(hwnd);



		switch (LOWORD(wParam))

		{

		case   ID_NEW_OPEN:

		case   ID_NEW_SAVE:

		case   ID_FILE_SAVE:

		case   ID_FILE_SAVEAS:

			MessageBeep(0);

			return 0;


		case   ID_FILE_EXIT:

			SendMessage(hwnd, WM_CLOSE, 0, 0);

			return 0;



		case   ID_EDIT_UNDO:

			MessageBeep(0);

			return 0;


		case   ID_TIMER_START:              // the order shown here.

			CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);

			iSelection = LOWORD(wParam);

			CheckMenuItem(hMenu, iSelection, MF_CHECKED);



			SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)

				GetStockObject

				(idColor[LOWORD(wParam) - ID_BACKGROUND_WHITE]));



			InvalidateRect(hwnd, NULL, TRUE);


			if (SetTimer(hwnd, ID_TIMER, 1000, NULL))

			{

				EnableMenuItem(hMenu, ID_TIMER_START, MF_GRAYED);

				EnableMenuItem(hMenu, ID_TIMER_END, MF_ENABLED);

			}

			return 0;



		case   ID_TIMER_END:

			KillTimer(hwnd, ID_TIMER);

			EnableMenuItem(hMenu, ID_TIMER_START, MF_ENABLED);

			EnableMenuItem(hMenu, ID_TIMER_END, MF_GRAYED);

			return 0;

		case   ID_ABOUT_HELP:

			MessageBox(hwnd, TEXT("Help not yet implemented!"),

				szAppName, MB_ICONEXCLAMATION | MB_OK);

			return 0;


		}

		break;



	case   WM_TIMER:

		MessageBeep(0);

		return 0;



	case   WM_DESTROY:

		PostQuitMessage(0);

		return 0;

	}

	return DefWindowProc(hwnd, message, wParam, lParam);

}
