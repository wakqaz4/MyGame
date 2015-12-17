#include "main.h"
#include <Windows.h>
#include <iostream>

int main()
{
	HMODULE hDll;
	hDll = LoadLibrary("DLLTest.dll");
	if (!hDll)
	{
		std::cout << "Loading DLL failed!" << std::endl;
		return 0;
	}
	typedef int(*pFun)();
	pFun ret;
	ret = (pFun)GetProcAddress(hDll, "test");
	int res = 0;
	res = ret();
	std::cout << "Loading Dll succeeded! the result is " << res << std::endl;
	return 1;
}