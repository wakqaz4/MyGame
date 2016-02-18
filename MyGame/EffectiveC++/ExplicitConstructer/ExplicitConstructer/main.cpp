/************************************************************************
* Better declare constructor with explicit in order to avoid implicit cast.
* // [2016/2/15 12:09 Zhaoyu.Wang]  
************************************************************************/

class B
{
public:
	B(int x = 0, bool y = true){};
};

void DoSomething(B objectB)
{

}

int main()
{
	B b;
	DoSomething(b);

	B b1(28);
	DoSomething(b1);

	/**
	 *	F9 here, and you'll see...
	 */
	DoSomething(28);

	DoSomething(B(28));
}