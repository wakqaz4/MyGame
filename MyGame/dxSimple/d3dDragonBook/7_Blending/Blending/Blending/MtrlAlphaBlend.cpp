/************************************************************************
* Blend material with alpha channel.
* D3D9 dragon book's example in chapter 7.
* // [2016/1/15 10:52 Zhaoyu.Wang]  
************************************************************************/
#include "tchar.h"
#include "../../../win32Utility/win32Utility/win32Utility/win32Utility.h"
#include "../../../dInputUtility/dInputUtility/dInputUtility/KeyBoard.h"
#include "../../../dInputUtility/dInputUtility/dInputUtility/Mouse.h"
#include "../../../d3dUtility/d3dUtility/d3dUtility/DXDevice9.h"
#include "../../../GDCamera/GDCamera.h"
#include "../../../GDMath/GDMath.h"
#include "../../../GDObserver/GDObserver.h"

using namespace GD;

#pragma comment(lib, "win32Utility.lib")
#pragma comment(lib, "dInputUtility.lib")
#pragma comment(lib, "DXUT.lib")
#pragma comment(lib, "winmm.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool Direct3DInit();
bool ObjectInit(HINSTANCE hInstance);
void Direct3DRender(float deltaTime);
void RunGame(float deltatTime);

//////////////////////////////////////////////////////////////////////////
// Put global variables here.  
//////////////////////////////////////////////////////////////////////////
LPD3DXFONT gPFont = nullptr;
LPD3DXMESH gPTeapot = nullptr;
LPDIRECT3DVERTEXBUFFER9 gPVertexBuffer = nullptr;
LPDIRECT3DTEXTURE9 gPBkTex = nullptr;
D3DMATERIAL9 BkMtrl;
D3DMATERIAL9 TeapotMtrl;

struct SystemGlobalEnvironment
{
	CWindow* m_pWindow;
	CDXInput* m_pDInput;
	GD::CMouse* m_pMouse;
	CKeyBoard* m_pKeyBoard;
	CDXDevice9* m_pDXDevice;
	CCamera<float>* m_pCamera;
	CObserver* m_pObserver;
};
SystemGlobalEnvironment gEnv{ 0 };

