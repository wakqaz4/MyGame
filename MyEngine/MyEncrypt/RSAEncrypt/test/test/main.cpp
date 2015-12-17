#include <iostream>
#include <map>
#include <Windows.h>
#include <xmmintrin.h>
#include <queue>
using namespace std;

//Test with __m128
__declspec(align(16)) float A[] = { 2.0f, -1.0f, 3.0f, 4.0f };
__declspec(align(16)) float B[] = { -1.0f, 3.0f, 4.0f, 2.0f };
__declspec(align(16)) float C[] = { 0.0f, 0.0f, 0.0f, 0.0f };
__declspec(align(16)) float D[] = { 0.0f, 0.0f, 0.0f, 0.0f };

__m128 addWithAssembly(__m128 a, __m128 b)
{
	__m128 r;
	__asm
	{
		movaps xmm0, xmmword ptr[a]
			movaps xmm1, xmmword ptr[b]
			addps xmm0, xmm1
			movaps xmmword ptr[r], xmm0
	}
	return r;
}

__m128 addWithIntrinsics(__m128 a, __m128 b)
{
	__m128 r = _mm_add_ps(a, b);
	return r;
}


void __m128_test()
{
	__m128 a = _mm_load_ps(&A[0]);
	__m128 b = _mm_load_ps(&B[0]);

	__m128 c = addWithAssembly(a, b);
	__m128 d = addWithIntrinsics(a, b);

	_mm_store_ps(&A[0], a);
	_mm_store_ps(&B[0], b);

	_mm_store_ps(&C[0], c);
	_mm_store_ps(&D[0], d);

	int i = 0;
	for (float *a = &A[0]; i < 16; i++)
	{
		if (i % 4 == 0)
		{
			cout << endl;
		}
		cout << *a++ << endl;

	}
}





//reference and pointer
void RefAndPointerTest(void)
{
	int a = 1;
	int& refA = a;
	int* pointerA = &a;
	int& refRefA = refA;

	std::cout << "address of refA " << &refA << endl;
	std::cout << "value of refRefA " << refRefA << endl;

	std::cout << "value of pointerA " << pointerA << endl;
	std::cout << "address of pointerA " << &pointerA << endl;

	std::cout << "address of refRefA " << &refRefA<<endl;
}

//Function Pointer and Class Member Function Pointer
class Test
{
public:
	Test() :m_member(0){};
	virtual void TestFunc(){ std::cout << m_member << std::endl; };
public:
	int m_member;
};
void TestFunc()
{

}

void ClassFuncPointer(void)
{
	typedef void(Test::*pClassFunc)(void);
	typedef void(*pFunc)(void);
	
	pClassFunc pClassTestFunc = &(Test::TestFunc);
	cout << "Before assigning a instance:" << endl;
	cout << "Value of ClassTestFunc:" << pClassTestFunc << endl;
	cout << "Address of ClassTestFunc:" << &pClassTestFunc << endl;
	printf("Address of ClassTestFunc Pointer: %p \n\n", pClassTestFunc);

	pFunc pTestFunc = TestFunc;
	cout << "*pTestFunc " << *pTestFunc << endl;
	cout << "pTestFunc " << pTestFunc << endl;
	cout << "&pTestFunc " << &pTestFunc << endl;
	cout << endl;

	Test test1;
	Test test2;
	test1.m_member = 1;
	test2.m_member = 2;

	cout << "Address of TestFunc:" << pTestFunc << endl;
	cout << "Value of ClassTestFunc:" << pClassTestFunc << endl;
	cout << "Address of ClassTestFunc:" << &pClassTestFunc << endl;
	(test1.*pClassTestFunc)();
	(test2.*pClassTestFunc)();	
}

//Put classFunctionPointer into a container...such as: std::map
class CBase1
{
	CBase1():
	m_testNum(0)
	{};

	~CBase1(){};
public:
	friend void MapAndCFuncPtr();

public:
	void Test1(){ std::cout << "CBase1::Test1" << endl << m_testNum << endl; };
private:
	int m_testNum;
};

class CBase2
{
	CBase2() :
	m_testNum(0)
	{};

	~CBase2(){};

public:
	void Test2(){ std::cout << "CBase2::Test2 " << endl << m_testNum << endl; }
private:
	int m_testNum;

public:


};

void test1()
{};
void test2()
{};

void MapAndCFuncPtr(void)
{
	typedef void(CBase1::*pClassFunc1)(void);
	typedef void(CBase2::*pClassFunc2)(void);
	typedef void(*pFunc)(void);

	std::map<pClassFunc1, pClassFunc2> m_map;
	pClassFunc1 pf1 = &(CBase1::Test1);
	if (pf1 == pClassFunc1(pf1))
	{
		cout << "OK" << endl;
	}
	else
	{
		cout << pf1 << endl;
		cout << pClassFunc1(pf1) << endl;
	}
	
	pClassFunc1* temp1 = &pf1;

	std::map<pFunc, pFunc> mmap;
//	mmap[&test1] = &test2;
//	(&test1).~pFunc();
	pair<pClassFunc1, pClassFunc2> par(pf1, &(CBase2::Test2));
//	m_map.insert(par);

	std::cout << std::boolalpha << "T is copy-constructible? "<< std::is_copy_constructible<pClassFunc1>::value << endl;

}

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
using namespace std;


class CTest1
{
public:
	int initBool : 1;
	int var1 : 7;
	int var2 : 31;
};

int main()
{
	CTest1 a;	
	a.initBool = 2;
	a.var1 = 21;
	a.var2 = 123;
	std::cout << a.initBool << endl;
	std::cout << sizeof(a) << endl;
	//__m128_test();	

	//ClassFuncPointer();

	//MapAndCFuncPtr();	

	return 0;
}