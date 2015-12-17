#include <dinput.h>

#pragma once

class CDirectInputClass
{
private:
	LPDIRECTINPUT8 m_pDInput;
	LPDIRECTINPUTDEVICE8 m_pMouseDev;
	LPDIRECTINPUTDEVICE8 m_pKeyboardDev;
	DIMOUSESTATE m_mouseState;
	char m_keyboardBuffer[256];

public:
	bool Init(HWND hwnd, HINSTANCE hInstance, DWORD mouseCoopFlags, DWORD keyboardCoopFlags);
	void Update();
	bool IsMouseDown(int iMouseButton);
	bool IsKeyDown(int iKey);
	float GetMouseDX();
	float GetMouseDY();
	float GetMouseDZ();

public:
	CDirectInputClass();
	~CDirectInputClass();
};