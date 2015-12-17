#include <windows.h>
#include <math.h>
#include <process.h>


static TCHAR* szAppChildName[4] =
{
	TEXT("IncreasingNumber"),
	TEXT("IncreasingPrimeNumber"),
	TEXT("Fibonacci"),
	TEXT("DrawCircle")
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndIncreasingNumber(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndIncreasingPrimeNumber(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndFibonacci(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndDrawCircle(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT(CALLBACK *ChildWndProc[4])(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) =
{
	WndIncreasingNumber,
	WndIncreasingPrimeNumber,
	WndFibonacci,
	WndDrawCircle
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR sznCmdLine, int iCmdShow)
{
	WNDCLASS	wndclass;
	HWND		hwnd;
	MSG			msg;
	static TCHAR szAppName[] = TEXT("Multi1");

	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	wndclass.lpfnWndProc = WndProc;

	wndclass.cbClsExtra = 0;

	wndclass.cbWndExtra = 0;

	wndclass.hInstance = hInstance;

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	wndclass.lpszMenuName = NULL;

	wndclass.lpszClassName = szAppName;



	if (!RegisterClass(&wndclass))

	{

		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);

		return 0;

	}
	for (int i = 0; i < 4; i++)
	{
		wndclass.lpfnWndProc = ChildWndProc[i];
		wndclass.lpszClassName = szAppChildName[i];
		wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		RegisterClass(&wndclass);
	}

	hwnd = CreateWindow(szAppName, TEXT("Multitasking Demo"),

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

void IncreasingNumber(PVOID pvoid)
{};
void IncreasingPrimeNumber(PVOID pvoid)
{};
void Fibonacci(PVOID pvoid)
{};
void DrawCircle(PVOID pvoid)
{};
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndChild[4];
	static int cxClient, cyClient;
	switch (message)
	{
	case WM_CREATE:
		for (int i = 0; i < 4; i++)
		{
			hwndChild[i] = CreateWindow(
				szAppChildName[i], szAppChildName[i],
				WS_CHILDWINDOW | WS_BORDER | WS_VISIBLE | WS_VSCROLL,
				0, 0, 0, 0,
				hwnd, NULL, NULL, NULL
				);
			SetScrollRange(hwndChild[i], SB_VERT, 0, 255, true);
			SetScrollPos(hwndChild[i], SB_VERT, 0, true);
		}
		
		_beginthread(IncreasingNumber, 512, NULL);
		_beginthread(IncreasingPrimeNumber, 512, NULL);
		_beginthread(Fibonacci, 512, NULL);
		_beginthread(DrawCircle, 512, NULL);
		
		break;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		
		for (int i = 0; i < 4; i++)
		{
			MoveWindow(hwndChild[i], (i % 2) * (cxClient/2), (i / 2) * (cyClient/2), cxClient/2-10, cyClient/2-10, true);
		}

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK WndIncreasingNumber(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	hdc;
	PAINTSTRUCT	ls;
	TEXTMETRIC	tm;
	static TCHAR szBuffer[20];
	static int wnd1CxClient, wnd1CyClient;
	static int wnd1CxChar, wnd1CxCaps, wnd1CyChar;
	static int scrollBarInfo = 0;
	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);

		GetTextMetrics(hdc, &tm);

		wnd1CxChar = tm.tmAveCharWidth;

		wnd1CxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * wnd1CxChar / 2;

		wnd1CyChar = tm.tmHeight + tm.tmExternalLeading;


		ReleaseDC(hwnd, hdc);

		break;
	case WM_SIZE:
		wnd1CxClient = LOWORD(lParam);
		wnd1CyClient = HIWORD(lParam);
		break;
	case WM_VSCROLL:

		switch (LOWORD(wParam))

		{

		case   SB_PAGEDOWN:

			scrollBarInfo += 15;

			// fall through

		case   SB_LINEDOWN:

			scrollBarInfo += 1;

			break;



		case   SB_PAGEUP:

			scrollBarInfo -= 15;

			// fall through

		case   SB_LINEUP:

			scrollBarInfo -= 1;

			break;



		case   SB_TOP:

			scrollBarInfo = 0;

			break;



		case   SB_BOTTOM:

			scrollBarInfo = 255;

			break;


			
		case   SB_THUMBPOSITION:

		case   SB_THUMBTRACK:

			scrollBarInfo = HIWORD(wParam);

			break;
			


		default:

			break;

		}

		SetScrollPos(hwnd, SB_VERT, scrollBarInfo, TRUE);
		InvalidateRect(hwnd, NULL, true);

		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ls);
		for (int i = 0; i < 500; i++)
		{
			
			wsprintf(szBuffer, TEXT("%3d"), i);

			TextOut(hdc, wnd1CxClient / 2, wnd1CyChar*(i- scrollBarInfo)+2, szBuffer, sizeof(char)*3);

		}
		EndPaint(hwnd, &ls);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL IsPrimeNumber(int iPrime)
{
	for (int i = 2; i <iPrime-1; i++)
	{
		if (iPrime%i == 0)
		{
			return false;
		}
	}
	return true;
}
LRESULT CALLBACK WndIncreasingPrimeNumber(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	hdc;
	PAINTSTRUCT	ls;
	TEXTMETRIC	tm;
	static int scrollBarInfo;
	static TCHAR szBuffer[20];
	static int wnd3CxClient, wnd3CyClient;
	static int wnd3CxChar, wnd3CxCaps, wnd3CyChar;

	switch (message)
	{
	case WM_VSCROLL:

		switch (LOWORD(wParam))

		{

		case   SB_PAGEDOWN:

			scrollBarInfo += 15;

			// fall through

		case   SB_LINEDOWN:

			scrollBarInfo += 1;

			break;



		case   SB_PAGEUP:

			scrollBarInfo -= 15;

			// fall through

		case   SB_LINEUP:

			scrollBarInfo -= 1;

			break;



		case   SB_TOP:

			scrollBarInfo = 0;

			break;



		case   SB_BOTTOM:

			scrollBarInfo = 255;

			break;



		case   SB_THUMBPOSITION:

		case   SB_THUMBTRACK:

			scrollBarInfo = HIWORD(wParam);

			break;



		default:

			break;

		}

		SetScrollPos(hwnd, SB_VERT, scrollBarInfo, TRUE);
		InvalidateRect(hwnd, NULL, true);
		break;
	case WM_CREATE:
		hdc = GetDC(hwnd);

		GetTextMetrics(hdc, &tm);

		wnd3CxChar = tm.tmAveCharWidth;

		wnd3CxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * wnd3CxChar / 2;

		wnd3CyChar = tm.tmHeight + tm.tmExternalLeading;


		ReleaseDC(hwnd, hdc);

		break;
	case WM_SIZE:
		wnd3CxClient = LOWORD(lParam);
		wnd3CyClient = HIWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ls);

		for (int i = 0, iPrime = 2, iWidth; i < 500; i++)
		{
			if (!IsPrimeNumber(iPrime++))
			{
				i--;
				continue;
			}
			iWidth = wsprintf(szBuffer, TEXT("%3d"), iPrime-1);
			TextOut(hdc, wnd3CxClient / 2, wnd3CyChar*(i-scrollBarInfo) + 2, szBuffer, iWidth);
		}

		EndPaint(hwnd, &ls);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK WndFibonacci(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	hdc;
	PAINTSTRUCT	ls;
	TEXTMETRIC	tm;
	static int scrollBarInfo;
	static TCHAR szBuffer[20];
	static int wnd3CxClient, wnd3CyClient;
	static int wnd3CxChar, wnd3CxCaps, wnd3CyChar;

	switch (message)
	{
	case WM_VSCROLL:

		switch (LOWORD(wParam))

		{

		case   SB_PAGEDOWN:

			scrollBarInfo += 15;

			// fall through

		case   SB_LINEDOWN:

			scrollBarInfo += 1;

			break;



		case   SB_PAGEUP:

			scrollBarInfo -= 15;

			// fall through

		case   SB_LINEUP:

			scrollBarInfo -= 1;

			break;



		case   SB_TOP:

			scrollBarInfo = 0;

			break;



		case   SB_BOTTOM:

			scrollBarInfo = 255;

			break;



		case   SB_THUMBPOSITION:

		case   SB_THUMBTRACK:

			scrollBarInfo = HIWORD(wParam);

			break;



		default:

			break;

		}

		SetScrollPos(hwnd, SB_VERT, scrollBarInfo, TRUE);
		InvalidateRect(hwnd, NULL, true);
		break;
	case WM_CREATE:
		hdc = GetDC(hwnd);

		GetTextMetrics(hdc, &tm);

		wnd3CxChar = tm.tmAveCharWidth;

		wnd3CxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * wnd3CxChar / 2;

		wnd3CyChar = tm.tmHeight + tm.tmExternalLeading;


		ReleaseDC(hwnd, hdc);

		break;
	case WM_SIZE:
		wnd3CxClient = LOWORD(lParam);
		wnd3CyClient = HIWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ls);

		TextOut(hdc, wnd3CxClient / 2, 0, TEXT("  0"), sizeof(char)* 3);

		for (int i = 0, iPrev = 0, iTemp = 0, iCur = 1, iWidth = 0; i < 40; i++)
		{
			iTemp = iCur;
			iCur += iPrev;
			iPrev = iTemp;
			iWidth = wsprintf(szBuffer, TEXT("%3d"), iCur);
			TextOut(hdc, wnd3CxClient / 2, wnd3CyChar*(i + 1 - scrollBarInfo) + 2, szBuffer, iWidth);

		}
		EndPaint(hwnd, &ls);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK WndDrawCircle(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	hdc;
	PAINTSTRUCT	ls;
	TEXTMETRIC	tm;
	static TCHAR szBuffer[20];
	static int wnd3CxClient, wnd3CyClient;
	static int wnd3CxChar, wnd3CxCaps, wnd3CyChar;

	switch (message)
	{
	case WM_CREATE:

		break;
	case WM_SIZE:
		wnd3CxClient = LOWORD(lParam);
		wnd3CyClient = HIWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ls);

		Ellipse(hdc, 0, 0, wnd3CxClient, wnd3CyClient);
		EndPaint(hwnd, &ls);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}