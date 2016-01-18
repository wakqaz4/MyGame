#include "DXInput.h"

using namespace GD;

CDXInput* CDXInput::This = nullptr;

bool CDXInput::Init(HINSTANCE hInstance)
{
	if (FAILED(DirectInput8Create(hInstance, GD::DINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, nullptr)))
	{
		::MessageBoxA(nullptr, "DirectInput8Create()-Failed", "Error", MB_OK);
		return false;
	}
	This = this;
	return true;
}
void CDXInput::Update(bool focus)
{

}
void CDXInput::ShutDown()
{

}