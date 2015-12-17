#include "MyCommon.h"
#include "CDirectInputClass.h"

CDirectInputClass::CDirectInputClass()
{
	m_pDirectInput = nullptr;
	m_pMouseInput = nullptr;
	m_pKeyboardInput = nullptr;
	ZeroMemory(&m_mouseState, sizeof(m_mouseState));
	ZeroMemory(&m_keyboardBuffer, sizeof(m_keyboardBuffer));
}

CDirectInputClass::~CDirectInputClass()
{
	if (m_pMouseInput != nullptr)
	{
		m_pMouseInput->Unacquire();
	}
	if (m_pKeyboardInput != nullptr)
	{
		m_pKeyboardInput->Unacquire();
	}
	SAFE_RELEASE(m_pMouseInput);
	SAFE_RELEASE(m_pKeyboardInput);
	SAFE_RELEASE(m_pDirectInput);
}

bool CDirectInputClass::Init(HWND hwnd, HINSTANCE hInstance, DWORD keyboardCoopFlags, DWORD mouseCoopFlags)
{
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&m_pDirectInput, nullptr);
	if (FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseInput, nullptr)))
	{
		return false;
	}
	m_pMouseInput->SetCooperativeLevel(hwnd, keyboardCoopFlags);
	m_pMouseInput->SetDataFormat(&c_dfDIMouse);
	m_pMouseInput->Acquire();

	if (FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardInput, nullptr)))
	{
		return false;
	}
	m_pKeyboardInput->SetCooperativeLevel(hwnd, mouseCoopFlags);
	m_pKeyboardInput->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboardInput->Acquire();

	return true;
}

void CDirectInputClass::Update()
{
	m_pMouseInput->GetDeviceState(sizeof(m_mouseState), (void**)&m_mouseState);
	m_pMouseInput->Acquire();

	m_pKeyboardInput->GetDeviceState(sizeof(m_keyboardBuffer), (void**)&m_keyboardBuffer);
	m_pKeyboardInput->Acquire();
}

bool CDirectInputClass::IsKeyDown(int iKey)
{
	if (m_keyboardBuffer[iKey] & 0x80)
	{
		return true;
	}
	return false;
}

bool CDirectInputClass::IsMouseButtonDown(int iMouse)
{
	if (m_mouseState.rgbButtons[iMouse] & 0x80)
	{
		return true;
	}
	return false;
}

float CDirectInputClass::MouseDX()
{
	return (float)m_mouseState.lX;
}

float CDirectInputClass::MouseDY()
{
	return (float)m_mouseState.lY;
}

float CDirectInputClass::MouseDZ()
{
	return (float)m_mouseState.lZ;
}