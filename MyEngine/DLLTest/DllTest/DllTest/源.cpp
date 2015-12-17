#include "main.h"

extern "C"
{
	int __declspec(dllexport) test()
	{
		return 1;
	}
}