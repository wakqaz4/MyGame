/************************************************************************
* Wrapper of DX device9.
* //  [1/6/2016 Zhaoyu.Wang]
************************************************************************/
#ifndef _DX_DEVICE9_H_
#define _DX_DEVICE9_H_
#pragma once

#include "DX9.h"
#include <functional>

namespace GD
{
	class CDXDevice9
	{
		typedef void(CALLBACK* pFuncRender)(void);
		/**
		 *	Seems can not implement this typedef...But why?
		 *	*/
		//template<class T>
		//typedef void(T::*pClassFuncRender)(void);

	public:
		CDXDevice9(HWND hwnd);
		/**
		 *	Init with suggested parameters
		 *	*/
		CDXDevice9(HWND hwnd, bool windowed, int width, int height, int stencil, int color);
		~CDXDevice9();

		inline LPDIRECT3DDEVICE9 GetD3DDevice(){ return m_pDevice; }
		void RenderDirectly(pFuncRender renderFunc);
		void RenderDirectly(std::function<void(void)> pRenderFunc);
		//template<class T>
		//void RenderDirectly(T* renderClass, pClassFuncRender);
		template<typename T>
		void RenderDirectly(std::function<void(T*)> pClassRenderFunc);

	private:
		LPDIRECT3DDEVICE9 m_pDevice;
		HWND m_hwnd;
		
	};
	/**
	 *	A question:
	 *	Should this class be single instanced?
	 *	*/
}

#endif