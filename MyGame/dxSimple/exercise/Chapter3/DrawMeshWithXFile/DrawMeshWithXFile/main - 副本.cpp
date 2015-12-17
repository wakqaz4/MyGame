/************************************************************************
* This project draws a Lolita with prepared .X file.
* .X file is a format supported by DX, which includes much information 
* such as mesh, texture, material, animation, and so on;
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
#define SAFE_RELEASE(p) {if(p){p->Release(); p = nullptr;}} 

//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables  
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd);
extern void Direct3DRender(HWND hwnd);
extern bool Direct3DClearup(HWND hwnd);
extern bool ObjectInit(HWND hwnd);
extern void MatrixSet();
extern void LightSet();

LPDIRECT3DDEVICE9 gPD3DDevice = nullptr;
LPD3DXFONT gPTextFPSFont = nullptr;
LPD3DXFONT gPTextAdapterFont = nullptr;
LPD3DXFONT gPTextHelperFont = nullptr;
LPD3DXFONT gPTextInfoFont = nullptr;
D3DXMATRIX gMatWorld;
WCHAR gStrFPS[50]{0};
WCHAR gStrAdapterDesc[50]{0};
LPD3DXMESH gPCharacter = nullptr;
D3DMATERIAL9* gPMaterial = nullptr;
DWORD gDwNumMtrl = 0;
LPDIRECT3DTEXTURE9* gPTexture = nullptr;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//【1】窗口创建四步曲之一：开始设计一个完整的窗口类
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
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//用一个以空终止的字符串，指定窗口类的名字。

	//【2】窗口创建四步曲之二：注册窗口类
	if (!RegisterClassEx(&wndClass))				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;

	//【3】窗口创建四步曲之三：正式创建窗口
	HWND hwnd = CreateWindowW(L"ForTheDreamOfGameDevelop", L"What the title!",				//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Direct3DInit(hwnd))
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

	D3DADAPTER_IDENTIFIER9 ident;
	pD3D9->GetAdapterIdentifier(0, 0, &ident);
	int len = MultiByteToWideChar(CP_ACP, 0, ident.Description, -1, 0, 0);
	MultiByteToWideChar(CP_ACP, 0, ident.Description, -1, gStrAdapterDesc, len);

	if (!ObjectInit(hwnd))
	{
		return false;
	}
	SAFE_RELEASE(pD3D9);

	return true;
}

void Direct3DRender(HWND hwnd)
{
	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	gPD3DDevice->BeginScene();
	
	MatrixSet();
	D3DXMatrixIdentity(&gMatWorld);
	gPD3DDevice->SetTransform(D3DTS_WORLD, &gMatWorld);//设置世界矩阵

	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	for (DWORD i = 0; i < gDwNumMtrl; i++)
	{
		gPD3DDevice->SetMaterial(&gPMaterial[i]);
		gPD3DDevice->SetTexture(0, gPTexture[i]);
		gPCharacter->DrawSubset(i);
	}

	int strLen = swprintf_s(gStrFPS, _T("FPS: %f"), 123.45f);
	gPTextFPSFont->DrawTextW(nullptr, gStrFPS, strLen, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(0, 239, 136));	
	strLen = sizeof(gStrAdapterDesc);
	gPTextAdapterFont->DrawTextW(nullptr, gStrAdapterDesc, -1, &formatRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(23, 23, 236));

	formatRect.top = 30;
	static wchar_t strInfo[256] = { 0 };
	swprintf_s(strInfo, -1, L"模型坐标: (%.2f, %.2f, %.2f)", gMatWorld._41, gMatWorld._42, gMatWorld._43);
	gPTextHelperFont->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));

	formatRect.top = WINDOW_HEIGHT * 2 / 3;
	gPTextInfoFont->DrawTextW(nullptr, _T("控制说明："), -1, &formatRect, DT_NOCLIP|DT_LEFT | DT_SINGLELINE, D3DCOLOR_XRGB(23, 25, 111));
	formatRect.top += 35;
	formatRect.left += 50;
	gPTextHelperFont->DrawText(NULL, L"按住鼠标左键并拖动：平移模型", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"按住鼠标右键并拖动：旋转模型", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"滑动鼠标滚轮：拉伸模型", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"W、S、A、D键：平移模型 ", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"上、下、左、右方向键：旋转模型 ", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"ESC键 : 退出程序", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));

	gPD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	gPD3DDevice->EndScene();
	gPD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

}

bool Direct3DClearup(HWND hwnd)
{
	SAFE_RELEASE(gPTextAdapterFont);
	SAFE_RELEASE(gPTextFPSFont);
	SAFE_RELEASE(gPTextHelperFont);
	SAFE_RELEASE(gPTextInfoFont);
	SAFE_RELEASE(gPTextAdapterFont);
	SAFE_RELEASE(gPD3DDevice);
	return true;
}

bool ObjectInit(HWND hwnd)
{
	srand(unsigned(time(nullptr)));
	PlaySound(_T("コミネリサ - Resuscitated Hope.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	D3DXCreateFont(gPD3DDevice, 30, 0, 0, 0, 0, 0, 0, 0, 0, _T("微软雅黑"), &gPTextAdapterFont);
	D3DXCreateFont(gPD3DDevice, 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("华文中宋"), &gPTextHelperFont);
	D3DXCreateFont(gPD3DDevice, 30, 0, 0, 0, 0, 0, 0, 0, 0, _T("黑体"), &gPTextInfoFont);
	D3DXCreateFont(gPD3DDevice, 36, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPTextFPSFont);

	//////////////////////////////////////////////////////////////////////////
	// Load mesh and materials here  
	//////////////////////////////////////////////////////////////////////////
	LPD3DXBUFFER pAdjBuffer = nullptr;
	LPD3DXBUFFER pMtrlBuffer = nullptr;
	D3DXLoadMeshFromX(_T("loli.x"), D3DXMESH_MANAGED, gPD3DDevice, &pAdjBuffer, &pMtrlBuffer, nullptr, &gDwNumMtrl, &gPCharacter);

	D3DXMATERIAL* pMtrl = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	gPMaterial = new D3DMATERIAL9[gDwNumMtrl];
	gPTexture = new LPDIRECT3DTEXTURE9[gDwNumMtrl];

	for (DWORD i = 0; i < gDwNumMtrl; i++)
	{
		gPMaterial[i] = pMtrl[i].MatD3D;
		gPMaterial[i].Ambient = gPMaterial[i].Diffuse;
		gPTexture[i] = nullptr;
		D3DXCreateTextureFromFileA(gPD3DDevice, pMtrl[i].pTextureFilename, &gPTexture[i]);
	}

	SAFE_RELEASE(pAdjBuffer);
	SAFE_RELEASE(pMtrlBuffer);

	gPD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //开启背面消隐
	gPD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //设置环境光

	return true;
}

void MatrixSet()
{
	//--------------------------------------------------------------------------------------
	//【四大变换之一】：世界变换矩阵的设置
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	//【四大变换之二】：取景变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0.0f, -250.0f);  //摄像机的位置
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
	gPD3DDevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之三】：投影变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj; //定义一个矩阵
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)((double)WINDOW_WIDTH / WINDOW_HEIGHT), 1.0f, 1000.0f); //计算投影变换矩阵
	gPD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);  //设置投影变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之四】：视口变换的设置
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //实例化一个D3DVIEWPORT9结构体，然后做填空题给各个参数赋值就可以了
	vp.X = 0;		//表示视口相对于窗口的X坐标
	vp.Y = 0;		//视口相对对窗口的Y坐标
	vp.Width = WINDOW_WIDTH;	//视口的宽度
	vp.Height = WINDOW_HEIGHT; //视口的高度
	vp.MinZ = 0.0f; //视口在深度缓存中的最小深度值
	vp.MaxZ = 1.0f;	//视口在深度缓存中的最大深度值
	gPD3DDevice->SetViewport(&vp); //视口的设置

}