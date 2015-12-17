/************************************************************************/
/* this is an copy version of Sales Item which is 
 * realized by myself
 */
/************************************************************************/
#include <iostream>
#include <string>
#pragma once

class MySalesItem
{
public:
	friend std::istream& operator>>(std::istream&, MySalesItem&);
	friend std::ostream& operator<<(std::ostream&, MySalesItem&);

	MySalesItem() :		
		units_sold(0),
		revenue(0.0f)
	{}
	~MySalesItem(){};

public:
	inline const std::string getBookNo()
	{
		return this->bookNo;
	}
	inline void operator +=(const MySalesItem& item2)
	{
		if (this->bookNo == item2.bookNo)
		{
			this->revenue += item2.revenue;
			this->units_sold += item2.units_sold;			
		}
		else
		{
		}
	}

	inline MySalesItem operator+(MySalesItem& item2)
	{
		MySalesItem ret;
		if (this->bookNo != item2.bookNo)
		{
			return ret;
		}
		ret.units_sold = this->units_sold + item2.units_sold;
		ret.revenue = this->revenue + item2.revenue;
		ret.bookNo = this->bookNo;
		return ret;
	}

	inline bool operator ==(const MySalesItem& item2)
	{
		return (this->bookNo == item2.bookNo)
			&& (this->revenue == item2.revenue)
			&& (this->units_sold == item2.units_sold);
	}
	
	inline bool operator !=(const MySalesItem& item2)
	{
		return !(*this == item2);
	}

private:
	std::string bookNo;      // implicitly initialized to the empty string
#ifdef IN_CLASS_INITS
	unsigned units_sold = 0; // explicitly initialized
	double revenue = 0.0;
#else
	unsigned units_sold;
	double revenue;
#endif
};

std::istream& operator>>(std::istream& in, MySalesItem& item)
{
	in >> item.bookNo>>item.revenue>>item.units_sold;
	return in;
}
std::ostream& operator<<(std::ostream& out, MySalesItem& item)
{
	out << item.bookNo <<" "<< item.revenue <<" "<<item.units_sold<<std::endl;
	return out;
}

