#include <iostream>
#include <stdio.h>
#include <string>

#include "Chapter_2/2_1_insertSort.h"
#include "2_2_AnalyzeAlgorithm.h"
#include "2_3_BinarySearch.h"

using namespace std;

int main(void)
{
	int num[10] = { 2, 64, 23, 123, 45, 32, 45, 6, 3, 9 };

	//
	SelectionSort(num, 10);

	int index = BinarySearch(num, 0, 9, 64);
	cout << index + 1 << endl;

	for (auto numEle : num)
	{
		std::cout << numEle << endl;
	}

	//Binary add


	return 0;
}