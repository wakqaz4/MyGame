//KEYVIEW1.C

/*---------------------------------------------------------------------

KEYVIEW1.C --Displays Keyboard and Character Messages

(c) Charles Petzold, 1998

---------------------------------------------------------------------*/

#include <windows.h>

#define NUMLINES 1000

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,

	PSTR szCmdLine, int iCmdShow)

{

	static TCHAR szAppName[] = TEXT("KeyView1");

	HWND                  hwnd;

	MSG                   msg;

	WNDCLASS              wndclass;



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



	hwnd = CreateWindow(szAppName, TEXT("Keyboard Message Viewer #1"),

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

	static int   cxClientMax, cyClientMax, cxClient, cyClient, cxChar, cyChar;

	static int   cLinesMax, cLines;

	static PMSG  pmsg;

	static RECT  rectScroll;

	static TCHAR szTop[] = TEXT("辈数        姓名      性别     ");

	static TCHAR szUnd[] = TEXT("_______        ___       ____     ");


	static TCHAR * szFormat[2] = {

		TEXT("%-13s %3d %-15s%c%6u %4d %3s %3s %4s %4s"),

		TEXT("%-13s  0x%04X%1s%c %6u %4d %3s %3s %4s %4s") };

	static TCHAR * szYes = TEXT("Yes");

	static TCHAR * szNo = TEXT("No");

	static TCHAR * szDown = TEXT("Down");

	static TCHAR * szUp = TEXT("Up");

	static DWORD			dwCharSet = DEFAULT_CHARSET;

	static TCHAR * szMessage[] = {

		TEXT("WM_KEYDOWN"), TEXT("WM_KEYUP"),

		TEXT("WM_CHAR"), TEXT("WM_DEADCHAR"),

		TEXT("WM_SYSKEYDOWN"), TEXT("WM_SYSKEYUP"),

		TEXT("WM_SYSCHAR"), TEXT("WM_SYSDEADCHAR") };

	HDC                   hdc;

	int                   i, iType;

	PAINTSTRUCT           ps;

	TCHAR                 szBuffer[128], szKeyName[32];

	TEXTMETRIC            tm;

	static int iVscrollPos;
	static int iHscrollPos;


	static int iGeneration = 1;

	switch (message)

	{

	case WM_CREATE:
		SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);

		SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
		break;

	case WM_HSCROLL:

		switch (LOWORD(wParam))

		{
		case SB_LEFT:

			iHscrollPos -= 1;

			break;



		case SB_RIGHT:

			iHscrollPos += 1;

			break;



		case SB_THUMBPOSITION:

			iHscrollPos = HIWORD(wParam);

			break;



		default:

			break;

		}


		iHscrollPos = max(0, min(iHscrollPos, NUMLINES - 1));

		if (iHscrollPos != GetScrollPos(hwnd, SB_HORZ))

		{

			SetScrollPos(hwnd, SB_HORZ, iHscrollPos, TRUE);

			InvalidateRect(hwnd, NULL, TRUE);

		}

		return 0;

	case WM_VSCROLL:

		switch (LOWORD(wParam))

		{

		case SB_LINEUP:

			iVscrollPos -= 1;

			break;



		case SB_LINEDOWN:

			iVscrollPos += 1;

			break;


		case SB_PAGEUP:

			iVscrollPos -= cyClient / cyChar;

			break;



		case SB_PAGEDOWN:

			iVscrollPos += cyClient / cyChar;

			break;



		case SB_THUMBPOSITION:

			iVscrollPos = HIWORD(wParam);

			break;



		default:

			break;

		}


		iVscrollPos = max(0, min(iVscrollPos, NUMLINES - 1));

		if (iVscrollPos != GetScrollPos(hwnd, SB_VERT))

		{

			SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);

			InvalidateRect(hwnd, NULL, TRUE);

		}

		return 0;

	case WM_DISPLAYCHANGE:

		// Get maximum size of client area

		cxClientMax = GetSystemMetrics(SM_CXMAXIMIZED);

		cyClientMax = GetSystemMetrics(SM_CYMAXIMIZED);


		// Get character size for fixed-pitch font

		hdc = GetDC(hwnd);

		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

		GetTextMetrics(hdc, &tm);

		cxChar = tm.tmAveCharWidth;

		cyChar = tm.tmHeight;


		ReleaseDC(hwnd, hdc);

		// Allocate memory for display lines

		if (pmsg)

			free(pmsg);

		cLinesMax = cyClientMax / cyChar;

		pmsg = (PMSG) malloc(cLinesMax * sizeof (MSG));

		cLines = 0;

		// fall through

	case WM_SIZE:

		if (message == WM_SIZE)

		{

			cxClient = LOWORD(lParam);

			cyClient = HIWORD(lParam);

		}

		// Calculate scrolling rectangle

		rectScroll.left = 0;

		rectScroll.right = cxClient;

		rectScroll.top = cyChar;

		rectScroll.bottom = cyChar * (cyClient / cyChar);


		InvalidateRect(hwnd, NULL, TRUE);

		return 0;

	case WM_PAINT:

		hdc = BeginPaint(hwnd, &ps);

		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

		SetBkMode(hdc, TRANSPARENT);

		TextOut(hdc, 0, 0, szTop, lstrlen(szTop));

		TextOut(hdc, 0, 0, szUnd, lstrlen(szUnd));

//		/*
		for (i = 0; i < NUMLINES; i++)
		{
			TextOut(hdc, 0, (cyClient / cyChar - 1 - i) * cyChar, szBuffer, 
				wsprintf(szBuffer, TEXT("%d代目    XXX   xxx"), i));
		}
//		*/
		/*
		for (i = 0; i < min(cLines, cyClient / cyChar - 1); i++)

		{

			iType = pmsg[i].message == WM_CHAR ||

				pmsg[i].message == WM_SYSCHAR ||

				pmsg[i].message == WM_DEADCHAR ||

				pmsg[i].message == WM_SYSDEADCHAR;


			GetKeyNameText(pmsg[i].lParam, szKeyName,

				sizeof (szKeyName) / sizeof (TCHAR));


			TextOut(hdc, 0, (cyClient / cyChar - 1 - i) * cyChar, szBuffer,

				wsprintf(szBuffer, szFormat[iType],

				szMessage[pmsg[i].message - WM_KEYFIRST],

				pmsg[i].wParam,

				(PTSTR)(iType ? TEXT(" ") : szKeyName),

				(TCHAR)(iType ? pmsg[i].wParam : ' '),

				LOWORD(pmsg[i].lParam),

				HIWORD(pmsg[i].lParam) & 0xFF,

				0x01000000 & pmsg[i].lParam ? szYes : szNo,

				0x20000000 & pmsg[i].lParam ? szYes : szNo,

				0x40000000 & pmsg[i].lParam ? szDown : szUp,

				0x80000000 & pmsg[i].lParam ? szUp : szDown));

		}
		*/

		EndPaint(hwnd, &ps);

		return 0;


	case   WM_DESTROY:

		PostQuitMessage(0);

		return 0;

	}

	return DefWindowProc(hwnd, message, wParam, lParam);

}
