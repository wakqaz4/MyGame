#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#pragma once

#include "DXInputDevice.h"

namespace GD
{
	typedef char charBufferUnit;

	class CKeyBoard:public CDXInputDevice
	{
	public:
		void Update(bool focus);
		bool IsKeyDown(int iKey);
		CKeyBoard();
	private:
		bool m_buffered;
	public:
		charBufferUnit m_keyboardBuffer[256];
	};
}

#endif
