#include <iostream>
using namespace std;

class Test
{
	int a;
	static int b;
	virtual void testFunc(){};
};

int main()
{
	unsigned int u1 = 10, u2 = 42;
	cout << u1 - u2 << endl;
	cout << u2 - u1 << endl;

	int i = 10, i2 = 42;
	cout << i2 - i << endl;
	cout << i - i2 << endl;

	cout << i - u1 << endl;
	cout << u1 - i << endl;

	cout << "\12 this is a new line with backSlash 12 \12";
	/**
	 *	\7 bell
	 *	\12 newline
	 *	\40 blank
	 *	\0 null
	 *	\115 'M'
	 *	\x4d
	 *	*/
	cout << "\\7" << "\7";
	cout << "\\12" << "\12";
	cout << "\\40" << "\40";
	cout << "\\0" << "\0";
	cout << "\\115" << "\115";
	cout << "\\x4d" << "\x4d";
	cout << "\x10" << endl;
	cout << 3.14e1L << endl;
	cout << 3.14L <<sizeof(3.14f)<<sizeof(3.14L)<< endl;
	cout << "\v\115\12"<< "\02 \t\115\12" << endl;

	int aa;
	cout << sizeof(aa) << endl;

	int var1{ 123 };
	float var2(3.3);
	int var3{ (int)var2 };

	return 0;
}