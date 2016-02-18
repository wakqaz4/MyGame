#include "KeyBoard.h"
#include "assert.h"

using namespace GD;

CKeyboard::CKeyboard()
: CDXInputDevice("Keyboard", GUID_SysKeyboard)
{
	::ZeroMemory(m_keyboardBuffer, sizeof(m_keyboardBuffer));
	/**
	*	I think the buffered flag should be set to true, when I get more familir with DirectInput
	*	*/
	m_buffered = false;

	bool res = CreateDirectInputDevice(&c_dfDIKeyboard, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, 256);

#ifdef _DEBUG
	assert(res);
#endif
	Accquire();
}

void CKeyboard::Update(bool focus)
{
	if (!focus)
	{
		return;
	}
	DIDEVICEOBJECTDATA rgdod[256];
	DWORD dwItems = 256;

	while (GetDInputDevice())
	{
		dwItems = 256;
		if (m_buffered)
		{
			/**
			*	Yet to be implemented
			*	*/
			HRESULT hr = GetDInputDevice()->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
			if (SUCCEEDED(hr))
			{
				for (int i = 0; i < dwItems; i++)
				{
					int key = rgdod[i].dwOfs;
					bool pressed = ((rgdod[i].dwData & 0x80) != 0);
					/**
					 *	Should've processed event here
					 *	*/
					//ProcessEvent(key, pressed);
				}
				break;
			}
			else
			{
				if (hr = DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
				{
					if (FAILED(hr = GetDInputDevice()->Acquire()))
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			//::ZeroMemory(m_keyboardBuffer, sizeof(m_keyboardBuffer));
			HRESULT hr = GetDInputDevice()->GetDeviceState(sizeof(m_keyboardBuffer), (void*)m_keyboardBuffer);
			GetDInputDevice()->Acquire();
			break;
		}
	}
	
}

bool CKeyboard::IsKeyDown(int iKey)
{
	if (m_keyboardBuffer[iKey] & 0x80)
	{
		return true;
	}
	return false;
}

bool CKeyboard::IsKeyUp(int iKey)
{
	if (m_keyboardBuffer[iKey] & 0x80)
	{
		return false;
	}
	return true;
}