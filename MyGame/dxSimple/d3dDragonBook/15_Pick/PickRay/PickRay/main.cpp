/************************************************************************
* This is a program used to implement the pick techniques in chapter 15 of 
* dragon book.
* In fact, you need to know the ray projection, and that's all.
* // [2016/2/1 16:25 Zhaoyu.Wang]  
************************************************************************/

#include "../../../GDTypedef/GDTypedef.h"
#include "../../../GDMacro/GDMacro.h"
#include "../../../GDMath/GDMath.h"
#include "../../../GDObserver/GDObserver.h"
#include "../../../d3dUtility/d3dUtility/d3dUtility/DXDevice9.h"
#include "../../../win32Utility/win32Utility/win32Utility/win32Utility.h"
#include "../../../dInputUtility/dInputUtility/dInputUtility/Mouse.h"
#include "../../../dInputUtility/dInputUtility/dInputUtility/Keyboard.h"
#include "tchar.h"

using namespace GD;

#pragma comment(lib, "win32Utility.lib")
#pragma comment(lib, "dInputUtility.lib")
#pragma comment(lib, "DXUT.lib")
#pragma comment(lib, "winmm.lib")
//////////////////////////////////////////////////////////////////////////
// Declare necessary external functions in GD Engine's framework.
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
bool Direct3DInit();
bool ObjectInit(HINSTANCE hInstance);
void Direct3DRender(float deltaTime);
void RunGame(float deltaTime);

//////////////////////////////////////////////////////////////////////////
// Declare unnecessary external functions that are not in GD Engine's 
// framework, used in this project only.
//////////////////////////////////////////////////////////////////////////
LPD3DXMESH gPTeapot = nullptr;
LPD3DXMESH gPCube = nullptr;

//////////////////////////////////////////////////////////////////////////
// Declare global system environment variable, necessary in GD Engine's 
// framework, which manages the whole game and editor.
//////////////////////////////////////////////////////////////////////////
struct SystemGlobalEnvironment
{
	CWindow* m_pWindow;
	CDXInput* m_pDInput;
	GD::CMouse* m_pMouse;
	CKeyboard* m_pKeyboard;
	CDXDevice9* m_pDXDevice;
	CCamera<float>* m_pCamera;
	CObserver* m_pObserver;
};
SystemGlobalEnvironment gEnv{ 0 };

//////////////////////////////////////////////////////////////////////////
// Declare other global variables not in GD Engine's framework, used in this
// project only.
//////////////////////////////////////////////////////////////////////////
LPD3DXFONT gPFont = nullptr;
struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x = x;  _y = y;  _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u;  _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;

	static const ::DWORD FVF;
};
const ::DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

