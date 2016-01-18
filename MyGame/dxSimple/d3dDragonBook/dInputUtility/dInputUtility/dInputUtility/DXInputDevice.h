
#ifndef _DXINPUT_DEVICE_H_
#define _DXINPUT_DEVICE_H_

#include "DXInput.h"

namespace GD
{
	class CDXInputDevice
	{
	public:
		CDXInputDevice(const char* deviceName, const GUID& guid);

	protected:
		bool Accquire();
		bool Unaccquire();
		LPDIRECTINPUTDEVICE8 GetDInputDevice();
		virtual bool CreateDirectInputDevice(LPCDIDATAFORMAT dataFormat, DWORD coopLevel, DWORD bufSize);

	private:
		LPDIRECTINPUTDEVICE8 m_pDInputDevice;
		const char* m_deviceName;
		const GUID& m_guid;
		LPCDIDATAFORMAT m_pDataFormat;
		DWORD m_coopLevel;
		bool m_needsPoll;
	};
}

#endif