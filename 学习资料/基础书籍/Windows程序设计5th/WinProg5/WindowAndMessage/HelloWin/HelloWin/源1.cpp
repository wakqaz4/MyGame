#include <windows.h>
#include <tchar.h>
using namespace std;

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	WNDCLASS wndclass;
	static TCHAR szAppName[] = _T("HelloWin");
	HWND hwnd;
	MSG msg;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	wndclass.lpfnWndProc = WinProc;

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
		MessageBox(NULL, _T("Never Say Ever!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,      // window class name

		TEXT("The Hello Program"),   // window caption

		WS_OVERLAPPEDWINDOW,  // window style

		CW_USEDEFAULT,// initial x position

		CW_USEDEFAULT,// initial y position

		CW_USEDEFAULT,// initial x size

		CW_USEDEFAULT,// initial y size

		NULL,                 // parent window handle

		NULL,            // window menu handle

		hInstance,   // program instance handle

		NULL);      // creation parameters



	ShowWindow(hwnd, iCmdShow);

	UpdateWindow(hwnd);

	while (true)
	{
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{			
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	HDC                   hdc;

	PAINTSTRUCT ps;

	RECT          rect;

	switch (message)
	{

		case WM_CREATE:

			//PlaySound(TEXT("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC);

			return 0;


		case   WM_PAINT:

			hdc = BeginPaint(hwnd, &ps);



			GetClientRect(hwnd, &rect);



			DrawText(hdc, TEXT("Hello, Windows7!"), -1, &rect,

				DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			EndPaint(hwnd, &ps);

			return 0;



		case   WM_DESTROY:

			PostQuitMessage(0);

			return 0;

		}

		return DefWindowProc(hwnd, message, wparam, lparam);

}
