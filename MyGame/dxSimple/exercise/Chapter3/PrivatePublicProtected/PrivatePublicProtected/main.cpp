#include <iostream>

class Base
{
private :
	int m_varPrivate;
protected:
	int m_varProtected;
public:
	int m_varPublic;

};
class Derived: public Base
{
public:
	inline int GetProtected()
	{
		return m_varProtected;
	}
	inline int GetPrivate()
	{
		return m_varPrivate;
	}
	
};
int main()