//////////////////////////////////////////////////////////////////////////
// Game Starts here.  
//////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	/**
	*	Fixed pipeLine 1: Create & init window.
	*/
	gEnv.m_pWindow = new CWindow(hInstance, WndProc);
	gEnv.m_pWindow->Init(WndProc, _T("GD Engine: FireParticle"), 1200, 800);

	/**
	*	Fixed pipeLine 2: Init Direct3D
	*/
	if (!Direct3DInit())
	{
		::MessageBox(nullptr, _T("Direct3DInit()-Failed"), _T("Error"), MB_OK);
		return -1;
	}

	/**
	*	Fixed pipeLine 3: Init Object
	*/
	if (!ObjectInit(hInstance))
	{
		::MessageBox(nullptr, _T("ObjectInit()-Failed"), _T("Error"), MB_OK);
		return -1;
	}

	/**
	*	Fixed pipeLine 4: Enter main loop/update
	*/
	gEnv.m_pWindow->Update(Direct3DRender, RunGame);

	/**
	*	Fixed pipeLine 5: Release & delete
	*/
	//delete & new will be replace with safer delete/new methods in the future.
	delete gEnv.m_pDInput;
	delete gEnv.m_pKeyboard;
	delete gEnv.m_pMouse;
	delete gEnv.m_pWindow;
	delete gEnv.m_pCamera;
	delete gEnv.m_pObserver;

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	{
		POINT pos{ 0 };
		::GetCursorPos(&pos);
		f32 sx = (f32)(LOWORD(lParam));
		f32 sy = (f32)(HIWORD(lParam));

		f32 px = 0.0f;
		f32 py = 0.0f;

		D3DVIEWPORT9 viewPort;
		gEnv.m_pDXDevice->GetD3DDevice()->GetViewport(&viewPort);

		D3DXMATRIX proj;
		gEnv.m_pDXDevice->GetD3DDevice()->GetTransform(D3DTS_PROJECTION, &proj);

		px = ((2.0f*sx / viewPort.Width) - 1.0f) / proj(0, 0);
		py = ((-2.0f*sy / viewPort.Height) + 1.0f) / proj(1, 1);

		Vector3 origin(0.0f, 0.0f, 0.0f);
		Vector3 direction(px, py, 1.0f);

		D3DXMATRIX view;
		gEnv.m_pDXDevice->GetD3DDevice()->GetTransform(D3DTS_VIEW, &view);
		D3DXMATRIX viewInv;
		D3DXMatrixInverse(&viewInv, 0, &view);

		D3DXVec3TransformCoord(
			(D3DXVECTOR3*)(&origin),
			(D3DXVECTOR3*)(&origin),
			&viewInv);
		D3DXVec3TransformCoord(
			(D3DXVECTOR3*)(&direction),
			(D3DXVECTOR3*)(&direction),
			&viewInv);
		direction.Normalize();

		Vector3 teapotCenter(0.0f, 0.0f, 50.0f);
		f32 teapotRadius = 20.0f;

		Vector3 v = origin - teapotCenter;
		f32 b = 2.0f*(direction*v);
		f32 c = v*v - teapotRadius*teapotRadius;

		f32 discriminant = b*b - (4.0f*c);
		if (discriminant < 0.0f)
		{
			return 0;
		}
		discriminant = sqrtf(discriminant);
		float s0 = (discriminant - b) / 2.0f;
		float s1 = (-discriminant - b) / 2.0f;
		if (s0 >= 0.0f || s1 >= 0.0f)
		{
			::MessageBoxA(nullptr, "Hit!", "Hit teapot", MB_OK);
		}
	}
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
	*	Fixed object init step 1: Create and init dInput.
	*/
	gEnv.m_pDInput = new GD::CDXInput(gEnv.m_pWindow->GetHwnd());
	gEnv.m_pDInput->Init(hInstance);
	gEnv.m_pMouse = new GD::CMouse();
	gEnv.m_pKeyboard = new GD::CKeyboard();
	/**
	*	Fixed object init step 2: Create and init camera.
	*/
	gEnv.m_pCamera = new GD::CCamera<float>();
	gEnv.m_pCamera->LookAtLH(GD::CVector3<float>(0.0f, 0.0f, -50.0f), GD::CVector3<float>(0.0f, 0.0f, 0.0f), GD::CVector3<float>(0.0f, 1.0f, 0.0f));
	/**
	*	Fixed object init step 3: Create and init observer.
	*/
	gEnv.m_pObserver = new CObserver();
	gEnv.m_pObserver->m_pCamera = gEnv.m_pCamera;
	gEnv.m_pObserver->m_pKeyboard = gEnv.m_pKeyboard;
	gEnv.m_pObserver->m_pMouse = gEnv.m_pMouse;
	/**
	*	Fixed object init step 4: Create and init terrain.
	*/
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//Create font and play sound to make sure the program operates normally.
	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}
	PlaySound(_T("moumoon - Hello,shooting-star.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);

	/**
	*	Create objects in scenario.
	*/
	D3DXCreateTeapot(gEnv.m_pDXDevice->GetD3DDevice(), &gPTeapot, 0);

	/**
	*	Create particle effects.
	*/

	return true;
}

//Add your objects here.
void CALLBACK RenderFunc(void)
{
	/**
	*	Show GPU info on TOP|LEFT .
	*/
	WCHAR TempName[60] = L"GPU Info:";
	WCHAR adapterName[30]{0};
	D3DADAPTER_IDENTIFIER9 Adapter;
	LPDIRECT3D9  pD3D = NULL;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return;
	}
	pD3D->GetAdapterIdentifier(0, 0, &Adapter);
	int len = ::MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);
	::MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, adapterName, len);
	wcscat_s(TempName, adapterName);
	pD3D->Release();

	gPFont->DrawText(nullptr, TempName, -1, nullptr, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(124, 24, 222));

	/**
	*	Set camera and view here.
	*/
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
	/**
	*	Draw terrain here.
	*/

	/**
	*	Draw objects in scene here.
	*/
	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = D3DCOLOR_WHITE;
	mtrl.Diffuse = D3DCOLOR_WHITE;
	mtrl.Emissive = D3DCOLOR_LIGHTGREEN;
	mtrl.Specular = D3DCOLOR_PUREGREEN;
	mtrl.Power = 2.0f;
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&mtrl);
	gPTeapot->DrawSubset(0);

}

void Direct3DRender(float deltaTime)
{
	gEnv.m_pDXDevice->RenderDirectly(RenderFunc);
}



//Pick items
void PickItems()
{


}

void RunGame(float deltatTime)
{
	gEnv.m_pMouse->Update(true);
	gEnv.m_pKeyboard->Update(true);
	gEnv.m_pObserver->Update();
	PickItems();
}