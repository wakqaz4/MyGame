/************************************************************************
* This program implements all four transform matrixes of Direct3D.                                                                  
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
#define WINDOW_CLASS_NAME _T("ClassName")
#define SAFE_RELEASE(p) {if (p){p->Release(); p = nullptr;}}

#define D3DFVF_CUSTOM_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables, which includes:
// WndProc, Direct3D init, render, clearup, ObjectInit, &
// gPD3DDevice, gPFont, gStrFPS, gPVertexBuffer, gPIndexBuffer
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern void Direct3DRender(HWND hwnd);
extern bool Direct3DClearup(HWND hwnd);
extern bool ObjectInit(HWND hwnd);

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
ID3DXFont* gPFont = nullptr;
WCHAR gStrFPS[50]{0};
LPDIRECT3DVERTEXBUFFER9 gPVertexBuffer = nullptr;
LPDIRECT3DINDEXBUFFER9 gPIndexBuffer = nullptr;

struct CUSTOM_VERTEX
{
	float x;
	float y;
	float z;
	DWORD color;
};

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
	wndClass.lpszClassName = WINDOW_CLASS_NAME;
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(nullptr, _T("Init Window Class Failed!"), _T("INIT ERROR!"), MB_OK);
		return -1;
	}

	HWND hwnd = CreateWindow(WINDOW_CLASS_NAME, _T("For Game Develop!"), WS_OVERLAPPEDWINDOW,
		0, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
	{
		MessageBox(nullptr, _T("Init Direct3D Failed!"), _T("INIT ERROR!"), MB_OK);
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
	LPDIRECT3D9 pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
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
	gPD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);      
	gPD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   

	return true;
}

void matrixSet()
{
	//////////////////////////////////////////////////////////////////////////
	// We need to make 4 transform matrixes here, which are world, view(camera),
	// projection, and setting parameters(viewport);
	//////////////////////////////////////////////////////////////////////////
	
	D3DXMATRIX matWorldTrans;
	D3DXMatrixIdentity(&matWorldTrans);
	D3DXMATRIX matRotX;
	D3DXMATRIX matRotY;
	D3DXMATRIX matRotZ;
	D3DXMatrixRotationX(&matRotX, timeGetTime()/1000.0f);
	D3DXMatrixRotationY(&matRotY, timeGetTime()/1000.0f / 2);
	D3DXMatrixRotationZ(&matRotZ, timeGetTime()/1000.0f / 3);
	matWorldTrans = matRotX*matRotY*matRotZ;
	gPD3DDevice->SetTransform(D3DTS_WORLD, &matWorldTrans);

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye{ 0.0f, 0.0f, -200.0f };
	D3DXVECTOR3 vAt{ 0.0f, 0.0f, 0.0f };
	D3DXVECTOR3 vUp{ 0.0f, 1.0f, 0.0f };
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	gPD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 6.0f, 1.0f, 1.0f, 1000.0f);
	gPD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	gPD3DDevice->SetViewport(&vp);
}

void Direct3DRender(HWND hwnd)
{
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 14, 139), 1.0f, 0);
	gPD3DDevice->BeginScene();

	matrixSet();
	gPD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	gPD3DDevice->SetStreamSource(0, gPVertexBuffer, 0, sizeof(CUSTOM_VERTEX));
	gPD3DDevice->SetFVF(D3DFVF_CUSTOM_VERTEX);
	gPD3DDevice->SetIndices(gPIndexBuffer);
	gPD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	int strLen = swprintf_s(gStrFPS, 20, _T("FPS: %f"), 300.0f);
	gPFont->DrawTextW(nullptr, gStrFPS, strLen, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(18, 39, 136));

	gPD3DDevice->EndScene();

	gPD3DDevice->Present(nullptr, nullptr, hwnd, nullptr);

}

bool Direct3DClearup(HWND hwnd)
{
	SAFE_RELEASE(gPFont);
	SAFE_RELEASE(gPVertexBuffer);
	SAFE_RELEASE(gPIndexBuffer);
	SAFE_RELEASE(gPD3DDevice);
	return true;
}

bool ObjectInit(HWND hwnd)
{
	if (FAILED(D3DXCreateFont(gPD3DDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}
	srand(unsigned(time(nullptr)));
	PlaySound(_T("岩田さゆり - Thank You For Everything.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);

	//////////////////////////////////////////////////////////////////////////
	// create vertices  
	//////////////////////////////////////////////////////////////////////////
	CUSTOM_VERTEX vertices[] = 
	{
		{ -20.0f, 20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, 20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, -20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, -20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, -20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, -20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) }
	};
	if (FAILED(gPD3DDevice->CreateVertexBuffer(8 * sizeof(CUSTOM_VERTEX), 0, D3DFVF_CUSTOM_VERTEX, D3DPOOL_DEFAULT, &gPVertexBuffer, nullptr)))
	{
		return false;
	}
	void* pVertices;
	gPVertexBuffer->Lock(0, 8 * sizeof(CUSTOM_VERTEX), (void**)(&pVertices), 0);
	memcpy(pVertices, vertices, sizeof(vertices));
	gPVertexBuffer->Unlock();

	//////////////////////////////////////////////////////////////////////////
	// create indices  
	//////////////////////////////////////////////////////////////////////////
	int* pIndices = nullptr;
	if (FAILED(gPD3DDevice->CreateIndexBuffer(36 * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &gPIndexBuffer, nullptr)))
	{
		return false;
	}
	gPIndexBuffer->Lock(0, 0, (void**)(&pIndices), 0);
	// 顶面
	pIndices[0] = 0, pIndices[1] = 1, pIndices[2] = 2;
	pIndices[3] = 0, pIndices[4] = 2, pIndices[5] = 3;
	// 正面
	pIndices[6] = 0, pIndices[7] = 3, pIndices[8] = 7;
	pIndices[9] = 0, pIndices[10] = 7, pIndices[11] = 4;
	// 左侧面
	pIndices[12] = 0, pIndices[13] = 4, pIndices[14] = 5;
	pIndices[15] = 0, pIndices[16] = 5, pIndices[17] = 1;
	// 右侧面
	pIndices[18] = 2, pIndices[19] = 6, pIndices[20] = 7;
	pIndices[21] = 2, pIndices[22] = 7, pIndices[23] = 3;
	// 背面
	pIndices[24] = 2, pIndices[25] = 5, pIndices[26] = 6;
	pIndices[27] = 2, pIndices[28] = 1, pIndices[29] = 5;
	// 底面
	pIndices[30] = 4, pIndices[31] = 6, pIndices[32] = 5;
	pIndices[33] = 4, pIndices[34] = 7, pIndices[35] = 6;
	gPIndexBuffer->Unlock();

	return true;
}

