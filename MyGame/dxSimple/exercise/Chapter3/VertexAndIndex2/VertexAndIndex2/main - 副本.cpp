/************************************************************************
* Draw a man with cat ears while smiling, using vertices&indices                                                                  
************************************************************************/
#include <windows.h>
#include <time.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

/************************************************************************
* Some necessary macros                                                                  
************************************************************************/
#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	L"迈向三维世界：Direct3D四大变换  示例程序"	//为窗口标题定义的宏
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //定义一个安全释放宏，便于后面COM接口指针的释放
#define PI 3.14159

/************************************************************************
* Declare external functions and global variables                                                                  
************************************************************************/
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern void Direct3DUpdate(HWND hwnd);
extern bool Direct3DRelease(HWND hwnd);
extern bool ObjectInit(HWND hwnd);
extern float GetFPS();
float gFPS{ 0 };
WCHAR gStrFPS[50]{0};

struct CUSTOM_VERTEX
{
	float x;
	float y;
	float z;
	float rhw;
	DWORD color;
};
#define D3DFVF_CUSTOM_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
ID3DXFont* gPFont = nullptr;
LPDIRECT3DVERTEXBUFFER9 gPVertexBuffer = nullptr;
LPDIRECT3DINDEXBUFFER9 gPIndexBuffer = nullptr;

