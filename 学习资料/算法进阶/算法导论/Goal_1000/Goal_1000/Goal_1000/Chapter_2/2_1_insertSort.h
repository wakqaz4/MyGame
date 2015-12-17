

void InsertSort(int* source, int count)
{
	for (int index = 1; index < count; index++)
	{
		int currElement = source[index];
		int i = index - 1;
		for (; i >= 0; i--)
		{
			if (currElement < source[i])
			{
				source[i + 1] = source[i];
			}
			else
			{
				break;
			}
		}
		source[i+1] = currElement;
	}
}

//assume that the righter part is a well-done serial, while the lefter part is a random serial
void InsertSort_1(int* source, int count)
{	
	for (int index = count - 2; index >= 0; --index)
	{
		int currElement = source[index];
		int i = index + 1;
		for (;( i <= count - 1) && (currElement > source[i]); ++i)
		{
			source[i - 1] = source[i];
		}
		source[i - 1] = currElement;
	}
}

//when compared, compare from left to right;
void InsertSort_3(int* source, int count)
{
	int index;
	if (count <= 1)
	{
		return;
	}
	for (index = 1; index < count; index++)
	{
		int currElement = source[index];
		int i = 0;
		for (; (i < index) && (currElement>source[i]); ++i)
		{

		}
		if (i != index)
		{
			for (int j = index - 1; j >= i; --j)
			{
				source[j + 1] = source[j];
			}
			source[i] = currElement;
		}		
	}
}

//2-1-4 : Binary_add
void BinaryAdd(int* A, int* B, int length, int* C)
{
	int D;
	int flag = 0;
	for (int i = length - 1; i >= 0; --i)
	{
		D = A[i] + B[i]+ flag;
		flag = (D >> 1)&0x01;
		C[i + 1] = D & 0x01;
	}
	C[0] = flag;
}

//UnitTest
class UnitTestabc
{
public:
	int add(int a, int b)
	{
		return a + b;
	}

};