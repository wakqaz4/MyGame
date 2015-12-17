#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace std;

DWORD WINAPI Printer_Thread(LPVOID data)
{
	for (int index = 0; index < 25; index++)
	{
		cout << (int)data;
		Sleep(100);
	}
	cout << endl;
	return (DWORD)data;
}

DWORD WINAPI printer_thread2(LPVOID data)
{
	for (int index = 0; index < 25; index++)
	{
		cout << data << endl;
	}
	return (DWORD)data;
}

void main(void)
{
	HANDLE thread_handle;
	DWORD thread_id;

	cout << "\nStarting threads...\n";
	thread_handle = CreateThread(NULL,
		0,
		Printer_Thread,
		(LPVOID)1,
		0,
		&thread_id);
	CreateThread(NULL,
		0,
		printer_thread2,
		(LPVOID)45,
		0,
		&thread_id);

	for (int index = 0; index < 25; index++)
	{
		cout << 2;
		Sleep(100);
	}
	cout << endl;

	CloseHandle(thread_handle);
	
	cout << "\n All threads are terminated!\n" << endl;
	return;

}