/************************************************************************
* Function definitions                                                                  
************************************************************************/
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
	wndClass.style = CS_VREDRAW|CS_HREDRAW;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(nullptr, _T("Content"), _T("Caption"), MB_OK);
		return -1;
	}

	HWND hwnd = CreateWindow(_T("ClassName"), _T("abc"), WS_OVERLAPPEDWINDOW,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
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
		}
	}
	UnregisterClass(_T("ClassName"), hInstance);
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
		Direct3DRelease(hwnd);
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
	// D3D Init step1: D3D9  
	//////////////////////////////////////////////////////////////////////////
	LPDIRECT3D9 pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D9 == nullptr)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// D3D Init Step2: caps&vp  
	//////////////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////
	// D3D Init step3: parameter  
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
	// D3D Init step4: createDevice with vp&D3D9&presentParameters  
	//////////////////////////////////////////////////////////////////////////
	if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &gPD3DDevice)))
	{
		return false;
	}
	SAFE_RELEASE(pD3D9);

	//gPD3DDevice->SetRenderState(D3DRS_LIGHTING, false);			//关闭光照
	//gPD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//背面消隐
	//gPD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	
	if (!ObjectInit(hwnd))
	{
		return -1;
	}

	return true;
}
void Direct3DUpdate(HWND hwnd)
{
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 214, 158), 1.0f, 0);
	gPD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	gPD3DDevice->BeginScene();

	gPD3DDevice->SetStreamSource(0, gPVertexBuffer, 0, sizeof(CUSTOM_VERTEX));
	gPD3DDevice->SetFVF(D3DFVF_CUSTOM_VERTEX);
	gPD3DDevice->SetIndices(gPIndexBuffer);
	gPD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 17, 0, 16);

	int charCount = swprintf_s(gStrFPS, 20, _T("FPS:%0.3f"), GetFPS());
	gPFont->DrawText(NULL, gStrFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 39, 136));


	gPD3DDevice->EndScene();
	gPD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

}
bool Direct3DRelease(HWND hwnd)
{
	SAFE_RELEASE(gPFont);
	SAFE_RELEASE(gPVertexBuffer);
	SAFE_RELEASE(gPIndexBuffer);
	SAFE_RELEASE(gPD3DDevice);
	return true;
}
bool ObjectInit(HWND hwnd)
{
	srand(unsigned(time(nullptr)));
	PlaySound(_T("Kalafina - believe.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	D3DXCreateFont(gPD3DDevice, 40, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont);

	//////////////////////////////////////////////////////////////////////////
	// some constant variables used to be modified to fit the screen  
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const float headRadius = 50.0f;
	const float neckWidth = 20.0f;
	const float neckHeight = 30.0f;
	const float bodyWidth = 200.0f;
	const float bodyHeight = 200.0f;
	const float armWidth = 100.0f;
	const float armHeight = 40.0f;
	const float legWidth = 40.0f;
	const float legHeight = 200.0f;
	const float handRadius = 30.0f;
	const float footRadius = 30.0f;
	//////////////////////////////////////////////////////////////////////////
	// Set vertices  
	//////////////////////////////////////////////////////////////////////////
	enum ORGNIZM
	{
		HEAD, 
		NECK, 
		BODY,
		LEFT_ARM,
		RIGHT_ARM,
		LEFT_HAND,
		RIGHT_HAND,
		LEFT_LEG,
		RIGHT_LEG,
		LEFT_FOOT,
		RIGHT_FOOT,
		ORGAN_COUNT
	};
	const int verticesNum[ORGAN_COUNT] = { 17, 4, 4, 4, 4, 9, 9, 4, 4, 4, 4 };
	CUSTOM_VERTEX verticesHead[17];
	CUSTOM_VERTEX verticesNeck[4] = { 0 };
	CUSTOM_VERTEX verticesBody[4] = { 0 };
	CUSTOM_VERTEX verticesLeftArm[4] = { 0 };
	CUSTOM_VERTEX verticesRightArm[4] = { 0 };
	CUSTOM_VERTEX verticesLeftHand[9] = { 0 };
	CUSTOM_VERTEX verticesRightHand[9] = { 0 };
	CUSTOM_VERTEX verticesLeftLeg[4] = { 0 };
	CUSTOM_VERTEX verticesRightLeg[4] = { 0 };
	CUSTOM_VERTEX verticesLeftFoot[4] = { 0 };
	CUSTOM_VERTEX verticesRightFoot[4] = { 0 };
	int verticesNumSum = 21;
	//for (int i = 0; i < ORGAN_COUNT; i++)
	//{
	//	verticesNumSum += verticesNum[i];
	//}

	/**
	 *	Then fill in the custom vertices
	 *	*/		
	verticesHead[0].x = 400.0f;
	verticesHead[0].y = 50.0f;
	verticesHead[0].z = 0.0f;
	verticesHead[0].rhw = 1.0f;
	verticesHead[0].color = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);

	for (int i = 0; i < verticesNum[HEAD]-1; i++)
	{
		verticesHead[i + 1].x = verticesHead[0].x + headRadius*cos(i*PI / 8);
		verticesHead[i + 1].y = verticesHead[0].y + headRadius*sin(i*PI / 8);
		verticesHead[i + 1].z = 0.0f;
		verticesHead[i + 1].rhw = 1.0f;
		verticesHead[i + 1].color = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	}

	verticesNeck[0].x = 400.0f - neckWidth/2;
	verticesNeck[1].x = verticesNeck[0].x;
	verticesNeck[2].x = verticesNeck[0].x + neckWidth;
	verticesNeck[3].x = verticesNeck[2].x;

	verticesNeck[0].y = headRadius * 2 - neckHeight / 3;
	verticesNeck[1].y = verticesNeck[0].y + neckHeight;
	verticesNeck[2].y = verticesNeck[0].y;
	verticesNeck[3].y = verticesNeck[1].y;

	for (int i = 0; i < 4; i++)
	{
		verticesNeck[i].z = 0.0f;
		verticesNeck[i].rhw = 1.0f;
		verticesHead[i + 1].color = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	}
	/**
	 *	Then copy to vertex buffer
	 *	*/
	gPD3DDevice->CreateVertexBuffer(verticesNumSum*sizeof(CUSTOM_VERTEX), 0, D3DFVF_CUSTOM_VERTEX, D3DPOOL_DEFAULT, &gPVertexBuffer, nullptr);
	void* pVertices;
	gPVertexBuffer->Lock(0, verticesNumSum*sizeof(CUSTOM_VERTEX), (void**)(&pVertices), 0);
	memcpy(pVertices, verticesHead, verticesNumSum*sizeof(CUSTOM_VERTEX));
	gPVertexBuffer->Unlock();

	//////////////////////////////////////////////////////////////////////////
	// Set indices  
	//////////////////////////////////////////////////////////////////////////
		//declare indices of different parts
	int indicesHead[48]{0};
	int indicesNeck[6]{0};
	int indicesNumSum = 54;
	int iIndicesNum = 0;
		//fill in the head indices	
	for (int i = 0; i < 16; i++)
	{
		indicesHead[i * 3] = 0;
		indicesHead[i * 3 + 1] = i + 1;
		indicesHead[i * 3 + 2] = i + 2;
		iIndicesNum++;
	}
	indicesHead[47] = 1;
		//fill in the neck indices
	for (int i = 0; i < 2; i++)
	{
		indicesNeck[i*3] = iIndicesNum + i;
		indicesNeck[i*3 + 1] = 16 + 1;
		indicesNeck[i*3 + 2] = 16 + 2;
		iIndicesNum++;
	}
	indicesNeck[3] = 16 + 3;
	
		//create pIndices
	if (FAILED(gPD3DDevice->CreateIndexBuffer(indicesNumSum*sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &gPIndexBuffer, nullptr)))
	{
		return false;
	}
	int* pIndices;
	gPIndexBuffer->Lock(0, 0, (void**)(&pIndices), 0);
	memcpy(pIndices, indicesHead, indicesNumSum*sizeof(int));
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