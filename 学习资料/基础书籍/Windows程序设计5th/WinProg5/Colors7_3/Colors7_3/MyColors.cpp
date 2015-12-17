//MyColors.C

/*------------------------------------------------------------------------

Copied from Colors.cpp, to practice

-------------------------------------------------------------------------*/

#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ScrollProc(HWND, UINT, WPARAM, LPARAM);


int     idFocus;

WNDPROC OldScroll[3];


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,

	PSTR szCmdLine, int iCmdShow)

{

	static TCHAR szAppName[] = TEXT("Colors1");

	HWND                                 hwnd;

	MSG                                  msg;

	WNDCLASS                      wndclass;



	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	wndclass.lpfnWndProc = WndProc;

	wndclass.cbClsExtra = 0;

	wndclass.cbWndExtra = 0;

	wndclass.hInstance = hInstance;

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	wndclass.hbrBackground = CreateSolidBrush(0);

	wndclass.lpszMenuName = NULL;

	wndclass.lpszClassName = szAppName;



	if (!RegisterClass(&wndclass))

	{

		MessageBox(NULL, TEXT("This program requires Windows NT!"),

			szAppName, MB_ICONERROR);

		return 0;

	}

	hwnd = CreateWindow(szAppName, TEXT("Color Scroll"),

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
	static COLORREF crPrim[3] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255) };

	static HBRUSH         hBrush[3], hBrushStatic;

	static HWND           hwndScroll[3], hwndLabel[3], hwndValue[3], hwndRect;

	static int            color[3], cyChar;

	static RECT           rcColor;

	static TCHAR *szColorLabel[] = { TEXT("Red"), TEXT("Green"),

		TEXT("Blue") };

	HINSTANCE                     hInstance;

	int                           i, cxClient, cyClient;

	TCHAR                         szBuffer[10];
	static int dev_id;

	int wndWidth;
	int wndHeight;
	int colorTextWidth = 40;
	int colorTextHeight = 15;
	int colorPixelNumWidth = 40;
	int colorPixelNumHeight = 15;
	int colorScrollBarWidth = 40;
	int colorScrollBarHeight;

	switch (message)
	{
	case WM_CREATE:

		hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		
		hwndRect = CreateWindow(TEXT("static"), NULL,

			WS_CHILD | WS_VISIBLE | SS_WHITERECT,

			0, 0, 0, 0,

			hwnd, (HMENU)9, hInstance, NULL);
		/*
		hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		hwndRect = CreateWindow(TEXT("static"), NULL,

			WS_CHILD | WS_VISIBLE | SS_WHITERECT,

			0, 0, 0, 0,

			hwnd, (HMENU)9, hInstance, NULL);
			*/
		for (i = 0; i < 3; i++)
		{

				hwndLabel[i] = CreateWindow(
				TEXT("static"),
				szColorLabel[i],
				WS_CHILD|WS_VISIBLE|SS_CENTER,
				0, 0, 0, 0,
				hwnd,
				(HMENU)(i + 3),
				hInstance,
				NULL
				);

				hwndScroll[i] = CreateWindow(
					TEXT("scrollbar"),
					NULL,
					WS_CHILD | WS_VISIBLE | WS_TABSTOP | SBS_VERT,
					0, 0, 0, 0,
					hwnd,
					(HMENU)(i + 0),
					hInstance,
					NULL
					);

				SetScrollRange(hwndScroll[i], SB_CTL, 0, 255, FALSE);
				SetScrollPos(hwndScroll[i], SB_CTL, 0, FALSE);

				hwndValue[i] = CreateWindow(
					TEXT("static"),
					NULL,
					WS_CHILD | WS_VISIBLE | SS_CENTER,
					0, 0, 0, 0,
					hwnd,
					(HMENU)(i + 6),
					hInstance,
					NULL
					);
				OldScroll[i] = (WNDPROC)SetWindowLong(hwndScroll[i], GWL_WNDPROC, (LONG)ScrollProc);
				hBrush[i] = CreateSolidBrush(crPrim[i]);
		}
		//To be explained...
		
		hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));

		cyChar = HIWORD(GetDialogBaseUnits());
		
		return 0;
		

	case WM_SIZE:

		SetRect(&rcColor, LOWORD(lParam) / 2, 0, LOWORD(lParam), HIWORD(lParam));
		wndWidth = LOWORD(lParam);
		wndHeight = HIWORD(lParam);
		colorScrollBarHeight = wndHeight*8/10;
		MoveWindow(hwndRect, 0, 0, wndWidth/2, wndHeight, true);
		for (int i = 0; i < 3; i++)
		{
			
			MoveWindow(hwndLabel[i], wndWidth*i / 6, wndHeight / 10, colorTextWidth, colorTextHeight, true);
			MoveWindow(hwndScroll[i], wndWidth*i / 6, wndHeight / 8, colorScrollBarWidth, colorScrollBarHeight, true);
			MoveWindow(hwndValue[i], wndWidth*i / 6, wndHeight * 9.5 / 10, colorPixelNumWidth, colorPixelNumHeight, true);
			

		}		
		SetFocus(hwnd);
		return 0;
		
	case WM_SETFOCUS:
		SetFocus(hwndScroll[idFocus]);
		return 0;

	case WM_VSCROLL:

		i = GetWindowLong((HWND)lParam, GWL_ID);

		switch (LOWORD(wParam))

		{

		case   SB_PAGEDOWN:

			color[i] += 15;

			// fall through

		case   SB_LINEDOWN:

			color[i] = min(255, color[i] + 1);

			break;



		case   SB_PAGEUP:

			color[i] -= 15;

			// fall through

		case   SB_LINEUP:

			color[i] = max(0, color[i] - 1);

			break;



		case   SB_TOP:

			color[i] = 0;

			break;



		case   SB_BOTTOM:

			color[i] = 255;

			break;



		case   SB_THUMBPOSITION:

		case   SB_THUMBTRACK:

			color[i] = HIWORD(wParam);

			break;



		default:

			break;

		}

		SetScrollPos(hwndScroll[i], SB_CTL, color[i], TRUE);

		wsprintf(szBuffer, TEXT("%i"), color[i]);

		SetWindowText(hwndValue[i], szBuffer);



		DeleteObject((HBRUSH)

			SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)

			CreateSolidBrush(RGB(color[0], color[1], color[2]))));



		InvalidateRect(hwnd, &rcColor, TRUE);

		return 0;

		/*
		dev_id = GetWindowLong((HWND)lParam, GWL_ID);

		switch (LOWORD(wParam))

		{

		case   SB_PAGEDOWN:

			color[dev_id] += 15;

			// fall through

		case   SB_LINEDOWN:

			color[dev_id] = min(255, color[dev_id] + 1);

			break;



		case   SB_PAGEUP:

			color[dev_id] -= 15;

			// fall through

		case   SB_LINEUP:

			color[dev_id] = max(0, color[dev_id] - 1);

			break;



		case   SB_TOP:

			color[dev_id] = 0;

			break;



		case   SB_BOTTOM:

			color[dev_id] = 255;

			break;



		case   SB_THUMBPOSITION:

		case   SB_THUMBTRACK:

			color[dev_id] = HIWORD(wParam);

			break;



		default:

			break;

		}

		SetScrollPos(hwndScroll[dev_id], SB_CTL, color[dev_id], TRUE);

		wsprintf(szBuffer, TEXT("%i"), color[dev_id]);

		SetWindowText(hwndValue[dev_id], szBuffer);



		DeleteObject((HBRUSH)

			SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)

			CreateSolidBrush(RGB(color[0], color[1], color[2]))));



		InvalidateRect(hwnd, &rcColor, TRUE);

		return 0;

		*/
	case   WM_CTLCOLORSCROLLBAR:

		i = GetWindowLong((HWND)lParam, GWL_ID);

		return (LRESULT)hBrush[i];



	case   WM_CTLCOLORSTATIC:

		i = GetWindowLong((HWND)lParam, GWL_ID);



		if (i >= 3 && i <= 8)                       // static text controls

		{

			SetTextColor((HDC)wParam, crPrim[i % 3]);

			SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNHIGHLIGHT));

			return (LRESULT)hBrushStatic;

		}

		break;

	case   WM_SYSCOLORCHANGE:

		DeleteObject(hBrushStatic);

		hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));

		return 0;

	case   WM_DESTROY:

		DeleteObject((HBRUSH)

			SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)

			GetStockObject(WHITE_BRUSH)));



		for (i = 0; i < 3; i++)

			DeleteObject(hBrush[i]);



		DeleteObject(hBrushStatic);

		PostQuitMessage(0);

		return 0;

	}

	return DefWindowProc(hwnd, message, wParam, lParam);

}
LRESULT CALLBACK ScrollProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetParent(hwnd),

				(id + (GetKeyState(VK_SHIFT) < 0 ? 2 : 1)) % 3));
			break;
		default:
			break;
		}
	case WM_SETFOCUS:
		idFocus = id;
		break;
	}

	return CallWindowProc(OldScroll[id], hwnd, message, wParam, lParam);	

}