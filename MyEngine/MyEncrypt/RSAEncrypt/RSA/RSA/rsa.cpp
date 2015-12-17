#include <string>
#include <iostream>
using namespace std;

static string MULTIPLY(string number1, string number2)
{
	int i, j;
	//restore result string
	int *iresult;
	int length1 = number1.size();
	int length2 = number2.size();
	string result = "";

	reverse(number1.begin(), number1.end());
	reverse(number2.begin(), number2.end());

	iresult = (int*)malloc(sizeof(int)* (length1 + length2 + 1));
	memset(iresult, 0, sizeof(int)* (length1 + length2 + 1));

	for (i = 0; i < length1; i++)

	{

		for (j = 0; j < length2; j++)

		{

			iresult[i + j] += ((number1[i] - 48) * (number2[j] - 48));

		}

	}



	int carry = 0;

	for (i = 0; i < length1 + length2; i++)

	{

		int value = iresult[i] + carry;

		iresult[i] = value % 10;

		carry = value / 10;

	}



	for (i = length1 + length2 - 1; i >= 0; i--)

	{

		if (iresult[i] != 0)break;

	}



	for (; i >= 0; i--)

	{

		result = result + (char)(iresult[i] + 48);

	}



	free(iresult);



	if (result == "") result = "0";
	return result;
}