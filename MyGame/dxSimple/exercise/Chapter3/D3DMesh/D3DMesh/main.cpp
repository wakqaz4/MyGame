/************************************************************************
* this project is used to draw four meshes                                                                   
* Critical Function: LPD3DXMESH
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SAFE_RELEASE(p) {if (p){p->Release(); p = nullptr; }}

//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables  
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern void Direct3DRender(HWND hwnd);
extern bool Direct3DClearup(HWND hwnd);
extern bool ObjectInit(HWND hwnd);
extern void MatrixSet();

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
ID3DXFont* gPFont = nullptr;
LPD3DXMESH gPTeapot = nullptr;
LPD3DXMESH gPCube = nullptr;
LPD3DXMESH gPTorus = nullptr;
LPD3DXMESH gPSphere = nullptr;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = (HICON)LoadImage(nullptr, _T("Icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = _T("ClassName");
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("ClassName"), _T("The fantastic meshes!"), WS_OVERLAPPEDWINDOW,
		0, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
	{
		MessageBox(hwnd, _T("Direct3D Init Error!"), _T("Error!"), MB_OK);
		return -1;
	}

	MSG msg{ 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
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
		Direct3DClearup(hwnd);
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
	d3dpp.BackBufferCount = 1;
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

	if (!ObjectInit(hwnd))
	{
		return false;
	}

	return true;
}

void Direct3DRender(HWND hwnd)
{
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(23, 53, 189), 1.0f, 0);

	MatrixSet();
	gPD3DDevice->BeginScene();

	WCHAR str[50];
	int setLength = swprintf_s(str, _T("%0.3f"), 300.0f);
	gPFont->DrawTextW(nullptr, str, setLength, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 10, 45));

	D3DXMATRIX Ry;
	D3DXMatrixRotationY(&Ry, timeGetTime() / 1000.0f);
	D3DXMATRIX teaPotWorldTrans;
	D3DXMatrixTranslation(&teaPotWorldTrans, 5.0f, 5.0f, 5.0f);
	teaPotWorldTrans *= Ry;
	gPD3DDevice->SetTransform(D3DTS_WORLD, &teaPotWorldTrans);
	gPTeapot->DrawSubset(0);

	gPD3DDevice->EndScene();
	gPD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

bool Direct3DClearup(HWND hwnd)
{
	SAFE_RELEASE(gPFont);
	SAFE_RELEASE(gPTeapot);
	SAFE_RELEASE(gPCube);
	SAFE_RELEASE(gPTorus);
	SAFE_RELEASE(gPSphere);
	SAFE_RELEASE(gPD3DDevice);
	return true;
}

bool ObjectInit(HWND hwnd)
{
	D3DXCreateFont(gPD3DDevice, 36, 0, 0, 0, 0, 0, 0, 0, 0, _T("¿¬Ìå"), &gPFont);
	srand(unsigned(time(nullptr)));
	PlaySound(_T("¾ý¤¬¤¤¤ë¤«¤é (Long Version).wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);

	D3DXCreateTeapot(gPD3DDevice, &gPTeapot, nullptr);
	D3DXCreateBox(gPD3DDevice, 2, 2, 2, &gPCube, nullptr);
	D3DXCreateTorus(gPD3DDevice, 1, 2, 25, 25, &gPTorus, nullptr);
	D3DXCreateSphere(gPD3DDevice, 2, 25, 25, &gPSphere, nullptr);

	return true;
}

void MatrixSet()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	gPD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 0.0f, -200.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	gPD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 6.0f, 1.0f, 1.0f, 1000.0f);
	gPD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 viewPort;
	viewPort.Height = WINDOW_HEIGHT;
	viewPort.Width = WINDOW_WIDTH;
	viewPort.X = 0;
	viewPort.Y = 0;
	viewPort.MinZ = 0.0f;
	viewPort.MaxZ = 1.0f;
	gPD3DDevice->SetViewport(&viewPort);

}