/*-------------------------------------------------------------------------

CHECKER1.C --      Mouse Hit-Test Demo Program No. 1

(c) Charles Petzold, 1998

--------------------------------------------------------------------------*/

#include <windows.h>

#include <time.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR  szCmdLine, int iCmdShow)

{
	static TCHAR      szAppName[] = TEXT("Checker1");

	HWND         hwnd;

	MSG          msg;

	WNDCLASS     wndclass;



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

		MessageBox(NULL, TEXT("Program requires Windows NT!"),

			szAppName, MB_ICONERROR);

		return 0;

	}

	hwnd = CreateWindow(szAppName, TEXT("Checker1 Mouse Hit-Test Demo"),

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
	static int cxRect, cyRect;
	static int cxPos, cyPos;
	static int cxTempCursor, cyTempCursor;
	static int showCross[DIVISIONS][DIVISIONS] = { 0 };
	RECT	rect;
	RECT	oldRect;
	HDC		hdc;
	PAINTSTRUCT ps;
	srand((unsigned) time(NULL));

	static int cxObject, cyObject;
	static int bufferCross[2] = { -1 };

	switch (message)
	{

		case WM_CREATE:
			cxObject = rand() % DIVISIONS;
			cyObject = rand() % DIVISIONS;

			bufferCross[0] = cxObject;
			bufferCross[1] = cyObject;
			showCross[cxObject][cyObject] = 1;

			break;
		case WM_LBUTTONDOWN:
			cxRect = LOWORD(lParam) / cxPos;
			cyRect = HIWORD(lParam) / cyPos;
//			(showCross[cxRect][cyRect] == 1) ? (showCross[cxRect][cyRect] = 0) : (showCross[cxRect][cyRect] = 1);
			if (showCross[cxRect][cyRect] == 1)
			{
				showCross[cxRect][cyRect] = 0;
				cxObject = rand() % DIVISIONS;
				cyObject = rand() % DIVISIONS;
				bufferCross[0] = cxObject;
				bufferCross[1] = cyObject;

				showCross[cxObject][cyObject] = 1;
				oldRect.left = cxObject*cxPos;
				oldRect.right = oldRect.left + cxPos;
				oldRect.top = cyObject*cyPos;
				oldRect.bottom = oldRect.top + cyPos;
				InvalidateRect(hwnd, &oldRect, true);

				rect.left = cxRect*cxPos;
				rect.right = rect.left + cxPos;
				rect.top = cyRect*cyPos;
				rect.bottom = rect.top + cyPos;
				InvalidateRect(hwnd, &rect, true);
			}
			break;
		case WM_SIZE:
			cxPos = LOWORD(lParam) / DIVISIONS;
			cyPos = HIWORD(lParam) / DIVISIONS;
			break;

		case WM_PAINT:
//			hdc = GetDC(hwnd);
			hdc = BeginPaint(hwnd, &ps);
			for (int i = 0; i < DIVISIONS+1; i++)
			{			
				MoveToEx(hdc, i*cxPos, 0, NULL);
				LineTo(hdc, i*cxPos, (DIVISIONS )*cyPos);
			}
			for (int j = 0; j < DIVISIONS+1; j++)
			{
				MoveToEx(hdc, 0, j*cyPos,  NULL);
				LineTo(hdc, (DIVISIONS )*cxPos, j*cyPos);
			}
			for (int i = 0; i < DIVISIONS; i++)
			{
				for (int j = 0; j < DIVISIONS; j++)
				{
					if (showCross[i][j] == 1)
					{
						MoveToEx(hdc, i*cxPos, j*cyPos, NULL);
						LineTo(hdc, (i + 1)*cxPos, (j + 1)*cyPos);
						MoveToEx(hdc, (i + 1)*cxPos, j*cyPos, NULL);
						LineTo(hdc, i*cxPos, (j + 1)*cyPos);
					}
				}
			}
//			ReleaseDC(hwnd, hdc);
			EndPaint(hwnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:

			
			break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}