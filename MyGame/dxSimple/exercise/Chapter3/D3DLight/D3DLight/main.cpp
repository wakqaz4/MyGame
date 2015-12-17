/************************************************************************
* This is a program which uses D3D meshes, material and light, where material
* includes ambient , diffuse , specular, emissive;
* Here D3D9 used Phong-lightening-model. Which is introduced detailedly
* in GameEngineArchitecture.
* //  [12/8/2015 Zhaoyu.Wang]
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
#define SAFE_RELEASE(p) {if (p){p->Release();} p = nullptr; }

//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables  
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern void Direct3DRender(HWND hwnd);
extern bool Direct3DClearup(HWND hwnd);
extern bool ObjectInit(HWND hwnd);
extern void MatrixSet();
extern void LightSet(LPDIRECT3DDEVICE9, UINT);

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
ID3DXFont* gPFont = nullptr;
LPD3DXMESH gPTeapot = nullptr;
LPD3DXMESH gPBox = nullptr;
LPD3DXMESH gPTorus = nullptr;
LPD3DXMESH gPSphere = nullptr;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };							//用WINDCLASSEX定义了一个窗口类
	wndClass.cbSize = sizeof(WNDCLASSEX);			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra = 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = _T("ClassName");
	wndClass.cbClsExtra = 0;

	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	HWND hwnd = CreateWindow(_T("ClassName"), _T("Found This!"), WS_OVERLAPPEDWINDOW,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
	{
		MessageBox(nullptr, _T("Direct3D Init Failed!"), _T("D3D ERROR!"), MB_OK);
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
	d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
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

	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0, 30), 1.0f, 0.0f);
	gPD3DDevice->BeginScene();

	MatrixSet();

	WCHAR str[50];
	int strCount = swprintf_s(str, _T("Mat, Mesh & Light!"));
	gPFont->DrawTextW(nullptr, str, strCount, &formatRect, DT_TOP| DT_RIGHT, D3DCOLOR_XRGB(25, 134, 111));

	D3DXMATRIX Ry;
	D3DXMatrixRotationY(&Ry, timeGetTime() / 1000.0f);
	D3DXMATRIX teaPotWorldTrans;
	D3DXMatrixTranslation(&teaPotWorldTrans, 2.5f, 2.5f, 2.5f);
	D3DXMATRIX Sa;
	D3DXMatrixScaling(&Sa, 2.0f, 2.0f, 2.0f);
	teaPotWorldTrans = teaPotWorldTrans * Ry * Sa;
	gPD3DDevice->SetTransform(D3DTS_WORLD, &teaPotWorldTrans);

	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.7f, 1.0f);
	material.Diffuse = D3DXCOLOR(0.4f, 0.6f, 0.6f, 1.0f);
	material.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	material.Emissive = D3DXCOLOR(0.3f, 0.0f, 0.1f, 1.0f);
	gPD3DDevice->SetMaterial(&material);

	gPTeapot->DrawSubset(0);


	D3DXMATRIX boxWorldTrans;
	D3DXMatrixTranslation(&boxWorldTrans, -5.0f, 5.0f, 5.0f);
	boxWorldTrans *= Ry;
	gPD3DDevice->SetTransform(D3DTS_WORLD, &boxWorldTrans);

	ZeroMemory(&material, sizeof(material));
	material.Ambient = D3DXCOLOR(0.3f, 0.1f, 0.5f, 1.0f);
	material.Diffuse = D3DXCOLOR(0.4f, 0.6f, 0.6f, 1.0f);
	material.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	material.Emissive = D3DXCOLOR(0.3f, 0.0f, 0.1f, 1.0f);
	gPD3DDevice->SetMaterial(&material);
	gPBox->DrawSubset(0);

	D3DXMATRIX torusWorldTrans;
	D3DXMatrixTranslation(&torusWorldTrans, 5.0f, -5.0f, 5.0f);
	torusWorldTrans *= Ry;
	gPD3DDevice->SetTransform(D3DTS_WORLD, &torusWorldTrans);

	ZeroMemory(&material, sizeof(material));
	material.Ambient = D3DXCOLOR(0.5f, 0.2f, 0.3f, 1.0f);
	material.Diffuse = D3DXCOLOR(0.6f, 0.2f, 0.4f, 1.0f);
	material.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	material.Emissive = D3DXCOLOR(0.3f, 0.0f, 0.1f, 1.0f);
	gPD3DDevice->SetMaterial(&material);
	gPTorus->DrawSubset(0);

	D3DXMATRIX sphereWorldTrans;
	D3DXMatrixTranslation(&sphereWorldTrans, -5.0f, -5.0f, 5.0f);
	sphereWorldTrans *= Ry;
	gPD3DDevice->SetTransform(D3DTS_WORLD, &sphereWorldTrans);

	ZeroMemory(&material, sizeof(material));
	material.Ambient = D3DXCOLOR(0.9f, 0.1f, 0.9f, 1.0f);
	material.Diffuse = D3DXCOLOR(0.3f, 0.6f, 0.8f, 1.0f);
	material.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	material.Emissive = D3DXCOLOR(0.9f, 0.4f, 0.7f, 1.0f);
	gPD3DDevice->SetMaterial(&material);
	gPSphere->DrawSubset(0);

	LightSet(gPD3DDevice, 1);

	gPD3DDevice->EndScene();
	gPD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

bool Direct3DClearup(HWND hwnd)
{
	SAFE_RELEASE(gPFont);	
	SAFE_RELEASE(gPD3DDevice);
	return true;
}
bool ObjectInit(HWND hwnd)
{
	srand(unsigned(time(nullptr)));
	PlaySound(_T("コミネリサ - Resuscitated Hope.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);

	if (FAILED(D3DXCreateFont(gPD3DDevice, 38, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}
	D3DXCreateTeapot(gPD3DDevice, &gPTeapot, 0);
	D3DXCreateBox(gPD3DDevice, 2, 2, 2, &gPBox, 0);
	D3DXCreateTorus(gPD3DDevice, 1.0f, 2.0f, 25, 25, &gPTorus, 0);
	D3DXCreateSphere(gPD3DDevice, 2.0f, 25, 25, &gPSphere, 0);


	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.7f, 1.0f);
	material.Diffuse = D3DXCOLOR(0.4f, 0.6f, 0.6f, 1.0f);
	material.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	material.Emissive = D3DXCOLOR(0.3f, 0.0f, 0.1f, 1.0f);
	gPD3DDevice->SetMaterial(&material);	

	return true;
}

void MatrixSet()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	gPD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 0.0f, -50.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.f, 1.f, 0.f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	gPD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 6.0f, 1.0f, 1.0f, 1000.0f);
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

VOID LightSet(LPDIRECT3DDEVICE9 pd3dDevice, UINT nType)
{
	//定义一个光照类型并初始化
	static D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	//一个switch，给3种光源选项
	switch (nType)
	{
	case 1:     //点光源
		light.Type = D3DLIGHT_POINT;
		light.Ambient = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Position = D3DXVECTOR3(0.0f, 200.0f, 0.0f);
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Range = 300.0f;
		break;
	case 2:     //平行光
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		break;
	case 3:     //聚光灯
		light.Type = D3DLIGHT_SPOT;
		light.Position = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
		light.Direction = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		light.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Range = 300.0f;
		light.Falloff = 0.1f;
		light.Phi = D3DX_PI / 3.0f;
		light.Theta = D3DX_PI / 6.0f;
		break;
	}

	pd3dDevice->SetLight(0, &light); //设置光源
	pd3dDevice->LightEnable(0, true);//启用光照
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(36, 36, 36));   //设置一下环境光
	 
}
