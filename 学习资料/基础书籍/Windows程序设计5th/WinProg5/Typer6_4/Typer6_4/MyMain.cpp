/*
Copied from source.cpp, in order to practice.
*/

#include <windows.h>
#define BUFFER(x,y) *(pBuffer + y * cxBuffer + x)

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR  szAppName[] = TEXT("Typer");

	HWND                  hwnd;

	MSG                   msg;

	WNDCLASS              wndclass;



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

		MessageBox(NULL, TEXT("This program requires Windows NT!"),

			szAppName, MB_ICONERROR);

		return 0;

	}

	hwnd = CreateWindow(szAppName, TEXT("Typing Program"),

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

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static DWORD			dwCharSet = DEFAULT_CHARSET;
	static int				cxChar, cyChar, cxClient, cyClient, cxBuffer, cyBuffer, xCaret, yCaret;
	static TCHAR			*pBuffer = NULL;
	HDC						hdc;
	int						x, y, i;
	PAINTSTRUCT				ps;
	TEXTMETRIC				tm;

	switch (message)
	{
		case WM_INPUTLANGCHANGE:
		
			dwCharSet = wParam;
		
		case WM_CREATE:
			hdc = GetDC(hwnd);
			SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0,

				dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));
			GetTextMetrics(hdc, &tm);
			cxChar = tm.tmAveCharWidth;
			cyChar = tm.tmHeight;

			DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
			ReleaseDC(hwnd, hdc);
			break;

		case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			cxBuffer = max(1, cxClient / cxChar);
			cyBuffer = max(1, cyClient / cyChar);

			if (pBuffer != NULL)
			{
				free(pBuffer);
			}
			pBuffer = (TCHAR*)malloc(cxBuffer*cyBuffer*sizeof(TCHAR));
			for (y = 0; y < cyBuffer; y++)
			{
				for (x = 0; x < cxBuffer; x++)
				{
					BUFFER(x, y) = ' ';
				}
			}
			xCaret = 0;
			yCaret = 0;

			if (hwnd = GetFocus())
			{
				SetCaretPos(xCaret*cxChar, yCaret*cyChar);
			}
			InvalidateRect(hwnd, NULL, true);
			return 0;

		case WM_SETFOCUS:
			CreateCaret(hwnd, NULL, cxChar, cyChar);
			SetCaretPos(xCaret*cxChar, yCaret*cyChar);
			ShowCaret(hwnd);
			return 0;

		case WM_KILLFOCUS:
			HideCaret(hwnd);
			DestroyCaret();
			return 0;

		case WM_PAINT:

			break;

		case WM_CHAR:

			for (i = 0; i < (int)LOWORD(lParam); i++)

			{

				switch (wParam)

				{

				case '\b':                                           // backspace

					if (xCaret > 0)

					{

						xCaret--;

						SendMessage(hwnd, WM_KEYDOWN, VK_DELETE, 1);

					}

					break;



				case '\t':                                                  // tab

					do

					{

						SendMessage(hwnd, WM_CHAR, ' ', 1);

					}

					while (xCaret % 8 != 0);

					break;

				case '\n':                                                  // line feed

					if (++yCaret == cyBuffer)

						yCaret = 0;

					break;



				case '\r':                                                  // carriage return

					xCaret = 0;



					if (++yCaret == cyBuffer)

						yCaret = 0;

					break;



				case '\x1B':                                       // escape

					for (y = 0; y < cyBuffer; y++)

					for (x = 0; x < cxBuffer; x++)

						BUFFER(x, y) = ' ';



					xCaret = 0;

					yCaret = 0;

					InvalidateRect(hwnd, NULL, FALSE);

					break;

				default:                                                    // character codes

					BUFFER(xCaret, yCaret) = (TCHAR)wParam;

					HideCaret(hwnd);

					hdc = GetDC(hwnd);

					SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0,

						dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));

					TextOut(hdc, xCaret * cxChar, yCaret * cyChar,

						&BUFFER(xCaret, yCaret), 1);

					DeleteObject(

						SelectObject(hdc, GetStockObject(SYSTEM_FONT)));

					ReleaseDC(hwnd, hdc);

					ShowCaret(hwnd);


					if (++xCaret == cxBuffer)

					{

						xCaret = 0;

						if (++yCaret == cyBuffer)

							yCaret = 0;

					}

					break;

				}

			}
			SetCaretPos(xCaret * cxChar, yCaret * cyChar);
			return 0;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_TAB:
				if (GetKeyState(VK_LCONTROL))
				{
					MessageBox(hwnd, TEXT("jUST TRY"), TEXT("ANOTHER SHOT"), 0);
				}
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}