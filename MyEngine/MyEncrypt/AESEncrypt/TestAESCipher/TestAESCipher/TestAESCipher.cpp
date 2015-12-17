///////////////////////////////
// http://mingcn.cnblogs.com //
//  xelz CopyRight (c) 2010  //
///////////////////////////////

#include "stdafx.h"
#include "AES.h"
#include <windows.h>
#include <iostream>
#pragma comment(lib, "AESDLL.lib")

void print(unsigned char* state);

int main(int argc, char* argv[])
{
	//unsigned char input[] = 
	//{
	//	"nishinaocan"
	//};
	//unsigned char key[] = 
	//{
	//	"guangrongshiming"
	//};
	//AES aes(key);
	//
	//printf("Input:\n");
	//print(input);

	//aes.Cipher(input);
	//printf("After Cipher:\n");
	//print(input);

	//aes.InvCipher(input);
	//printf("After InvCipher:\n");
	//print(input);

	//int j;
	//printf("\n");
	//char str[32] = "Hello,My AES Cipher!";
	//for(j=0; j<32; j++)printf("%X ",(unsigned char)str[j]);
	//printf("\n");
	//aes.Cipher((void *)str);
	//for(j=0; j<32; j++)printf("%X ",(unsigned char)str[j]);
	//printf("\n");
	//aes.InvCipher((void *)str,21);
	//for(j=0; j<32; j++)printf("%X ",(unsigned char)str[j]);
	//printf("\n");
	//return 0;
	HMODULE aesDll;
	aesDll = LoadLibrary("AESDLL.dll");
	if (!aesDll)
	{
		std::cout<<"Loading DLL error!"<<std::endl;
		return 0 ;
	}
	typedef AES*(* pfun)(unsigned char*);
	pfun pFunc;
	pFunc = (pfun)GetProcAddress(aesDll, "CreateAES");
	unsigned char tempCh[] = "guangrongshiming";
	AES* temp = pFunc(tempCh);

	FreeLibrary(aesDll);
	return 0;
}

void print(unsigned char* state)
{
	int i;
	for(i=0; i<16; i++)
	{
		printf("%s%X ",state[i]>15 ? "" : "0", state[i]);
	}
	printf("\n");
}

