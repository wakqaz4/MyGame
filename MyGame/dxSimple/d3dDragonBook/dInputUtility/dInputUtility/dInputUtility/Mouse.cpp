#include "Mouse.h"
#include "assert.h"

using namespace GD;

CMouse::CMouse() :
CDXInputDevice("Mouse", GUID_SysMouse)
{
	::ZeroMemory(&m_mouseBuffer, sizeof(m_mouseBuffer));
	/**
	 *	I think the buffered flag should be set to true, when I get more familiar with DirectInput
	 *	*/
	m_buffered = false;

	bool res = CreateDirectInputDevice(&c_dfDIMouse, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, 256);

#ifdef _DEBUG
	assert(res);
#endif
	Accquire();
}

void CMouse::Update(bool focus)
{
	if (!focus)
	{
		return;
	}
	if (m_buffered)
	{
		/**
		*	Yet to be implemented
		*	*/
	}
	else
	{
		HRESULT hr = GetDInputDevice()->GetDeviceState(sizeof(m_mouseBuffer), (void**)&m_mouseBuffer);
		GetDInputDevice()->Acquire();
	}
}

bool CMouse::IsButtonDown(int iButton)
{
	if (m_mouseBuffer.rgbButtons[iButton] & 0x80)
	{
		return true;
	}
	return false;	
}
//mouse moves horizontally
float CMouse::GetDeltaX()
{
	return (float)m_mouseBuffer.lX;
}
//mouse moves vertically
float CMouse::GetDeltaY()
{
	return (float)m_mouseBuffer.lY;
}
//mouse's wheel movement
float CMouse::GetDeltaZ()
{
	return (float)m_mouseBuffer.lZ;
}
