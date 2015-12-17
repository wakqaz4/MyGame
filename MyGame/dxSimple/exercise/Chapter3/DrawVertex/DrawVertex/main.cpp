/************************************************************************
* This program draws two triangles using vertex with D3D                                                                  
************************************************************************/

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <d3d9.h>
#include "d3dx9.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
#define SAFE_RELEASE(p) {if (p != nullptr){(p)->Release();(p) = nullptr;}}

extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern void Direct3DUpdate(HWND hwnd);
extern bool Direct3DExit(HWND hwnd);
extern bool ObjectInit(HWND hwnd);
extern float GetFPS();

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
ID3DXFont* gPFont = nullptr;
float gFPS = 0;
WCHAR gStrFPS[50]{0};
LPDIRECT3DVERTEXBUFFER9 gPVertexBuffer = nullptr;

/************************************************************************
* Custom vertex format
************************************************************************/
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
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
	wndClass.lpszClassName = _T("WinClass");
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_HREDRAW | CS_HREDRAW;

	if (!RegisterClass(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("WinClass"), _T("Drawing Vertex"), WS_OVERLAPPEDWINDOW,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
	{
		MessageBox(hwnd, _T("Initialize Direct3D failed!"), _T("D3D ERROR!"), MB_OK);
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
			Direct3DUpdate(hwnd);			
		}	
	}
	UnregisterClass(_T("WinClass"), hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
		Direct3DUpdate(hwnd);
		ValidateRect(hwnd, nullptr);
		break;
	case WM_DESTROY:
		Direct3DExit(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
    
	return 0;
}

bool Direct3DInit(HWND hwnd)
{
	//////////////////////////////////////////////////////////////////////////
	// D3D init step 1: Create D3D9
	//////////////////////////////////////////////////////////////////////////
	LPDIRECT3D9 pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D9 == nullptr)
	{
		MessageBox(nullptr, _T("Creating D3D9 failed!"), _T("D3D ERROR!"), MB_OK);
		return false;
	}

    //////////////////////////////////////////////////////////////////////////
    // Step2: create CAPS and vp  
    //////////////////////////////////////////////////////////////////////////

        //  Get device caps
	D3DCAPS9 caps;
	if (FAILED(pD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return false;
	}
            // And then get hardware 
	int vp;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	//////////////////////////////////////////////////////////////////////////
	//  step 3: create Present parameter and fill in the struct   
	//////////////////////////////////////////////////////////////////////////

	D3DPRESENT_PARAMETERS params{ 0 };
	params.BackBufferWidth = WINDOW_WIDTH;
	params.BackBufferHeight = WINDOW_HEIGHT;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;
	params.BackBufferCount = 1;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = 0;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = hwnd;
	params.Windowed = true;
	params.EnableAutoDepthStencil = true;
	params.AutoDepthStencilFormat = D3DFMT_D24S8;
	params.Flags = 0;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    //////////////////////////////////////////////////////////////////////////
    // Step 4: create device using pD3D9, vp, params  
    //////////////////////////////////////////////////////////////////////////

	if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &params, &gPD3DDevice)))
	{
		return false;
	}

    //Remember to release the pD3D9 pointer at last
	SAFE_RELEASE(pD3D9);

	ObjectInit(hwnd);
    
    return true;
}

void Direct3DUpdate(HWND hwnd)
{

	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
    //////////////////////////////////////////////////////////////////////////
    // step 1: Clear!!!!!!!!!!!!!!!!!  
    //////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////// 
	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    //////////////////////////////////////////////////////////////////////////
    // Step 2: Begin Scene  
    //////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////// 
	gPD3DDevice->BeginScene();

    //////////////////////////////////////////////////////////////////////////
    // Step3: Formal Rendering  
    //////////////////////////////////////////////////////////////////////////
	    //Render fonts
    int charCount = swprintf_s(gStrFPS, 20, _T("FPS:%0.3f"),GetFPS());
	gPFont->DrawTextW(nullptr, gStrFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));
        //Render vertex
	gPD3DDevice->SetStreamSource(0, gPVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	gPD3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	gPD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    //////////////////////////////////////////////////////////////////////////
    // Step4: End Scene  
    //////////////////////////////////////////////////////////////////////////
    gPD3DDevice->EndScene();

	gPD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
    

}

bool Direct3DExit(HWND hwnd)
{
	SAFE_RELEASE(gPVertexBuffer);
	SAFE_RELEASE(gPFont);
	SAFE_RELEASE(gPD3DDevice);
	return true;
}

bool ObjectInit(HWND hwnd)
{
	if (FAILED(D3DXCreateFont(gPD3DDevice, 36, 0, 0, 1, false, 0, 0, 0, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}
	PlaySound(_T("sweet ARMS - デート・ア・ライブ.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);

	srand(unsigned(time(nullptr)));

    //////////////////////////////////////////////////////////////////////////
    // Vertex shader step1: create Vertex Buffer  
    //////////////////////////////////////////////////////////////////////////
	if (FAILED(gPD3DDevice->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &gPVertexBuffer, nullptr)))
	{
		return E_FAIL;
	}
    //////////////////////////////////////////////////////////////////////////
    // Vertex shader step2: visit Vertex buffer  
    //////////////////////////////////////////////////////////////////////////

        //Set vertices
	CUSTOMVERTEX vertices[] = 
	{
		{ 300.0f, 100.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(rand()%256, rand()%256, rand()%256)},
		{ 500.0f, 100.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 400.0f, 250.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 400.0f, 250.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ float(800 * rand() / (RAND_MAX + 1.0f)), float(600 * rand() / (RAND_MAX + 1.0f)), 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ float(800 * rand() / (RAND_MAX + 1.0f)), float(600 * rand() / (RAND_MAX + 1.0f)), 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) }
	};
        //Fill in vertex buffer
	VOID* pVertices;
	if (FAILED(gPVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, vertices, sizeof(vertices));
	gPVertexBuffer->Unlock();

	gPD3DDevice->SetRenderState(D3DRS_CULLMODE, false);

    return true;
}

float GetFPS()
{
	static float prevTime = 0;
	static float currTime = 0;
	static int tickCount = 0;
	static 	float FPS = 0;
	tickCount++;
	currTime = timeGetTime();
	if (currTime - prevTime >= 1000)
	{	
		FPS = ((float)tickCount)*1000.0f / (currTime - prevTime);
		tickCount = 0;
		prevTime = currTime;		
	}
	return FPS;
}