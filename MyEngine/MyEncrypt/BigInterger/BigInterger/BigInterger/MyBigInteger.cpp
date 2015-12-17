#include <iostream>
#include "MyBigInteger.h"
using namespace std;

CMyBigInteger::CMyBigInteger()
{
	m_sourceString = new char[m_maxLength];

};
CMyBigInteger::CMyBigInteger(char[])
{};
CMyBigInteger::CMyBigInteger(string)
{};
CMyBigInteger::CMyBigInteger(int)
{};

CMyBigInteger CMyBigInteger::Add(CMyBigInteger number1, CMyBigInteger number2)
{};
CMyBigInteger CMyBigInteger::Minus(CMyBigInteger number1, CMyBigInteger number2)
{};

CMyBigInteger CMyBigInteger::Multiply(CMyBigInteger number1, CMyBigInteger number2)
{};
CMyBigInteger CMyBigInteger::Divide(CMyBigInteger number1, CMyBigInteger number2)
{};
CMyBigInteger CMyBigInteger::Power(CMyBigInteger number2)
{};
CMyBigInteger CMyBigInteger::Mod(CMyBigInteger number1, CMyBigInteger number2)
{};

CMyBigInteger CMyBigInteger::operator+ (CMyBigInteger number1)
{};
CMyBigInteger CMyBigInteger::operator- (CMyBigInteger number1)
{};
CMyBigInteger CMyBigInteger::operator* (CMyBigInteger number1)
{};
CMyBigInteger CMyBigInteger::operator/ (CMyBigInteger number1)
{};
CMyBigInteger CMyBigInteger::operator% (CMyBigInteger number1)
{};

