/************************************************************************
* This project also draws a mesh with .X file, and it also used DirectInput
* to deal with mouse&keyboard inputs. However, this project used a class
* CDirectInput instead of all the smothering pMouse, pKeyboard...
************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include "MyCommon.h"
#include "CDirectInputClass.h"

#pragma comment (lib, "winmm")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables here.
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd, HINSTANCE hInstance);
extern bool ObjectInit(HWND hwnd, HINSTANCE hInstance);
extern void Direct3DRender(HWND hwnd);
extern void Direct3DUpdate(HWND hwnd);
extern bool Direct3DClearup(HWND hwnd);
extern void MatrixSet();
extern void LightSet();

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
WCHAR gStrFPS[50]{0};
LPD3DXFONT gPFont = nullptr;
CDirectInputClass gDirectInputClass;
LPD3DXMESH gPMesh = nullptr;
D3DMATERIAL9* gPMaterial = nullptr;
LPDIRECT3DTEXTURE9* gPTexture = nullptr;
DWORD gNumMaterials = 0;


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
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	//��2�����ڴ����Ĳ���֮����ע�ᴰ����
	if (!RegisterClassEx(&wndClass))				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;

	//��3�����ڴ����Ĳ���֮������ʽ��������
	HWND hwnd = CreateWindowW(L"ForTheDreamOfGameDevelop", L"What the title!",				//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd, hInstance))
	{
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

bool Direct3DInit(HWND hwnd, HINSTANCE hInstance)
{
	//////////////////////////////////////////////////////////////////////////
	// Init D3D9  
	//////////////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////
	// Init DirectInput  
	//////////////////////////////////////////////////////////////////////////
	if (!gDirectInputClass.Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))
	{
		return false;
	}


	//////////////////////////////////////////////////////////////////////////
	// Init objects, which includes music, font, mesh, material, texture,
	// and other
	//////////////////////////////////////////////////////////////////////////	
	
	SAFE_RELEASE(pD3D9);
	if (!ObjectInit(hwnd, hInstance))
	{
		return false;
	}
	
	return true;

}

bool ObjectInit(HWND hwnd, HINSTANCE hInstance)
{
	PlaySound(_T("Kalafina - believe.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	srand(unsigned(time(nullptr)));
	if (FAILED(D3DXCreateFont(gPD3DDevice, 25, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"), &gPFont)))
	{
		return false;
	}

	LPD3DXBUFFER pAdjBuffer = nullptr;
	LPD3DXBUFFER pMaterialBuffer = nullptr;
	D3DXLoadMeshFromX(_T("Optimus.X"), D3DXMESH_MANAGED, gPD3DDevice, &pAdjBuffer, &pMaterialBuffer, nullptr, &gNumMaterials, &gPMesh);
	D3DXMATERIAL* pMaterial = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();
	gPMaterial = new D3DMATERIAL9[gNumMaterials];
	gPTexture = new LPDIRECT3DTEXTURE9[gNumMaterials];
	for (DWORD i = 0; i < gNumMaterials; i++)
	{
		gPMaterial[i] = pMaterial[i].MatD3D;
		gPMaterial[i].Ambient = gPMaterial[i].Diffuse;
		gPTexture[i] = NULL;
		D3DXCreateTextureFromFileA(gPD3DDevice, pMaterial[i].pTextureFilename, &gPTexture[i]);
	}
	SAFE_RELEASE(pAdjBuffer);
	SAFE_RELEASE(pMaterialBuffer);
	gPD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //������������
	gPD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //���û�����

	return true;
}
void MatrixSet()
{

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 0.0f, -2000.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	gPD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 6.0f, 1.0f, 1.0f, 4000.0f);
	gPD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 viewPort;
	viewPort.X = 0;
	viewPort.Y = 0;
	viewPort.Width = WINDOW_WIDTH;
	viewPort.Height = WINDOW_HEIGHT;
	viewPort.MinZ = 0.0f;
	viewPort.MaxZ = 1.0f;
	gPD3DDevice->SetViewport(&viewPort);

}
void Direct3DUpdate(HWND hwnd)
{
	gDirectInputClass.Update();

	static float translationX = 0;
	static float translationY = 0;
	static float translationZ = 0;

	if (gDirectInputClass.IsKeyDown(DIK_W))
	{
		translationY += 0.05f;
	}
	if (gDirectInputClass.IsKeyDown(DIK_S))
	{
		translationY -= 0.05f;
	}
	if (gDirectInputClass.IsKeyDown(DIK_A))
	{
		translationX -= 0.05f;
	}
	if (gDirectInputClass.IsKeyDown(DIK_D))
	{
		translationX += 0.05f;
	}

	static float angleX = 0.f;
	static float angleY = 0.f;
	static float angleZ = 0.f;
	if (gDirectInputClass.IsKeyDown(DIK_UP))
	{
		angleX += 0.005f;
	}
	if (gDirectInputClass.IsKeyDown(DIK_DOWN))
	{
		angleZ += 0.005f;
	}
	if (gDirectInputClass.IsKeyDown(DIK_LEFT))
	{
		angleY += 0.005f;
	}
	if (gDirectInputClass.IsKeyDown(DIK_RIGHT))
	{
		angleY -= 0.005f;
	}

	D3DXMATRIX matRx;
	D3DXMATRIX matRy;
	D3DXMATRIX matRz;
	D3DXMatrixRotationX(&matRx, angleX);
	D3DXMatrixRotationY(&matRy, angleY);
	D3DXMatrixRotationZ(&matRz, angleZ);
	
	static D3DXMATRIX matWorld;		
	D3DXMatrixTranslation(&matWorld, translationX, translationY, translationZ);
	matWorld = matRx * matRy * matRz * matWorld;
	gPD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	MatrixSet();
}

void Direct3DRender(HWND hwnd)
{

	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(25, 144, 23), 1.0f, 0);

	gPD3DDevice->BeginScene();

	Direct3DUpdate(hwnd);

	for (DWORD i = 0; i < gNumMaterials; i++)
	{
		gPD3DDevice->SetMaterial(&gPMaterial[i]);
		gPD3DDevice->SetTexture(0, gPTexture[i]);		
		gPMesh->DrawSubset(i);
	}
	
	gPFont->DrawTextW(nullptr, _T("FPS:�ٵ�"), -1, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(24, 235, 122));
	formatRect.left = 0, formatRect.top = 380;
	gPFont->DrawText(NULL, L"����˵��:", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(235, 123, 230, 255));
	formatRect.top += 35;
	gPFont->DrawText(NULL, L"    ��ס���������϶���ƽ��ģ��", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPFont->DrawText(NULL, L"    ��ס����Ҽ����϶�����תģ��", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPFont->DrawText(NULL, L"    ���������֣�����ģ��", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPFont->DrawText(NULL, L"    W��S��A��D����ƽ��ģ�� ", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPFont->DrawText(NULL, L"    �ϡ��¡����ҷ��������תģ�� ", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPFont->DrawText(NULL, L"    ESC�� : �˳�����", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));

	gPD3DDevice->EndScene();
	gPD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

bool Direct3DClearup(HWND hwnd)
{
	SAFE_DELETE(gPMaterial);
	SAFE_DELETE(gPTexture);
	SAFE_RELEASE(gPMesh);
	SAFE_RELEASE(gPFont);
	SAFE_RELEASE(gPD3DDevice);
	return true;
}

