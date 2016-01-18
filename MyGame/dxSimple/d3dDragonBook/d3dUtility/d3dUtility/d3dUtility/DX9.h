/************************************************************************
* Wrapper of DIRECT3D9, single instance, using This pointer to get its 
* instance;
* //  [1/6/2016 Zhaoyu.Wang]
************************************************************************/

#ifndef _DX9_UT_
#define _DX9_UT_

#include "d3d9.h"
#include "d3dx9.h"

namespace GD
{
	class CDX9
	{
	public:
		CDX9();
		~CDX9();
		inline LPDIRECT3D9 GetDirect3D9(){ return m_pD3d9; }

	private:
		LPDIRECT3D9 m_pD3d9;
	public:
		static CDX9* This;
	};
}
#endif