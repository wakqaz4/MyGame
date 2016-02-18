/************************************************************************
* Implements terrain and a human like camera that could walk on terrain, 
* and also a observer camera that could move randomly.
* // [2016/1/21 21:17 Zhaoyu.Wang]  
************************************************************************/
#include "tchar.h"
#include "../../win32Utility/win32Utility/win32Utility.h"
#include "../../dInputUtility/dInputUtility/Keyboard.h"
#include "../../dInputUtility/dInputUtility/Mouse.h"
#include "../../../GDCamera/GDCamera.h"
#include "../../../GDObserver/GDObserver.h"
#include "Terrain.h"

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
LPD3DXMESH gPMesh = nullptr;

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
	CTerrain* m_pTerrain;
};
SystemGlobalEnvironment gEnv{ 0 };

//////////////////////////////////////////////////////////////////////////
// Declare other global variables not in GD Engine's framework, used in this
// project only.
//////////////////////////////////////////////////////////////////////////
LPD3DXFONT gPFont = nullptr;

//////////////////////////////////////////////////////////////////////////
// Game Starts here.  
//////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	/**
	 *	Fixed pipeLine 1: Create & init window.
	 */
	gEnv.m_pWindow = new CWindow(hInstance, WndProc);
	gEnv.m_pWindow->Init(WndProc, _T("GD Engine: Terrain"), 1200, 800);

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
	delete gEnv.m_pTerrain;

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
	gEnv.m_pTerrain = new CTerrain(gEnv.m_pDXDevice, L"coastMountain64.raw", 64, 64, 10, 0.5f);
	gEnv.m_pTerrain->GenTexture(Vector3(0.0f, 1.0f, 0.0f));

	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//Create font and play sound to make sure the program operates normally.
	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}
	PlaySound(_T("岩田さゆり - Thank You For Everything.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);

	/**
	 *	Create objects in scenario.
	 */
	D3DXCreateTeapot(gEnv.m_pDXDevice->GetD3DDevice(), &gPMesh, 0);

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
	Matrix I;
	I.Identity();
	gEnv.m_pTerrain->Draw(I, false);
	/**
	 *	Draw objects in scene here.
	 */	
	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = D3DCOLOR_WHITE;
	mtrl.Diffuse = D3DCOLOR_WHITE;
	mtrl.Emissive = D3DCOLOR_RED;
	mtrl.Specular = D3DCOLOR_PUREGREEN;
	mtrl.Power = 2.0f;
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&mtrl);	
	gPMesh->DrawSubset(0);
}

void Direct3DRender(float deltaTime)
{
	gEnv.m_pDXDevice->RenderDirectly(RenderFunc);
}

void RunGame(float deltatTime)
{
	Vector3 playerPos = gEnv.m_pObserver->m_pCamera->GetPosition();
	f32 terrainHeight = gEnv.m_pTerrain->GetHeight(playerPos.x, playerPos.z);
	playerPos.y = terrainHeight + 2.0f;
	gEnv.m_pObserver->m_pCamera->SetPosition(playerPos);

	gEnv.m_pMouse->Update(true);
	gEnv.m_pKeyboard->Update(true);
	gEnv.m_pObserver->Update();

}
