/************************************************************************
* This project intends to make a summary of the DX APIs that I've learned
* before, which mainly include vertices&indices, mesh material texture,
* and D3DXMATRIX, camera, DXInput.
* And in order to get familiar with all the d3d9 and dinput8, I wrote all
* the code with original APIs, which will be wrapped in classes in the next
* project(ReorganizeOfAll)
*	I intend to finish this program with the following 7 steps:
*	1. Finish with basic functions, such as d3d9 ,dinput8, playSound, font;
*	2. Draw a sphere with colored vertices&indices, with fixed camera;
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
LPDIRECT3DVERTEXBUFFER9 gPVertexBuffer = nullptr;
LPDIRECT3DINDEXBUFFER9 gPIndexBuffer = nullptr;
LPD3DXMESH gPTeapotMesh = nullptr;
LPDIRECT3DTEXTURE9 gPTexture = nullptr;
LPDIRECT3DVERTEXBUFFER9 gPCubeVertexBuffer = nullptr;
LPDIRECT3DINDEXBUFFER9 gPCubeIndexBuffer = nullptr;

struct ScolorVertex
{
	float x;
	float y;
	float z;
//	float rhw;
	DWORD color;
};
struct SUVVertex
{
	float x;
	float y;
	float z;
	float u;
	float v;
};

#define D3DFVF_COLOR_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define D3DFVF_UV_VERTEX (D3DFVF_XYZ | D3DFVF_TEX1)
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
		Direct3DUpdate(hwnd);
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
	srand(unsigned(time(nullptr)));
	PlaySound(_T("孙燕姿 - 天使的指纹.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	D3DXCreateFont(gPD3DDevice, 38, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont);
	//draw a sphere, 16 triangles per circle.So there should be 16*(16/2+1) = 144 vertices
	//and there should be 16*16 triangles, so 16*16*3 indices.
	//ScolorVertex vertices[144];
	//float sphereRadius = 50.0f;
	//for (int i = 0; i < 16; i++)
	//{
	//	for (int j = 0; j < 9; j++)
	//	{
	//		vertices[i * 9 + j].x = sphereRadius*sin(D3DX_PI / 8 * j)*cos(D3DX_PI / 8 * i);
	//		vertices[i * 9 + j].y = sphereRadius*(-cos(D3DX_PI/8*j));
	//		vertices[i * 9 + j].z = sphereRadius*sin(D3DX_PI / 8 * j)*sin(D3DX_PI / 8 * i);
	//		vertices[i * 9 + j].color = D3DCOLOR_XRGB(11, 222, 1);
	//	}
	//}

	//gPD3DDevice->CreateVertexBuffer(sizeof(vertices), 0, D3DFVF_COLOR_VERTEX, D3DPOOL_DEFAULT, &gPVertexBuffer, nullptr);
	//void* pVertex;
	//gPVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertex, 0);
	//memcpy(pVertex, vertices, sizeof(vertices));
	//gPVertexBuffer->Unlock();	

	//int indices[768];
	//for (int i = 0; i < 16; i++)
	//{
	//	for (int j = 0; j < 8; j++)
	//	{
	//		if (i == 15)
	//		{
	//			indices[(i * 8 + j) * 6 + 0] = i * 9 + j;
	//			indices[(i * 8 + j) * 6 + 1] = j;
	//			indices[(i * 8 + j) * 6 + 2] = j + 1;
	//			indices[(i * 8 + j) * 6 + 3] = i * 9 + j;
	//			indices[(i * 8 + j) * 6 + 4] = i * 9 + j +1;
	//			indices[(i * 8 + j) * 6 + 5] = j + 1;
	//		}
	//		else
	//		{
	//			indices[(i * 8 + j) * 6 + 0] = i * 9 + j;
	//			indices[(i * 8 + j) * 6 + 1] = (i + 1) * 9 + j;
	//			indices[(i * 8 + j) * 6 + 2] = (i + 1) * 9 + j + 1;
	//			indices[(i * 8 + j) * 6 + 3] = i * 9 + j;
	//			indices[(i * 8 + j) * 6 + 4] = i * 9 + j + 1;
	//			indices[(i * 8 + j) * 6 + 5] = (i + 1) * 9 + j + 1;
	//		}			
	//	}
	//}

	//gPD3DDevice->CreateIndexBuffer(sizeof(indices), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &gPIndexBuffer, nullptr);
	void* pIndex;
	//gPIndexBuffer->Lock(0, 0, (void**)&pIndex, 0);
	//memcpy(pIndex, indices, sizeof(indices));
	//gPIndexBuffer->Unlock();

	//D3DXCreateTeapot(gPD3DDevice, &gPTeapotMesh, nullptr);

	SUVVertex cubeVertices[8]
	{
		{ -20.0f, 20.0f, -20.0f, 0.0f, 0.0f },
		{ 20.0f, 20.0f, -20.0f, 0.0f, 1.0f },
		{ 20.0f, 20.0f, 20.0f, 1.0f, 1.0f },
		{ -20.0f, 20.0f, 20.0f, 1.0f, 0.0f },
		{ -20.0f, -20.0f, -20.0f, 0.0f, 0.0f },
		{ 20.0f, -20.0f, -20.0f, 0.0f, 3.0f },
		{ 20.0f, -20.0f, 20.0f, 3.0f, 3.0f },
		{ -20.0f, -20.0f, 20.0f, 3.0f, 0.0f }
	};
	gPD3DDevice->CreateVertexBuffer(sizeof(cubeVertices), 0, D3DFVF_UV_VERTEX, D3DPOOL_DEFAULT, &gPCubeVertexBuffer, nullptr);
	gPCubeVertexBuffer->Lock(0, sizeof(cubeVertices), (void**)&pIndex, 0);
	memcpy(pIndex, cubeVertices, sizeof(cubeVertices));
	gPCubeVertexBuffer->Unlock();

	int cubeIndices[36];
	{
		cubeIndices[0] = 0;
		cubeIndices[1] = 1;
		cubeIndices[2] = 2;
		cubeIndices[3] = 0;
		cubeIndices[4] = 2;
		cubeIndices[5] = 3;
		cubeIndices[6] = 4;
		cubeIndices[7] = 5;
		cubeIndices[8] = 6;
		cubeIndices[9] = 4; 
		cubeIndices[10] = 6;
		cubeIndices[11] = 7;

		cubeIndices[12] = 0;
		cubeIndices[13] = 1;
		cubeIndices[14] = 5;
		cubeIndices[15] = 0;
		cubeIndices[16] = 4;
		cubeIndices[17] = 5;

		cubeIndices[18] = 1;
		cubeIndices[19] = 2;
		cubeIndices[20] = 6;
		cubeIndices[21] = 1;
		cubeIndices[22] = 5;
		cubeIndices[23] = 6;

		cubeIndices[24] = 2;
		cubeIndices[25] = 6;
		cubeIndices[26] = 7;
		cubeIndices[27] = 2;
		cubeIndices[28] = 3;
		cubeIndices[29] = 7;

		cubeIndices[30] = 0;
		cubeIndices[31] = 3;
		cubeIndices[32] = 7;
		cubeIndices[33] = 0;
		cubeIndices[34] = 4;
		cubeIndices[35] = 7;
	}
	gPD3DDevice->CreateIndexBuffer(sizeof(cubeIndices), 0, D3DFMT_INDEX32,
		D3DPOOL_DEFAULT, &gPCubeIndexBuffer, nullptr);
	gPCubeIndexBuffer->Lock(0, sizeof(cubeIndices), (void**)pIndex, 0);
	memcpy(pIndex, cubeIndices, sizeof(cubeIndices));
	gPCubeIndexBuffer->Unlock();

	D3DXCreateTextureFromFileEx(gPD3DDevice, L"pal5q.jpg", 0, 0, 6, 0, D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, 0, 0, &gPTexture);

	D3DMATERIAL9 mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gPD3DDevice->SetMaterial(&mtrl);

	gPD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //设置环境光

	gPD3DDevice->SetRenderState(D3DRS_CULLMODE, false);   //关掉背面消隐，无论是否顺时针，随机的那个三角形都会显示。 
	//gPD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //开启背面消隐
	//gPD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);   //将深度测试函数设为D3DCMP_LESS
	//gPD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);     //深度测试成功后，更新深度缓存

	return true;
}
void MatrixSet()
{
	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 0.0f, -600.0f);
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

	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	gPD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	gPD3DDevice->BeginScene();
	MatrixSet();

	D3DXMATRIX matWorld;	
	static float rx = 0;
	static float ry = 0;
	D3DXMATRIX rotX;
	D3DXMATRIX rotY;
	if (gKeyboardBuffer[DIK_W] & 0x80)
	{
		rx += 0.001f;
	}
	if (gKeyboardBuffer[DIK_S] & 0x80)
	{
		 rx -= 0.001f;
	}
	if (gKeyboardBuffer[DIK_A] & 0x80)
	{
		ry += 0.001f;
	}
	if (gKeyboardBuffer[DIK_D] & 0x80)
	{
		ry -= 0.001f;
	}	
	D3DXMatrixRotationX(&rotX, rx);
	D3DXMatrixRotationY(&rotY, ry);
	matWorld = rotX*rotY;
	gPD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	formatRect.top -= gFormatRectChange;
	gPFont->DrawTextW(nullptr, _T("Tayler is swift"), -1, &formatRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(11,222, 1));

	//gPD3DDevice->SetStreamSource(0, gPVertexBuffer, 0, sizeof(ScolorVertex));
	//gPD3DDevice->SetFVF(D3DFVF_COLOR_VERTEX);
	//gPD3DDevice->SetIndices(gPIndexBuffer);
	//gPD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 144, 0, 256);
	//

	//D3DXMATRIX matTeapotTrans;
	//D3DXMATRIX matTeapotScale;
	//D3DXMatrixTranslation(&matTeapotTrans,50.0f, 50.05f, 0.0f);
	//D3DXMatrixScaling(&matTeapotScale, 5.0f, 5.0f, 5.0f);
	//matTeapotScale = matTeapotScale*matTeapotTrans;
	//gPD3DDevice->SetTransform(D3DTS_WORLD, &matTeapotScale);
	//gPTeapotMesh->DrawSubset(0);

	D3DXMATRIX matCubeTrans;
	D3DXMatrixTranslation(&matCubeTrans, -20.0f, -20.05f, 0.0f);
	gPD3DDevice->SetTransform(D3DTS_WORLD, &matCubeTrans);

	gPD3DDevice->SetTexture(0, gPTexture);
	gPD3DDevice->SetStreamSource(0, gPCubeVertexBuffer, 0, sizeof(SUVVertex));
	gPD3DDevice->SetFVF(D3DFVF_UV_VERTEX);
	gPD3DDevice->SetIndices(gPCubeIndexBuffer);
	gPD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

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


