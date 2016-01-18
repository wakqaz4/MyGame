#ifndef _GD_MOUSE_H_
#define _GD_MOUSE_H_
#pragma once

#include "DXInputDevice.h"

namespace GD
{
	class CMouse:public CDXInputDevice
	{
	public:
		CMouse();
		void Update(bool focus);
		bool IsButtonDown(int iButton);
		float GetDeltaX();
		float GetDeltaY();
		float GetDeltaZ();

	private:
		bool m_buffered;
	public:
		DIMOUSESTATE m_mouseBuffer;
	};
}

#endif