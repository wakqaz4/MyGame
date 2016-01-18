#include "DX9.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace GD;

CDX9* CDX9::This = nullptr;

CDX9::CDX9():
m_pD3d9(nullptr)
{
	if (This != nullptr)
	{
		return;
	}
	m_pD3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pD3d9)
	{
		::MessageBoxA(nullptr, "Direct3DCreate9()-Failed", "", MB_OK);

		/**
		 *	Maybe I should use some other operations here to safely exit?
		 *	*/
		::exit(-1);
	}
	This = this;
}

CDX9::~CDX9()
{
	m_pD3d9->Release();
}
