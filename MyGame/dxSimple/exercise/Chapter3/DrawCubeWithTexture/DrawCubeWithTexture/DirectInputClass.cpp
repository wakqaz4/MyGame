#include "MyCommon.h"
#include "DirectInputClass.h"

CDirectInputClass::CDirectInputClass()
{
	m_pDInput = nullptr;
	m_pMouseDev = nullptr;
	m_pKeyboardDev = nullptr;
	ZeroMemory(&m_mouseState, sizeof(m_mouseState));
	ZeroMemory(m_keyboardBuffer, sizeof(m_keyboardBuffer));
}

CDirectInputClass::~CDirectInputClass()
{
	SAFE_RELEASE(m_pMouseDev);
	SAFE_RELEASE(m_pKeyboardDev);
	SAFE_RELEASE(m_pDInput);
}

bool CDirectInputClass::Init(HWND hwnd, HINSTANCE hInstance, DWORD mouseCoopFlags, DWORD keyboardCoopFlags)
{
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&m_pDInput, nullptr);
	m_pDInput->CreateDevice(GUID_SysMouse, &m_pMouseDev, nullptr);
	m_pMouseDev->SetCooperativeLevel(hwnd, mouseCoopFlags);
	m_pMouseDev->SetDataFormat(&c_dfDIMouse);
	m_pMouseDev->Acquire();

	m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDev, nullptr);
	m_pKeyboardDev->SetCooperativeLevel(hwnd, keyboardCoopFlags);
	m_pKeyboardDev->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboardDev->Acquire();

	return true;
}

void CDirectInputClass::Update()
{
	if (m_pMouseDev->GetDeviceState(sizeof(m_mouseState), (void**)&m_mouseState))
	{
		m_pMouseDev->Acquire();
	}
	if (m_pKeyboardDev->GetDeviceState(sizeof(m_keyboardBuffer), (void*)m_keyboardBuffer))
	{
		m_pKeyboardDev->Acquire();
	}
}

bool CDirectInputClass::IsMouseDown(int iMouseButton)
{
	if (m_mouseState.rgbButtons[iMouseButton] & 0x80)
	{
		return true;
	}
	return false;
}

float CDirectInputClass::GetMouseDX()
{
	return (float)m_mouseState.lX;
}

float CDirectInputClass::GetMouseDY()
{
	return (float)m_mouseState.lY;
}

float CDirectInputClass::GetMouseDZ()
{
	return (float)m_mouseState.lZ;
}

bool CDirectInputClass::IsKeyDown(int iKey)
{
	if (m_keyboardBuffer[iKey] & 0x80)
	{
		return true;
	}
	return false;
}

