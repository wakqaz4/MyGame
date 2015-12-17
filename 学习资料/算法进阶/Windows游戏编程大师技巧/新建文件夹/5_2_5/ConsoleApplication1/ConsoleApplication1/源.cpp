#include <stdio.h>
#include <malloc.h>
#include <iostream>
#include <objbase.h>
using namespace std;

// {1F26ECA1-2999-4C01-AEAA-2A6674C32E57}
const IID IID_IX = 
{ 0x1f26eca1, 0x2999, 0x4c01, { 0xae, 0xaa, 0x2a, 0x66, 0x74, 0xc3, 0x2e, 0x57 } };

// {3921E7B1-CCC9-478B-8C12-55C544FA4F89}
const IID IID_IY = 
{ 0x3921e7b1, 0xccc9, 0x478b, { 0x8c, 0x12, 0x55, 0xc5, 0x44, 0xfa, 0x4f, 0x89 } };

// {E573DA7B-D9A1-4310-BC5F-080320944EAE}
const IID IID_IZ = 
{ 0xe573da7b, 0xd9a1, 0x4310, { 0xbc, 0x5f, 0x8, 0x3, 0x20, 0x94, 0x4e, 0xae } };

interface IX : IUnknown
{
	virtual void _stdcall fx(void) = 0;
	
};
interface IY : IUnknown
{
	virtual void _stdcall fy(void) = 0;
};


class CCOM_OBJECT :public IX, public IY
{
public:
	CCOM_OBJECT() :ref_count(0){};
	~CCOM_OBJECT(){};

private:
	virtual HRESULT _stdcall QueryInterface(const IID &iid, void **iface);
	virtual ULONG _stdcall AddRef();
	virtual ULONG _stdcall Release();
	virtual void _stdcall fx(void)
	{
		cout << "Function fx has been canceled" << endl;
	}
	virtual void _stdcall fy(void)
	{
		cout << "Function fy has been canceled." << endl;
	}
	int ref_count;
};

HRESULT _stdcall CCOM_OBJECT::QueryInterface(const IID& iid, void** iface)
{
	if (iid == IID_IUnknown)
	{
		cout << "Requesting IUnknown interface" << endl;
		*iface = (IX*)this;
	}
	if (iid == IID_IX)
	{
		cout << "Requesting IX interface" << endl;
		*iface = (IX*)this;
	}
	else 
	{
		if (iid == IID_IY)
		{
			cout << "Requesting IY interface" << endl;
			*iface = (IY*)this;
		}
		else
		{
			cout << "Requesting unknown interface" << endl;
			*iface = NULL;
			return(E_NOINTERFACE);
		}
	}
	((IUnknown*)(*iface))->AddRef();
	return (S_OK);
}

ULONG _stdcall CCOM_OBJECT::AddRef()
{
	cout << "Adding a reference" << endl;
	return (++ref_count);
}

ULONG _stdcall CCOM_OBJECT::Release()
{
	cout << "Deleting a reference" << endl;
	if (-ref_count == 0)
	{
		delete this;
		return (0);
	}
	else
	{
		return (ref_count);
	}
}

IUnknown* CoCreateInstance(void)
{
	IUnknown *comm_obj = (IX*) new(CCOM_OBJECT);
	cout << "Creating Comm Ojbect" << endl;
	comm_obj->AddRef();
	return (comm_obj);
}

void main(void)
{
	IUnknown *punknown = CoCreateInstance();
	IX* pix = NULL;
	IY* piy = NULL;

	punknown->QueryInterface(IID_IX, (void**)&pix);
	pix->fx();
	pix->Release();
	piy->fy();
	piy->Release();
	punknown->Release();
}