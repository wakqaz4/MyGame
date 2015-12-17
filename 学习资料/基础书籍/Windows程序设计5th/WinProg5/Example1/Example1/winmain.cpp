#include <windows.h>
#include <tchar.h>

int _stdcall WinMain(HINSTANCE hinstance, HINSTANCE prevHinstance, PSTR szcmdline, int cmdshow)
{
	MessageBox(NULL, _T("Neverdie"), _T("Kill Me Now"), MB_OK | MB_ICONEXCLAMATION);
}