#include <iostream>
using namespace std;


/*
	Algorithm: SELECTION_SORT
	INPUT:    n, A
	OUTPUT:    A
	1.    for i = 1 to A.length-1
	2.        currElement = A[i]
	2.        for j = i to A.length
	3.            if (A[j]<currElement)
	4.                currElement = A[j]
	5.        A[i] = currElement;
*/
void SelectionSort(int* A, int count)
{
	if (count <= 1)
	{
		return;
	}
	for (int i = 0; i < count - 1; i++)
	{
		int currElement = A[i];
		int index = i;
		for (int j = i+1; j <= count - 1; j++)
		{
			if (A[j] < currElement)
			{
				currElement = A[j];
				index = j;
			}
		}
		A[index] = A[i];
		A[i] = currElement;
	}
}