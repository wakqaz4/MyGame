/************************************************************************
* This program intends to create a tool that could visually edit vertices
* and indices;
************************************************************************/
#include "tchar.h"
#include "../../../win32Utility/win32Utility/win32Utility/win32Utility.h"
#include "../../../dInputUtility/dInputUtility/dInputUtility/KeyBoard.h"
#include "../../../dInputUtility/dInputUtility/dInputUtility/Mouse.h"
#include "../../../d3dUtility/d3dUtility/d3dUtility/DXDevice9.h"
#include "../../../GDCamera/GDCamera.h"
#include "../../../GDMath/GDMath.h"


#pragma comment(lib, "win32Utility.lib")
#pragma comment(lib, "dInputUtility.lib")
#pragma comment(lib, "DXUT.lib")
#pragma comment(lib, "winmm.lib")


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
bool Direct3DInit(HWND hwnd);
bool ObjectInit(HWND hwnd, HINSTANCE hInstance);
void Direct3DRender(float deltaTime);
void RunGame(float deltaTime);

LPD3DXFONT gPFont = nullptr;
LPD3DXMESH gPTeapot = nullptr;

struct gPInput
{
	GD::CWindow* m_pWindow;
	GD::CDXInput* m_pDInput;
	GD::CMouse* m_pMouse;
	GD::CKeyBoard* m_pKeyboard;
	GD::CDXDevice9* m_pDXDevice;
	GD::CCamera<float>*	m_pCamera;
};
gPInput gEnv{0};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	gEnv.m_pWindow = new GD::CWindow(hInstance, WndProc);
	gEnv.m_pWindow->Init(WndProc);

	/**
	 *	Insert d3d init here
	 *	*/
	if (!Direct3DInit(gEnv.m_pWindow->GetHwnd()))
	{
		::MessageBox(nullptr, _T("Direct3DInit()-Failed"), _T("Error"), MB_OK);
		return -1;
	}

	/**
	 *	Insert object init here
	 *	*/
	if (!ObjectInit(gEnv.m_pWindow->GetHwnd(), hInstance))
	{
		::MessageBox(nullptr, _T("ObjectInit()-Failed"), _T("Error"), MB_OK);
		return -1;
	}

	gEnv.m_pWindow->Update(Direct3DRender, RunGame);

	delete gEnv.m_pDInput;
	delete gEnv.m_pKeyboard;
	delete gEnv.m_pMouse;
	delete gEnv.m_pWindow;
	delete gEnv.m_pCamera;

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

bool Direct3DInit(HWND hwnd)
{
	gEnv.m_pDXDevice = new GD::CDXDevice9(hwnd);
	gEnv.m_pCamera = new GD::CCamera<float>();
	gEnv.m_pCamera->LookAtLH(GD::CVector3<float>(0.0f, 0.0f, -50.0f), GD::CVector3<float>(0.0f, 0.0f, 0.0f), GD::CVector3<float>(0.0f, 1.0f, 0.0f));

	return true;
}

