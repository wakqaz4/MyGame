#include "DXDevice9.h"
#include "assert.h"

using namespace GD;

CDXDevice9::CDXDevice9(HWND hwnd):
m_hwnd(hwnd)
{
	if (CDX9::This == nullptr)
	{
		new CDX9();
	}
	LPDIRECT3D9 pD3d9 = CDX9::This->GetDirect3D9();

#ifdef _DEBUG
	assert(pD3d9 != nullptr);
#endif

	D3DCAPS9 caps;
	if (FAILED(pD3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		::MessageBoxA(nullptr, "GetDeviceCaps()-Failed", "", MB_OK);
	}
	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = 800;
	d3dpp.BackBufferHeight = 600;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 2;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = m_hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(pD3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &m_pDevice)))
	{
		::MessageBoxA(nullptr, "d3d CreateDevice()-Failed", "", MB_OK);
	}

	delete CDX9::This;
}

CDXDevice9::CDXDevice9(HWND hwnd, bool windowed, int width, int height, int stencil, int color) :
m_hwnd(hwnd)
{
	if (CDX9::This == nullptr)
	{
		new CDX9();
	}
	LPDIRECT3D9 pD3d9 = CDX9::This->GetDirect3D9();

#ifdef _DEBUG
	assert(pD3d9 != nullptr);
#endif

	D3DCAPS9 caps;
	if (FAILED(pD3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		::MessageBoxA(nullptr, "GetDeviceCaps()-Failed", "", MB_OK);
	}

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferFormat = ((color == 32 )? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8);
	d3dpp.BackBufferCount = 2;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = m_hwnd;
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = stencil ? D3DFMT_D24S8 : D3DFMT_D24X8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(pD3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &m_pDevice)))
	{
		::MessageBoxA(nullptr, "d3d CreateDevice()-Failed", "", MB_OK);
	}

	delete CDX9::This;
}

CDXDevice9::~CDXDevice9()
{
	if (m_pDevice)
	{
		m_pDevice->Release();
	}	
}

void CDXDevice9::RenderDirectly(pFuncRender pRenderFunc)
{
	m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDevice->BeginScene();

	pRenderFunc();

	m_pDevice->EndScene();
	m_pDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

void CDXDevice9::RenderDirectly(std::function<void(void)> pRenderFunc)
{
	m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDevice->BeginScene();

	pRenderFunc();

	m_pDevice->EndScene();
	m_pDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

//template<class T>
//void CDXDevice9::RenderDirectly(T* renderClass, pClassFuncRender renderFunc)
//{
//	m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
//	m_pDevice->BeginScene();
//
//	renderClass->*renderFunc();
//
//	m_pDevice->EndScene();
//	m_pDevice->Present(nullptr, nullptr, nullptr, nullptr);
//}

template<class T>
void CDXDevice9::RenderDirectly(std::function<void(T* renderClass)> pClassRenderFunc)
{
	m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDevice->BeginScene();

	pClassRenderFunc(renderClass);

	m_pDevice->EndScene();
	m_pDevice->Present(nullptr, nullptr, nullptr, nullptr);
}