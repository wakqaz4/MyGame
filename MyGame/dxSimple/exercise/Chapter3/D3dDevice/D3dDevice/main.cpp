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

#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	L"D3DX9渲染五步曲 示例程序"	//为窗口标题定义的宏
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //定义一个安全释放宏，便于后面COM接口指针的释放

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
	wndClass.cbSize = sizeof(WNDCLASSEX);			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra = 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = _T("abc");						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = _T("Class");
	wndClass.hIconSm = nullptr;

	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("Class"), WINDOW_TITLE,				//喜闻乐见的创建窗口函数CreateWindow
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
	if (FAILED(D3DXCreateFont(gPD3dDevice, 36, 0, 0, 1, false, 0, 0, 0, 0, _T("楷体"), &gPFont)))
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
	gPFont->DrawTextW(nullptr, _T("The first line: 圣经：创世纪"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(255, 39, 136));

	formatRect.top = 250;
	gPFont->DrawTextW(nullptr, _T("第二天，上帝要光暗分离"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(255, 255, 255));

	formatRect.top = 400;
	gPFont->DrawTextW(nullptr, _T("WANDS - 世界が終るまでは…"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256));

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
	//定义四个静态变量
	static float  fps = 0; //我们需要计算的FPS值
	static int     frameCount = 0;//帧数
	static float  currentTime = 0.0f;//当前时间
	static float  lastTime = 0.0f;//持续时间

	frameCount++;//每调用一次Get_FPS()函数，帧数自增1
	currentTime = timeGetTime()*0.001f;//获取系统时间，其中timeGetTime函数返回的是以毫秒为单位的系统时间，所以需要乘以0.001，得到单位为秒的时间

	//如果当前时间减去持续时间大于了1秒钟，就进行一次FPS的计算和持续时间的更新，并将帧数值清零
	if (currentTime - lastTime > 1.0f) //将时间控制在1秒钟
	{
		fps = (float)frameCount / (currentTime - lastTime);//计算这1秒钟的FPS值
		lastTime = currentTime; //将当前时间currentTime赋给持续时间lastTime，作为下一秒的基准时间
		frameCount = 0;//将本次帧数frameCount值清零
	}

	return fps;
}