bool ObjectInit(HWND hwnd, HINSTANCE hInstance)
{
	PlaySound(_T("Kalafina - believe.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);
	
	gEnv.m_pDInput = new GD::CDXInput(hwnd);
	gEnv.m_pDInput->Init(hInstance);
	gEnv.m_pMouse = new GD::CMouse();
	gEnv.m_pKeyboard = new GD::CKeyBoard();

	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷书"), &gPFont)))
	{
		return false;
	}
	D3DXCreateTeapot(gEnv.m_pDXDevice->GetD3DDevice(), &gPTeapot, nullptr);

	return true;
}

void CALLBACK renderFunc()
{	
	D3DMATRIX* pMatView = nullptr;
	GD::CMatrix44<float> viewMat(gEnv.m_pCamera->GetViewMatrixDX());
	pMatView = (D3DMATRIX*)(&viewMat);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_VIEW, pMatView); //应用取景变换矩阵

	D3DMATRIX* pProjMatrix;
	pProjMatrix = (D3DMATRIX*)(&gEnv.m_pCamera->GetProjMatrix().GetFliped());
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, pProjMatrix);  //设置投影变换矩阵

	D3DVIEWPORT9* pViewPort; 
	pViewPort = (D3DVIEWPORT9*)(&gEnv.m_pCamera->GetViewPort());
	gEnv.m_pDXDevice->GetD3DDevice()->SetViewport(pViewPort); //视口的设置

	//D3DXMATRIX matView; //定义一个矩阵
	//D3DXVECTOR3 vEye(0.0f, 0.0f, -50.0f);  //摄像机的位置
	//D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
	//D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
	//D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
	//gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵

	//D3DXMATRIX matProj; //定义一个矩阵
	//D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)((double)800 / 600), 1.0f, 1000.0f); //计算投影变换矩阵
	//gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &matProj);  //设置投影变换矩阵

	//D3DVIEWPORT9 vp; //实例化一个D3DVIEWPORT9结构体，然后做填空题给各个参数赋值就可以了
	//vp.X = 0;		//表示视口相对于窗口的X坐标
	//vp.Y = 0;		//视口相对对窗口的Y坐标
	//vp.Width = 800;	//视口的宽度
	//vp.Height = 600; //视口的高度
	//vp.MinZ = 0.0f; //视口在深度缓存中的最小深度值
	//vp.MaxZ = 1.0f;	//视口在深度缓存中的最大深度值
	//gEnv.m_pDXDevice->GetD3DDevice()->SetViewport(&vp); //视口的设置


	WCHAR TempName[60] = L"当前显卡型号：";   //定义一个临时字符串，且方便了把"当前显卡型号："字符串引入我们的目的字符串中
	WCHAR adapterName[30]{0};
	D3DADAPTER_IDENTIFIER9 Adapter;  //定义一个D3DADAPTER_IDENTIFIER9结构体，用于存储显卡信息
	LPDIRECT3D9  pD3D = NULL; //Direct3D接口对象的创建
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION))) //初始化Direct3D接口对象，并进行DirectX版本协商
	{
		return;
	}
	pD3D->GetAdapterIdentifier(0, 0, &Adapter);//调用GetAdapterIdentifier，获取显卡信息
	int len = ::MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);//显卡名称现在已经在Adapter.Description中了，但是其为char类型，我们要将其转为wchar_t类型
	::MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, adapterName, len);//这步操作完成后，g_strAdapterName中就为当前我们的显卡类型名的wchar_t型字符串了
	wcscat_s(TempName, adapterName);//把当前我们的显卡名加到“当前显卡型号：”字符串后面，结果存在TempName中	
	pD3D->Release();

	gPFont->DrawText(nullptr, TempName, -1, nullptr, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(124, 24, 222));
	
	D3DMATERIAL9 material;
	material.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	material.Diffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	material.Emissive = D3DXCOLOR(0.4f, 0.1f, 0.7f, 1.0f);
	material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Power = 5.0f;
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&material);

	D3DLIGHT9 light;
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	light.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	light.Diffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gEnv.m_pDXDevice->GetD3DDevice()->SetLight(0, &light);

	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, true);

	gPTeapot->DrawSubset(0);
}

void Direct3DRender(float deltaTime)
{
	gEnv.m_pDXDevice->RenderDirectly(renderFunc);
}

//Update camera's position and rotation, with keyboard & mouse message
void OnCameraUpdate()
{
	float dx = gEnv.m_pMouse->GetDeltaX();
	float dy = gEnv.m_pMouse->GetDeltaY();
	float dz = gEnv.m_pMouse->GetDeltaZ();
	gEnv.m_pCamera->RotateAroundZ(dz/20);
	gEnv.m_pCamera->RotateAroundX(-dy/20);
	gEnv.m_pCamera->RotateAroundY(-dx/20);

	float delta = 0.0001f;
	if (gEnv.m_pKeyboard->IsKeyDown(DIK_W))
	{		
		gEnv.m_pCamera->TranslateForward(delta);
	}
	if (gEnv.m_pKeyboard->IsKeyDown(DIK_S))
	{
		gEnv.m_pCamera->TranslateForward(-delta);
	}
	if (gEnv.m_pKeyboard->IsKeyDown(DIK_A))
	{
		gEnv.m_pCamera->TranslateRightward(-delta);
	}
	if (gEnv.m_pKeyboard->IsKeyDown(DIK_D))
	{
		gEnv.m_pCamera->TranslateRightward(delta);
	}
}

void RunGame(float deltaTime)
{
	gEnv.m_pMouse->Update(true);
	gEnv.m_pKeyboard->Update(true);

	/**
	 *	Debug mouse and keyBoard device
	 *	*/
	/*
	if (gEnv.m_pKeyboard->IsKeyDown(DIK_W))
	{
		::MessageBoxA(gEnv.m_pDInput->GetHWND(), "The left button is pressed!", "", MB_OK);
	}
	if (gEnv.m_pMouse->IsButtonDown(1))
	{
		::MessageBoxA(nullptr, "The left button is pressed!", "", MB_OK);
	}*/
	OnCameraUpdate();

}