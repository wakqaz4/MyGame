#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Goal_1000/Chapter_2/2_1_insertSort.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Goal_1000_UnitTest
{		
	TEST_CLASS(UnitTest)
	{
	public:		
		TEST_METHOD(abc)
		{
			// TODO:  在此输入测试代码
			UnitTestabc tt;
			int actual = tt.add(1, 23);
			Assert assert;
			assert.AreEqual(tt.add(1, 23), 24);
		}
	};
}