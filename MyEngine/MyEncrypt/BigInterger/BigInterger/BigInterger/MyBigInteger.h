#ifndef _MY_BIG_INTEGER_
#define _MY_BIG_INTEGER_
#include <iostream>
#include <string>
using namespace std;

class CMyBigInteger
{



public:
	CMyBigInteger();
	CMyBigInteger(char[]);
	CMyBigInteger(string);
	CMyBigInteger(int);	
	
	static CMyBigInteger Add(CMyBigInteger number1, CMyBigInteger number2);
	static CMyBigInteger Minus(CMyBigInteger number1, CMyBigInteger number2);
	static CMyBigInteger Multiply(CMyBigInteger number1, CMyBigInteger number2);
	static CMyBigInteger Divide(CMyBigInteger number1, CMyBigInteger number2);
	static CMyBigInteger Power(CMyBigInteger number2);
	static CMyBigInteger Mod(CMyBigInteger number1, CMyBigInteger number2);

	static CMyBigInteger operator+ (CMyBigInteger number1);
	static CMyBigInteger operator- (CMyBigInteger number1);
	static CMyBigInteger operator* (CMyBigInteger number1);
	static CMyBigInteger operator/ (CMyBigInteger number1);
	static CMyBigInteger operator% (CMyBigInteger number1);

private:
	char* m_sourceString;
	const int m_maxLength = 128;
	int m_actualLength;
};


#endif