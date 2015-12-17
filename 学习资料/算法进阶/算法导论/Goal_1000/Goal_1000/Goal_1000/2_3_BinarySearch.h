/*
BINARY_SEARCH
INPUT: A, v, l, r
1.    l = 1, r = A.length, index = (l+r)/2
2.    if (A[index] == v)
3.        return index
4.    if (l == index)
5.        if (A[r] == v)
6.            return r
7.        else
8.            return NIL
9.    if (A[index] < v)
10.        l = index + 1
11.        return BINARY_SEARCH(A, v, l, r)
12.    if (A[index] > v)
13.        r = index - 1
14.        return BINARY_SEARCH(A, v, l, r)
*/

int BinarySearch(int* A, int l, int r, int v)
{
	int index = (l + r) / 2;
	if (A[index] == v)
	{
		return index;
	}
	if (l == index)
	{
		if (A[r] == v)
		{
			return r;
		}
		else
		{
			return -1;
		}
	}

	if (A[index] < v)
	{
		l = index + 1;
		return BinarySearch(A, l, r, v);
	}
	else
	{
		r = index - 1;
		return BinarySearch(A, l, r, v);
	}
}