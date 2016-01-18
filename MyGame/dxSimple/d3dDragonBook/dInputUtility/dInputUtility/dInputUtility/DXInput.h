/************************************************************************
* This project intends to do a simple wrapper of dinput8, easier for usage
* //  [1/5/2016 Zhaoyu.Wang]
************************************************************************/
#ifndef _DX_INPUT_H_
#define _DX_INPUT_H_

#include "dinput.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace GD
{
	const DWORD DINPUT_VERSION  = 0x0800;
	class CDXInput
	{
	public:
		CDXInput(HWND hwnd){ m_hwnd = hwnd; This = this; }
		virtual ~CDXInput(){ This = nullptr; }
		bool Init(HINSTANCE hInstance);
		void Update(bool focus);
		void ShutDown();

		inline HWND GetHWND(){ return m_hwnd; }
		inline LPDIRECTINPUT8 GetDirectInput()
		{
			return m_pDInput;
		}

	private:
		HWND m_hwnd;
		LPDIRECTINPUT8 m_pDInput;
	public:
		static CDXInput* This;
	};

}
#endif