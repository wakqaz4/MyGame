/************************************************************************
* The first step of DirectX, mainly introducing pDevice and pFont                                                                  
************************************************************************/
#include <d3d9.h>
#include <tchar.h>
#include "d3dx9.h"
#include <time.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define WINDOW_WIDTH	800							//Ϊ���ڿ�ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ��
#define WINDOW_HEIGHT	600							//Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�
#define WINDOW_TITLE	L"D3DX9��Ⱦ�岽�� ʾ������"	//Ϊ���ڱ��ⶨ��ĺ�
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //����һ����ȫ�ͷź꣬���ں���COM�ӿ�ָ����ͷ�

extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern HRESULT Direct3DInit(HWND hwnd);
extern HRESULT ObjectsInit(HWND hwnd);
extern void Direct3DRender(HWND hwnd);
extern bool Direct3DClearup(HWND hwnd);
float GetFPS();

LPDIRECT3DDEVICE9 gPD3dDevice = nullptr;
ID3DXFont* gPFont = nullptr;
float gFPS = 0.0f;
WCHAR gStrFPS[50];

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);			//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;					//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra = 0;								//������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra = 0;							//���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //ָ��������Ĺ������
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = _T("abc");						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = _T("Class");
	wndClass.hIconSm = nullptr;

	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("Class"), WINDOW_TITLE,				//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (Direct3DInit(hwnd) != S_OK)
	{
		MessageBox(hwnd, _T("D3d Init failed!"), _T("Error!"), MB_OK);
		return -1;
	}

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

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
	UnregisterClass(_T("Class"), hInstance);
	return 0;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
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
/************************************************************************
* Direct3DX init with 5 procedures                                                                  
************************************************************************/
HRESULT Direct3DInit(HWND hwnd)
{
	PlaySound(_T("Dream - Get Over.wav"), nullptr, SND_ASYNC| SND_FILENAME| SND_LOOP);

	//////////////////////////////////////////////////////////////////////////
	// 1st step: Create Direct3D interface  
	//////////////////////////////////////////////////////////////////////////
	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D == nullptr)
	{
		return E_FAIL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 2nd step: Get hardware device 
	//////////////////////////////////////////////////////////////////////////
	D3DCAPS9 caps;
	int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	//If hardware supports vertex processing, use hardware, else use software
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	//////////////////////////////////////////////////////////////////////////
	// 3rd step: Create d3d present parameters struct and fill in the contents,  
	// to prepare for the 4th step which is used for creating device
	//////////////////////////////////////////////////////////////////////////
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;

	//////////////////////////////////////////////////////////////////////////
	// 4th step: Create device  
	// D3DDEVTYPE_HAL means the hardware abstract layer
	//////////////////////////////////////////////////////////////////////////
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd, vp, &d3dpp, &gPD3dDevice)))
	{
		return E_FAIL;
	}
	//Release LPDIRECT3D9 
	SAFE_RELEASE(pD3D);

	if (ObjectsInit(hwnd) != S_OK)
	{
		return E_FAIL;
	};
	return S_OK;
}
HRESULT ObjectsInit(HWND hwnd)
{
	if (FAILED(D3DXCreateFont(gPD3dDevice, 36, 0, 0, 1, false, 0, 0, 0, 0, _T("����"), &gPFont)))
	{
		return E_FAIL;
	}
	srand(unsigned(time(nullptr)));
	return S_OK;
}
void Direct3DRender(HWND hwnd)
{
	//////////////////////////////////////////////////////////////////////////
	// Render step 1: Clear;  
	//////////////////////////////////////////////////////////////////////////
	gPD3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	//////////////////////////////////////////////////////////////////////////
	// Render step 2: Begin, using BeginScene()  
	//////////////////////////////////////////////////////////////////////////
	gPD3dDevice->BeginScene();

	//////////////////////////////////////////////////////////////////////////
	// Render step 3: Render formally  
	//////////////////////////////////////////////////////////////////////////
	
	//Write Frame on top
	int charCount = swprintf_s(gStrFPS, 20, _T("FPS: %0.3f"), GetFPS());
	gPFont->DrawTextW(nullptr, gStrFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));

	formatRect.top = 100;
	gPFont->DrawTextW(nullptr, _T("The first line: ʥ����������"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(255, 39, 136));

	formatRect.top = 250;
	gPFont->DrawTextW(nullptr, _T("�ڶ��죬�ϵ�Ҫ�ⰵ����"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(255, 255, 255));

	formatRect.top = 400;
	gPFont->DrawTextW(nullptr, _T("WANDS - ���礬�K��ޤǤϡ�"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256));

	//////////////////////////////////////////////////////////////////////////
	// Render step 4: Render end  
	//////////////////////////////////////////////////////////////////////////
	gPD3dDevice->EndScene();

	//////////////////////////////////////////////////////////////////////////
	// Render step 5: Present and Flip over  
	//////////////////////////////////////////////////////////////////////////
	gPD3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
}
bool Direct3DClearup(HWND hwnd)
{
	SAFE_RELEASE(gPFont);
	SAFE_RELEASE(gPD3dDevice);
	return true;
}

float GetFPS()
{
	//�����ĸ���̬����
	static float  fps = 0; //������Ҫ�����FPSֵ
	static int     frameCount = 0;//֡��
	static float  currentTime = 0.0f;//��ǰʱ��
	static float  lastTime = 0.0f;//����ʱ��

	frameCount++;//ÿ����һ��Get_FPS()������֡������1
	currentTime = timeGetTime()*0.001f;//��ȡϵͳʱ�䣬����timeGetTime�������ص����Ժ���Ϊ��λ��ϵͳʱ�䣬������Ҫ����0.001���õ���λΪ���ʱ��

	//�����ǰʱ���ȥ����ʱ�������1���ӣ��ͽ���һ��FPS�ļ���ͳ���ʱ��ĸ��£�����֡��ֵ����
	if (currentTime - lastTime > 1.0f) //��ʱ�������1����
	{
		fps = (float)frameCount / (currentTime - lastTime);//������1���ӵ�FPSֵ
		lastTime = currentTime; //����ǰʱ��currentTime��������ʱ��lastTime����Ϊ��һ��Ļ�׼ʱ��
		frameCount = 0;//������֡��frameCountֵ����
	}

	return fps;
}
