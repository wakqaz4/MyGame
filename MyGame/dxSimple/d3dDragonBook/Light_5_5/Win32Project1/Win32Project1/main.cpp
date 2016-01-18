/************************************************************************
* This program intends to try different kinds of light source and its
* corresponding effects.
************************************************************************/
#include <windows.h>
#include "tchar.h"
#include "d3d9.h"
#include "d3dx9.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SAFE_RELEASE(p) {if(p != nullptr){p->Release();p = nullptr;}}


//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables  
//////////////////////////////////////////////////////////////////////////
extern LRESULT WINAPI WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern bool ObjectInit(HWND);
extern bool Direct3DUpdate(HWND hwnd);
extern bool Direct3DClearup(HWND hwnd);

struct CustomVertex
{
	float _x;
	float _y;
	float _z;
	float _nx;
	float _ny;
	float _nz;

	CustomVertex(){};

	inline CustomVertex(float x, float y, float z, float nx, float ny, float nz)
	{
		_x = x;
		_y = y;
		_z = z;
		_nx = nx;
		_ny = ny;
		_nz = nz;
	}
	static const DWORD FVF;
};
const DWORD CustomVertex::FVF = D3DFVF_XYZ|D3DFVF_NORMAL;

LPDIRECT3DVERTEXBUFFER9 gPVertexBuffer = nullptr;
LPDIRECT3DINDEXBUFFER9 gPIndexBuffer = nullptr;
LPDIRECT3DDEVICE9 gPd3dDevice = nullptr;
LPD3DXFONT gPfont = nullptr;
LPD3DXMESH gPTeapot = nullptr;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = (HICON)LoadImage(nullptr, _T("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = _T("ClassName");
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return -1;
	}
	
	HWND hwnd = CreateWindow(_T("ClassName"), _T("WindowName"), WS_OVERLAPPEDWINDOW,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
	{
		return -1;
	}
	if (!ObjectInit(hwnd))
	{
		return -1;
	}

	MSG msg{0};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Direct3DUpdate(hwnd);
		}
	}
	return 0;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		PlaySound(_T("Megan Nicole - B-e-a-utiful.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);
		break;
	case WM_DESTROY:
		Direct3DClearup(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Init vertices, indices, material and light here  
//////////////////////////////////////////////////////////////////////////
bool Direct3DInit(HWND hwnd)
{
	LPDIRECT3D9 pD3D9;
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D9 == nullptr)
	{
		return false;
	}

	D3DCAPS9 caps;
	if (FAILED(pD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return false;
	}
	int vp;
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
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


	if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &gPd3dDevice)))
	{
		return false;
	}
	SAFE_RELEASE(pD3D9);

	return true;
}

bool ObjectInit(HWND hwnd)
{
	D3DXCreateFont(gPd3dDevice, 40, 0, 0, 0, 0, 0, 0, 0, 0, _T("¿¬Ìå"), &gPfont);
	D3DXCreateTeapot(gPd3dDevice, &gPTeapot, nullptr);

	CustomVertex v[12];
	// front face
	v[0] = CustomVertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	v[1] = CustomVertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f);
	v[2] = CustomVertex(1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);

	// left face
	v[3] = CustomVertex(-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f);
	v[4] = CustomVertex(0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f);
	v[5] = CustomVertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);

	// right face
	v[6] = CustomVertex(1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
	v[7] = CustomVertex(0.0f, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f);
	v[8] = CustomVertex(1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f);

	// back face
	v[9] = CustomVertex(1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	v[10] = CustomVertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f);
	v[11] = CustomVertex(-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);


	if (FAILED(gPd3dDevice->CreateVertexBuffer(sizeof(v), 0, CustomVertex::FVF, D3DPOOL_DEFAULT, &gPVertexBuffer, nullptr)))
	{
		return false;
	}
	void* gPData;
	gPVertexBuffer->Lock(0, sizeof(v), (void**)&gPData, 0);
	memcpy(gPData, v, sizeof(v));
	gPVertexBuffer->Unlock();	

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient = D3DXCOLOR(D3DCOLOR_XRGB(255, 255, 255));
	mtrl.Diffuse = D3DXCOLOR(D3DCOLOR_XRGB(255, 255, 255));
	mtrl.Specular = D3DXCOLOR(D3DCOLOR_XRGB(255, 255, 255));
	mtrl.Emissive = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
	mtrl.Power = 5.0f;
	gPd3dDevice->SetMaterial(&mtrl);

	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = D3DXCOLOR(D3DCOLOR_XRGB(255, 255, 255))*0.1f;
	light.Specular = D3DXCOLOR(D3DCOLOR_XRGB(255, 255, 255))*0.1f;
	light.Ambient = D3DXCOLOR(D3DCOLOR_XRGB(255, 255, 255))*1.1f;
	light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	gPd3dDevice->SetLight(0, &light);
	gPd3dDevice->LightEnable(0, true);

	gPd3dDevice->SetRenderState(D3DRS_LIGHTING, true);

	gPd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	gPd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);


	return true;
}

void MatrixSet()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	gPd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 1.0f, -3.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	gPd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 3, WINDOW_WIDTH / WINDOW_HEIGHT, 1.0f, 1000.0f);
	gPd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 viewPort;
	viewPort.X = 0.0f;
	viewPort.Y = 0.0f;
	viewPort.Width = WINDOW_WIDTH;
	viewPort.Height = WINDOW_HEIGHT;
	viewPort.MinZ = 0.0f;
	viewPort.MaxZ = 1.0f;
	gPd3dDevice->SetViewport(&viewPort);

}

bool Direct3DUpdate(HWND hwnd)
{
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	gPd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	gPd3dDevice->BeginScene();

	MatrixSet();
	D3DXMATRIX yRot;

	static float y = 0.0f;
	y += 0.0001f;
	D3DXMatrixRotationY(&yRot, y);
	gPd3dDevice->SetTransform(D3DTS_WORLD, &yRot);

	gPfont->DrawTextW(nullptr, _T("Tayler is swift"), -1, &formatRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(11, 222, 1));


	//gPTeapot->DrawSubset(0);
	gPd3dDevice->SetStreamSource(0, gPVertexBuffer, 0, sizeof(CustomVertex));
	gPd3dDevice->SetFVF(CustomVertex::FVF);
	gPd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

	gPd3dDevice->EndScene();
	gPd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

	return true;
}

bool Direct3DClearup(HWND hwnd)
{
	SAFE_RELEASE(gPVertexBuffer);
	SAFE_RELEASE(gPIndexBuffer);
	return true;
}