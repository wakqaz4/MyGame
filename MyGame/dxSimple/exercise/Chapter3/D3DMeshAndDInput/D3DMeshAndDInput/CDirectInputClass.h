/************************************************************************
* A class that encapsulates direct input and its devices.                                                                  
************************************************************************/
#include <dinput.h>

#pragma once

class CDirectInputClass
{
private:
	LPDIRECTINPUT8 m_pDirectInput;
	LPDIRECTINPUTDEVICE8 m_pMouseInput;
	LPDIRECTINPUTDEVICE8 m_pKeyboardInput;
	char m_keyboardBuffer[256];
	DIMOUSESTATE m_mouseState;

public:
	CDirectInputClass();
	~CDirectInputClass();
	bool Init(HWND hwnd, HINSTANCE hInstance, DWORD keyboardCoopFlags, DWORD mouseCoopFlags);
	void Update();
	bool IsKeyDown(int iKey);

	bool IsMouseButtonDown(int iButton);
	float MouseDX();
	float MouseDY();
	float MouseDZ();	
};