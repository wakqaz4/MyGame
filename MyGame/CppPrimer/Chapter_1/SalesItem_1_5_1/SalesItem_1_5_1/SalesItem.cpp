#include "MySalesItem.h"

int main()
{
	MySalesItem total;
	MySalesItem trans;
	if (std::cin >> total)
	{
		while (std::cin >> trans)
		{
			if (trans.getBookNo() != total.getBookNo())
			{
				std::cout << total;
				total = trans;
			}
			else
			{
				total += trans;
			}
		}
		std::cout << total;
	}
	return 0;
}