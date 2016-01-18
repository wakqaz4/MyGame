#include "DXInputDevice.h"

using namespace GD;

CDXInputDevice::CDXInputDevice(const char* deviceName, const GUID& guid):
m_guid(guid)
{
	m_deviceName = deviceName;
}
bool CDXInputDevice::Accquire()
{
	if (m_pDInputDevice == nullptr)
	{
		return false;
	}
	HRESULT hr = m_pDInputDevice->Acquire();
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool CDXInputDevice::Unaccquire()
{
	return (m_pDInputDevice&&m_pDInputDevice->Unacquire());
}

LPDIRECTINPUTDEVICE8 CDXInputDevice::GetDInputDevice()
{
	return m_pDInputDevice;
}

bool CDXInputDevice::CreateDirectInputDevice(LPCDIDATAFORMAT dataFormat, DWORD coopLevel, DWORD bufSize)
{
	if (CDXInput::This == nullptr)
	{
		::MessageBoxA(nullptr, "CreateDirectInputDevice()-Failed\nCheck DirectInput8", "Error", MB_OK);
		return false;
	}
	
	HRESULT hr = CDXInput::This->GetDirectInput()->CreateDevice(m_guid, &m_pDInputDevice, nullptr);
	if (FAILED(hr))
	{
		::MessageBoxA(nullptr, "CreateDevice()-Failed", "Error", MB_OK);
		return false;
	}

	DIDEVCAPS caps;
	caps.dwSize = sizeof(DIDEVCAPS);
	m_pDInputDevice->GetCapabilities(&caps);
	if (caps.dwFlags & DIDC_POLLEDDEVICE)
	{
		m_needsPoll = true;
	}
	
	if (dataFormat == nullptr)
	{
		::MessageBoxA(nullptr, "Incorrect DInput dataFormat!", "Warning", MB_OK);
	}
	hr = m_pDInputDevice->SetDataFormat(dataFormat);
	if (FAILED(hr))
	{
		return false;
	}
	m_pDataFormat = dataFormat;	

	hr = m_pDInputDevice->SetCooperativeLevel(CDXInput::This->GetHWND(), coopLevel);
	if (FAILED(hr))
	{
		return false;
	}
	m_coopLevel = coopLevel;

	/**
	 *	 Create a buffer to store past operations
	 *	 */
	//DIPROPDWORD diBuffer;
	//diBuffer.dwData = bufSize;
	//diBuffer.diph.dwSize = sizeof(DIPROPDWORD);
	//diBuffer.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	//diBuffer.diph.dwObj = 0;
	//diBuffer.diph.dwHow = DIPH_DEVICE;
	//hr = m_pDInputDevice->SetProperty(m_guid, &diBuffer.diph);
	//if (FAILED(hr))
	//{
	//	return false;
	//}

	return true;
}
