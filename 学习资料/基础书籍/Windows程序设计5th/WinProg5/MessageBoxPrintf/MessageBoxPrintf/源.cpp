#include <windows.h>
#include <tchar.h>
#include <stdio.h>

int MessageBoxPrintf(TCHAR* szCaption, TCHAR* szFormat, ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	
	va_start(pArgList, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer), szFormat, pArgList);
	va_end(pArgList);
	return MessageBox(NULL, szBuffer, szCaption, 0);

}
int CALLBACK WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	int cxScreen, cyScreen;
	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN); 

	MessageBoxPrintf(_T("ScrnSize"),
		_T("The Screen is %d pixels wide by %d pixels high."),
		cxScreen, cyScreen);
	return 0;
}