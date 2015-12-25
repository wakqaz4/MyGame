/************************************************************************
* This program renderes a circle with 17 indexes and vertices                                                                  
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
/************************************************************************
* Macros and other helpers                                                                  
************************************************************************/
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WND_CLASS_NAME _T("ClassName")
#define SAFE_RELEASE(p) {if (p){(p)->Release(); p = nullptr;}}


/************************************************************************
* External functions and global variables                                                                  
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern void Direct3DUpdate(HWND hwnd);
extern bool Direct3DExit(HWND hwnd);
extern bool ObjectInit(HWND hwnd);
extern float GetFPS();

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
ID3DXFont* gPFont = nullptr;
LPDIRECT3DVERTEXBUFFER9 gPVertexBuffer = nullptr;
LPDIRECT3DINDEXBUFFER9 gPIndexBuffer = nullptr;
float gFPS{ 0 };
WCHAR gStrFPS[50]{0};

const float gRadius = 100.0f;
const float PI = 3.14159f;
/************************************************************************
* Vertex struct and index struct                                                                  
************************************************************************/
struct CUSTOM_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
};
#define D3DFVF_CUSTOM_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };							//��WINDCLASSEX������һ��������
	wndClass.cbSize = sizeof(WNDCLASSEX);			//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;					//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra = 0;								//������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra = 0;							//���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //ָ��������Ĺ������
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = WND_CLASS_NAME;		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	//��2�����ڴ����Ĳ���֮����ע�ᴰ����
	if (!RegisterClassEx(&wndClass))				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;

	//��3�����ڴ����Ĳ���֮������ʽ��������
	HWND hwnd = CreateWindow(WND_CLASS_NAME, _T("end"),				//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
	{
		MessageBox(hwnd, _T("Init direct3d failed!"), _T("D3D ERROR!"), MB_OK);
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
	UnregisterClass(WND_CLASS_NAME, hInstance);
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
    // D3D init step1:   init d3d9
    //////////////////////////////////////////////////////////////////////////	
	LPDIRECT3D9 pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D9 == nullptr)
	{
		return false;
	}

    //////////////////////////////////////////////////////////////////////////
    // D3D init step2: caps  
    //////////////////////////////////////////////////////////////////////////    
	D3DCAPS9 caps;
	if (FAILED(pD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return false;
	}
	int vp = 0;
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
    
    //////////////////////////////////////////////////////////////////////////
    // D3D init step3: present parameters  
    //////////////////////////////////////////////////////////////////////////
	D3DPRESENT_PARAMETERS d3dpp;
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

    //////////////////////////////////////////////////////////////////////////
    // D3D init Step4: create device  
    //////////////////////////////////////////////////////////////////////////
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
void Direct3DUpdate(HWND hwnd)
{
    //////////////////////////////////////////////////////////////////////////
    // Five steps of rendering: clear, beginScene, render, EndScene, present  
    //////////////////////////////////////////////////////////////////////////   
	gPD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	gPD3DDevice->BeginScene();

	gPD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	gPD3DDevice->SetStreamSource(0, gPVertexBuffer, 0, sizeof(CUSTOM_VERTEX));
	gPD3DDevice->SetFVF(D3DFVF_CUSTOM_VERTEX);
	gPD3DDevice->SetIndices(gPIndexBuffer);
	gPD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 17, 0, 16);

	int charCount = swprintf_s(gStrFPS, 20, _T("FPS:%0.3f"), GetFPS());
	gPFont->DrawText(NULL, gStrFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));


	gPD3DDevice->EndScene();
	gPD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}
bool Direct3DExit(HWND hwnd)
{
	SAFE_RELEASE(gPFont);
	SAFE_RELEASE(gPVertexBuffer);
	SAFE_RELEASE(gPIndexBuffer);
	SAFE_RELEASE(gPD3DDevice);
	return true;
}
/************************************************************************
* Objects such as vertex buffer, index buffer, fonts are initialized here
************************************************************************/
bool ObjectInit(HWND hwnd)
{

	PlaySound(_T("���� - ԡ������.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	srand(unsigned(time(NULL)));

	D3DXCreateFont(gPD3DDevice, 40, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"), &gPFont);

	//////////////////////////////////////////////////////////////////////////
	// Fill in the vertex struct  
	//////////////////////////////////////////////////////////////////////////
	CUSTOM_VERTEX vertices[17];
	vertices[0].x = 400.0f;
	vertices[0].y = 300.0f;
	vertices[0].z = 0.0f;
	vertices[0].rhw = 1.0f;
	vertices[0].color = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	for (int i = 1; i < 17; i++)
	{
		vertices[i].x = gRadius*cos(PI*(i - 1)/ 8) + vertices[0].x;
		vertices[i].y = gRadius*sin(PI*(i - 1)/ 8) + vertices[0].y;
		vertices[i].z = 0.0f;
		vertices[i].rhw = 1.0f;
		vertices[i].color = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	}

	gPD3DDevice->CreateVertexBuffer(17 * sizeof(CUSTOM_VERTEX), 0, D3DFVF_CUSTOM_VERTEX, D3DPOOL_DEFAULT,
		&gPVertexBuffer, nullptr);
	void* pVertices{0};
	gPVertexBuffer->Lock(0, 17 * sizeof(CUSTOM_VERTEX), (void**)&pVertices, 0);
	memcpy(pVertices, vertices, 17 * sizeof(CUSTOM_VERTEX));
	gPVertexBuffer->Unlock();

    //////////////////////////////////////////////////////////////////////////
    // Fill in the index struct  
    //////////////////////////////////////////////////////////////////////////	
	int indexes[48];
	for (int i = 0; i < 16; i++)
	{
		indexes[i * 3] = 0;
		indexes[i * 3 + 1] = i + 1;
		indexes[i * 3 + 2] = i + 2;
	}
	indexes[47] = 1;
	
	gPD3DDevice->CreateIndexBuffer(48 * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &gPIndexBuffer, nullptr);
	int* pIndexes{0};
	gPIndexBuffer->Lock(0, 0, (void**)&pIndexes, 0);
	memcpy(pIndexes, indexes, 48 * sizeof(int));
	gPIndexBuffer->Unlock();

	return true;
}

float GetFPS()
{
	static float prevTime = 0;
	static float currTime = 0;
	static float FPS = 0;
	static int tickCount = 0;

	tickCount++;
	currTime = timeGetTime();
	if (currTime - prevTime >= 1000.0f)
	{
		FPS = (float)tickCount*1000.0f / (currTime - prevTime);
		prevTime = currTime;
		tickCount = 0;
	}
	return FPS;
}