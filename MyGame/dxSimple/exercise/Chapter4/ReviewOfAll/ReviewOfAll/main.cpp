/************************************************************************
* This project intends to make a summary of the DX APIs that I've learned
* before, which mainly include vertices&indices, mesh material texture,
* and D3DXMATRIX, camera, DXInput.
* And in order to get familiar with all the d3d9 and dinput8, I wrote all
* the code with original APIs, which will be wrapped in classes in the next
* project(ReorganizeOfAll)
*	I intend to finish this program with the following 7 steps:
*	1. Finish with basic functions, such as d3d9 ,dinput8, playSound, font;
*	2. Draw a sphere and a cube with colored vertices&indices, with fixed camera;
*	3. Draw a cube with UV texture and a teapot with d3dxMesh;
*	4. Load mesh from .X file with texture and material, and form light;
*	5. Form flexible camera with dInput8;
*	6. Wrap dInput8, d3d9 and camera with class;
*	7. Write a new program named ReorganizeOfAll with the upward classes.
* //  [12/16/2015 Zhaoyu.Wang]
************************************************************************/
#include <Windows.h>
#include <tchar.h>
#include <time.h>
#include "d3d9.h"
#include "d3dx9.h"
#include "dinput.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CLASS_NAME _T("ClassName")
#define SAFE_RELEASE(p) {if (p){(p)->Release(); p = nullptr;}}

//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables 
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern bool DirectInputInit(HWND hwnd, HINSTANCE hInstance);
extern bool ObjectInit();
extern void MatrixSet();
extern void CameraSet();
extern void Direct3DUpdate(HWND hwnd);
extern void Direct3DRender(HWND hwnd);
extern bool Direct3DCleanup(HWND hwnd);

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
LPD3DXFONT gPFont = nullptr;
LPDIRECTINPUTDEVICE8 gPMouse = nullptr;
LPDIRECTINPUTDEVICE8 gPKeyboard = nullptr;
DIMOUSESTATE gDIMouseState;
char gKeyboardBuffer[256]{0};
float gFormatRectChange = 0.0f;

struct ScolorVertex
{
	float x;
	float y;
	float z;
	DWORD color;
};

#define D3DFVF_COLOR_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
//////////////////////////////////////////////////////////////////////////
// The function entities.  
//////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = (HCURSOR)LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = (HICON)LoadImage(nullptr, _T("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return -1;
	}
	HWND hwnd = CreateWindow(CLASS_NAME, _T("A summarize."), WS_OVERLAPPEDWINDOW,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
	{
		return -1;
	}
	if (!DirectInputInit(hwnd, hInstance))
	{
		return -1;
	}
	if (!ObjectInit())
	{
		return -1;
	}

	MSG msg{ 0 };
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
			Direct3DRender(hwnd);
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
		Direct3DRender(hwnd);
		ValidateRect(hwnd, nullptr);
		break;
	case WM_DESTROY:
		Direct3DCleanup(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

bool Direct3DInit(HWND hwnd)
{
	LPDIRECT3D9 pD3D9;
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D9 == nullptr)
	{
		MessageBox(nullptr, _T("Creating D3D9 error!"), _T("D3D init error!"), MB_OK);
		return false;
	}
	D3DCAPS9 caps;
	if (FAILED(pD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return false;
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
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 2;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &gPD3DDevice)))
	{
		return false;
	}
	SAFE_RELEASE(pD3D9);

	return true;
}
bool DirectInputInit(HWND hwnd, HINSTANCE hInstance)
{
	LPDIRECTINPUT8 pDInput8;
	if (FAILED(DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&pDInput8, nullptr)))
	{
		return false;
	}
	if (FAILED(pDInput8->CreateDevice(GUID_SysMouse, &gPMouse, nullptr)))
	{
		return false;
	}
	gPMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	gPMouse->SetDataFormat(&c_dfDIMouse);
	gPMouse->Acquire();
	if (FAILED(pDInput8->CreateDevice(GUID_SysKeyboard, &gPKeyboard, nullptr)))
	{
		return false;
	}
	gPKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	gPKeyboard->SetDataFormat(&c_dfDIKeyboard);
	gPKeyboard->Acquire();
	return true;
}

bool ObjectInit()
{
	PlaySound(_T("孙燕姿 - 天使的指纹.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	D3DXCreateFont(gPD3DDevice, 38, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont);


	return true;
}
void MatrixSet()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	gPD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 0.0f, -300.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	gPD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 6.0f, WINDOW_WIDTH / WINDOW_HEIGHT, 1.0f, 1000.0f);
	gPD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 viewPort;
	viewPort.Width = WINDOW_WIDTH;
	viewPort.Height = WINDOW_HEIGHT;
	viewPort.X = 0;
	viewPort.Y = 0;
	viewPort.MinZ = 0.0f;
	viewPort.MaxZ = 1.0f;
	gPD3DDevice->SetViewport(&viewPort);
}
void CameraSet()
{

}
void DirectInputUpdate()
{
	gPMouse->GetDeviceState(sizeof(gDIMouseState), (void**)&gDIMouseState);
	gPMouse->Acquire();
	gPKeyboard->GetDeviceState(sizeof(gKeyboardBuffer), (void**)&gKeyboardBuffer);
	gPKeyboard->Acquire();

	if (gDIMouseState.rgbButtons[0] & 0x80)
	{
		gFormatRectChange += 0.001f;
	}
	if (gDIMouseState.rgbButtons[1] & 0x80)
	{
		gFormatRectChange -= 0.001f;
	}
	gFormatRectChange += gDIMouseState.lX;
	gFormatRectChange += gDIMouseState.lY;

	if (gKeyboardBuffer[DIK_W] & 0x80)
	{
		gFormatRectChange += 0.001f;
	}
	if (gKeyboardBuffer[DIK_S] & 0x80)
	{
		gFormatRectChange -= 0.001f;
	}
}

void Direct3DUpdate(HWND hwnd)
{
	DirectInputUpdate();
}
void Direct3DRender(HWND hwnd)
{
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(24, 212, 33), 1.0f, 0);
	gPD3DDevice->BeginScene();
	MatrixSet();

	formatRect.top -= gFormatRectChange;
	gPFont->DrawTextW(nullptr, _T("Tayler is swift"), -1, &formatRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(46, 2, 111));


	gPD3DDevice->EndScene();
	gPD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

}
bool Direct3DCleanup(HWND hwnd)
{
	SAFE_RELEASE(gPFont);
	SAFE_RELEASE(gPMouse);
	SAFE_RELEASE(gPKeyboard);
	SAFE_RELEASE(gPD3DDevice);
	return true;
}