struct Vertex
{
	Vertex(){};
	Vertex(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x = x;  _y = y;  _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u;  _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v; // texture coordinates

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	gEnv.m_pWindow = new CWindow(hInstance, WndProc);
	gEnv.m_pWindow->Init(WndProc, _T("Material Blending"), 1200, 800);

	/**
	*	Insert d3d init here
	*	*/
	if (!Direct3DInit())
	{
		::MessageBox(nullptr, _T("Direct3DInit()-Failed"), _T("Error"), MB_OK);
		return -1;
	}

	/**
	*	Insert object init here, such as input/mouse/keyboard, camera, 
	*	meshes, font, vertices, material, light, texture, terrain;
	*	[2016/8/6 ]
	*	Actually, acrodding to render pipeLine, object inited here should mainly be
	*	vertices, as material, light, texture are related with renderState;
	*	*/
	if (!ObjectInit(hInstance))
	{
		::MessageBox(nullptr, _T("ObjectInit()-Failed"), _T("Error"), MB_OK);
		return -1;
	}

	gEnv.m_pWindow->Update(Direct3DRender, RunGame);

	delete gEnv.m_pDInput;
	delete gEnv.m_pKeyBoard;
	delete gEnv.m_pMouse;
	delete gEnv.m_pWindow;
	delete gEnv.m_pCamera;

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

bool Direct3DInit()
{
	return (nullptr != (gEnv.m_pDXDevice = new CDXDevice9(gEnv.m_pWindow->GetHwnd())));
}

bool ObjectInit(HINSTANCE hInstance)
{
	/**
	 *	No need to be so annoying, just make a CObserver class, and that's all.
	 *	*/
	//if (!CameraInit())
	//{
	//	return false;
	//}
	//AddToUpdate(OnCameraUpdate);
	//if (!MouseInit())
	//{
	//	return false;
	//}
	//AddToUpdate(OnMouseUpdate);
	//if (!KeyBoardInit())
	//{
	//	return false;
	//}
	//AddToUpdate(OnKeyBoardUpdate);
	//if (!AudioInit())
	//{
	//	return false;
	//}
	//AddToUpdate(OnAudioUpdate);	
	gEnv.m_pObserver = new CObserver();
	gEnv.m_pCamera = new GD::CCamera<float>();
	gEnv.m_pCamera->LookAtLH(GD::CVector3<float>(0.0f, 0.0f, -50.0f), GD::CVector3<float>(0.0f, 0.0f, 0.0f), GD::CVector3<float>(0.0f, 1.0f, 0.0f));
	gEnv.m_pDInput = new GD::CDXInput(gEnv.m_pWindow->GetHwnd());
	gEnv.m_pDInput->Init(hInstance);
	gEnv.m_pMouse = new GD::CMouse();
	gEnv.m_pKeyBoard = new GD::CKeyBoard();
	gEnv.m_pObserver->m_pCamera = gEnv.m_pCamera;
	gEnv.m_pObserver->m_pKeyBoard = gEnv.m_pKeyBoard;
	gEnv.m_pObserver->m_pMouse = gEnv.m_pMouse;

	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}
	PlaySound(_T("Kalafina - believe.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);

	D3DXCreateTeapot(gEnv.m_pDXDevice->GetD3DDevice(), &gPTeapot, 0);
	if (FAILED(gEnv.m_pDXDevice->GetD3DDevice()->CreateVertexBuffer(6 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&gPVertexBuffer,
		0)))
	{
		return false;
	}
	Vertex* v;
	gPVertexBuffer->Lock(0, 6 * sizeof(Vertex), (void**)&v, 0);
	v[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	gPVertexBuffer->Unlock();


	TeapotMtrl.Ambient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	TeapotMtrl.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	TeapotMtrl.Specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	TeapotMtrl.Power = 2.0f;
	TeapotMtrl.Diffuse.a = 0.5f; // set alpha to 50% opacity

	BkMtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	BkMtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	BkMtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	BkMtrl.Power = 2.0f;

	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	dir.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)* 0.2f;
	dir.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) * 0.6f;
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);

	gEnv.m_pDXDevice->GetD3DDevice()->SetLight(0, &dir);
	gEnv.m_pDXDevice->GetD3DDevice()->LightEnable(0, true);

	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_SPECULARENABLE, true);

	D3DXCreateTextureFromFile(
		gEnv.m_pDXDevice->GetD3DDevice(),
		L"crate.jpg",
		&gPBkTex);

	/**
	 *	Set filter mode
	 *	*/
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	/**
	 *	Set mipmap mode
	 *	*/
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	
	//
	// Set alpha blending states.
	//

	// use alpha in material's diffuse component for alpha
	//gEnv.m_pDXDevice->GetD3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	//gEnv.m_pDXDevice->GetD3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	//// set blending factors so that alpha component determines transparency
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);


	return true;
}

void CALLBACK RenderFunc(void)
{
	/**
	 *	Set camera
	 *	*/
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

	// increase/decrease alpha via keyboard input
	if (::GetAsyncKeyState('A') & 0x8000f)
		TeapotMtrl.Diffuse.a += 0.01f;
	if (::GetAsyncKeyState('S') & 0x8000f)
		TeapotMtrl.Diffuse.a -= 0.01f;

	// force alpha to [0, 1] interval
	if (TeapotMtrl.Diffuse.a > 1.0f)
		TeapotMtrl.Diffuse.a = 1.0f;
	if (TeapotMtrl.Diffuse.a < 0.0f)
		TeapotMtrl.Diffuse.a = 0.0f;

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetFVF(Vertex::FVF);
	gEnv.m_pDXDevice->GetD3DDevice()->SetStreamSource(0, gPVertexBuffer, 0, sizeof(Vertex));
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&BkMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, gPBkTex);
	gEnv.m_pDXDevice->GetD3DDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// Draw the teapot
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	D3DXMatrixScaling(&W, 1.5f, 1.5f, 1.5f);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&TeapotMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, 0);
	gPTeapot->DrawSubset(0);

	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

}

void Direct3DRender(float deltaTime)
{
	gEnv.m_pDXDevice->RenderDirectly(RenderFunc);
}

void RunGame(float deltatTime)
{
	gEnv.m_pMouse->Update(true);
	gEnv.m_pKeyBoard->Update(true);
	gEnv.m_pObserver->Update();
}