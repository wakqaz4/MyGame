#include <iostream>

int main()
{
	int currVar;
	if (std::cin >> currVar)
	{
		auto prevVar = currVar;
		int count = 1;
		while (std::cin >> currVar)
		{
			if (currVar == prevVar)
			{
				++count;
			}
			else
			{
				std::cout << prevVar << " repeated " << count << "times" << std::endl;
				count = 0;
				prevVar = currVar;
			}
		}
		std::cout << prevVar << " repeated " << count << "times" << std::endl;
	}
	return 0;
}