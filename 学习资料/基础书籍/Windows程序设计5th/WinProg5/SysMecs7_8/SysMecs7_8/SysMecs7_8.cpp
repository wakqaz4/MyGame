//SYSMETS.C

/*----------------------------------------------------------------------------

SYSMETS.C -- Final System Metrics Display Program

(c) Charles Petzold, 1998

----------------------------------------------------------------------------*/

#include <windows.h>

#include "Sysmets.h"



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,

	PSTR szCmdLine, int iCmdShow)

{

	static TCHAR szAppName[] = TEXT("SysMets");

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

	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	wndclass.lpszMenuName = NULL;

	wndclass.lpszClassName = szAppName;



	if (!RegisterClass(&wndclass))

	{

		MessageBox(NULL, TEXT("Program requires Windows NT!"),

			szAppName, MB_ICONERROR);

		return 0;

	}



	hwnd = CreateWindow(szAppName, TEXT("Get System Metrics"),

		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,

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

	static int					  cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;

	static int					  iDeltaPerLine, iAccumDelta;         // for mouse wheel logic

	HDC							  hdc;

	int							  i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;

	PAINTSTRUCT					  ps;

	SCROLLINFO					  si;

	TCHAR                         szBuffer[10];

	TEXTMETRIC					  tm;

	ULONG                         ulScrollLines;       // for mouse wheel logic

	switch (message)

	{

	case   WM_CREATE:

		hdc = GetDC(hwnd);


		GetTextMetrics(hdc, &tm);

		cxChar = tm.tmAveCharWidth;

		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;

		cyChar = tm.tmHeight + tm.tmExternalLeading;



		ReleaseDC(hwnd, hdc);


		// Save the width of the three columns



		iMaxWidth = 40 * cxChar + 22 * cxCaps;


		// Fall through for mouse wheel information


	case   WM_SETTINGCHANGE:

		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);



		// ulScrollLines usually equals 3 or 0 (for no scrolling)

		// WHEEL_DELTA equals 120, so iDeltaPerLine will be 40


		if (ulScrollLines)

			iDeltaPerLine = WHEEL_DELTA / ulScrollLines;

		else

			iDeltaPerLine = 0;


		return 0;



	case   WM_SIZE:

		cxClient = LOWORD(lParam);

		cyClient = HIWORD(lParam);


		// Set vertical scroll bar range and page size


		si.cbSize = sizeof (si);

		si.fMask = SIF_RANGE | SIF_PAGE;

		si.nMin = 0;

		si.nMax = NUMLINES - 1;

		si.nPage = cyClient / cyChar;

		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);


		// Set horizontal scroll bar range and page size


		si.cbSize = sizeof (si);

		si.fMask = SIF_RANGE | SIF_PAGE;

		si.nMin = 0;

		si.nMax = 2 + iMaxWidth / cxChar;

		si.nPage = cxClient / cxChar;

		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

		return 0;



	case WM_VSCROLL:

		// Get all the vertical scroll bar information

		si.cbSize = sizeof (si);

		si.fMask = SIF_ALL;

		GetScrollInfo(hwnd, SB_VERT, &si);


		// Save the position for comparison later on

		iVertPos = si.nPos;

		switch (LOWORD(wParam))

		{

		case   SB_TOP:

			si.nPos = si.nMin;

			break;



		case   SB_BOTTOM:

			si.nPos = si.nMax;

			break;



		case SB_LINEUP:

			si.nPos -= 1;

			break;



		case   SB_LINEDOWN:

			si.nPos += 1;

			break;



		case SB_PAGEUP:

			si.nPos -= si.nPage;

			break;



		case   SB_PAGEDOWN:

			si.nPos += si.nPage;

			break;



		case   SB_THUMBTRACK:

			si.nPos = si.nTrackPos;

			break;

		default:

			break;

		}

		// Set the position and then retrieve it.  Due to adjustments

		//   by Windows it may not be the same as the value set.


		si.fMask = SIF_POS;

		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		GetScrollInfo(hwnd, SB_VERT, &si);


		// If the position has changed, scroll the window and update it


		if (si.nPos != iVertPos)

		{

			ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos),

				NULL, NULL);

			UpdateWindow(hwnd);

		}

		return 0;



	case   WM_HSCROLL:

		// Get all the vertical scroll bar information


		si.cbSize = sizeof (si);

		si.fMask = SIF_ALL;


		// Save the position for comparison later on


		GetScrollInfo(hwnd, SB_HORZ, &si);

		iHorzPos = si.nPos;


		switch (LOWORD(wParam))

		{

		case   SB_LINELEFT:

			si.nPos -= 1;

			break;



		case   SB_LINERIGHT:

			si.nPos += 1;

			break;


		case   SB_PAGELEFT:

			si.nPos -= si.nPage;

			break;


		case   SB_PAGERIGHT:

			si.nPos += si.nPage;

			break;



		case   SB_THUMBPOSITION:

			si.nPos = si.nTrackPos;

			break;



		default:

			break;

		}

		// Set the position and then retrieve it.  Due to adjustments

		//   by Windows it may not be the same as the value set.


		si.fMask = SIF_POS;

		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

		GetScrollInfo(hwnd, SB_HORZ, &si);



		// If the position has changed, scroll the window


		if (si.nPos != iHorzPos)

		{

			ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0,

				NULL, NULL);

		}

		return 0;


	case   WM_KEYDOWN:

		switch (wParam)

		{

		case   VK_HOME:

			SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);

			break;



		case   VK_END:

			SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);

			break;



		case   VK_PRIOR:

			SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);

			break;



		case   VK_NEXT:

			SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);

			break;


		case   VK_UP:

			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);

			break;



		case   VK_DOWN:

			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);

			break;



		case   VK_LEFT:

			SendMessage(hwnd, WM_HSCROLL, SB_PAGEUP, 0);

			break;



		case   VK_RIGHT:

			SendMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN, 0);

			break;

		}

		return 0;


	case   WM_MOUSEWHEEL:

		if (iDeltaPerLine == 0)

			break;


		iAccumDelta += (short)HIWORD(wParam);     // 120 or -120


		while (iAccumDelta >= iDeltaPerLine)

		{

			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);

			iAccumDelta -= iDeltaPerLine;

		}


		while (iAccumDelta <= -iDeltaPerLine)

		{

			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);

			iAccumDelta += iDeltaPerLine;

		}


		return 0;


	case   WM_PAINT:

		hdc = BeginPaint(hwnd, &ps);


		// Get vertical scroll bar position


		si.cbSize = sizeof (si);

		si.fMask = SIF_POS;

		GetScrollInfo(hwnd, SB_VERT, &si);

		iVertPos = si.nPos;


		// Get horizontal scroll bar position


		GetScrollInfo(hwnd, SB_HORZ, &si);

		iHorzPos = si.nPos;


		// Find painting limits


		iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);

		iPaintEnd = min(NUMLINES - 1,

			iVertPos + ps.rcPaint.bottom / cyChar);



		for (i = iPaintBeg; i <= iPaintEnd; i++)

		{

			x = cxChar * (1 - iHorzPos);

			y = cyChar * (i - iVertPos);



			TextOut(hdc, x, y,

				sysmetrics[i].szLabel,

				lstrlen(sysmetrics[i].szLabel));



			TextOut(hdc, x + 22 * cxCaps, y,

				sysmetrics[i].szDesc,

				lstrlen(sysmetrics[i].szDesc));



			SetTextAlign(hdc, TA_RIGHT | TA_TOP);



			TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,

				wsprintf(szBuffer, TEXT("%5d"),

				GetSystemMetrics(sysmetrics[i].Index)));



			SetTextAlign(hdc, TA_LEFT | TA_TOP);

		}


		EndPaint(hwnd, &ps);

		return 0;



	case   WM_DESTROY:

		PostQuitMessage(0);

		return 0;

	}

	return DefWindowProc(hwnd, message, wParam, lParam);

}
