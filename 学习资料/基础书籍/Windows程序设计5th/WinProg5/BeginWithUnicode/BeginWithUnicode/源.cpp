#include <cstdio>
#include <iostream>
using namespace std;

int main()
{
	wchar_t *pw = L"Hello!";
	int iLength;
	iLength = wcslen(pw);
	cout << "The string length of char is " << iLength << endl;

}