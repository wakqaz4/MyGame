/************************************************************************
* Implements a particle effect of snow.                                                                  
************************************************************************/
#include "../../win32Utility/win32Utility/win32Utility.h"
#include "../../dInputUtility/dInputUtility/Keyboard.h"
#include "../../dInputUtility/dInputUtility/Mouse.h"
#include "../../d3dUtility/d3dUtility/DXDevice9.h"
#include "../../../GDCamera/GDCamera.h"
#include "../../../GDObserver/GDObserver.h"
#include "../../../GDTypedef/GDTypedef.h"
#include "../../../GDMath/GDMath.h"
#include "SnowParticle.h"
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
LPD3DXMESH gPMesh = nullptr;
extern bool DrawBasicScene(IDirect3DDevice9* device, float scale);

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
	CParticleSystem* m_snowParticle;
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
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//Create font and play sound to make sure the program operates normally.
	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}
	PlaySound(_T("TRUSTRICK - FLYING FAFNIR.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);

	/**
	*	Create objects in scenario.
	*/
	D3DXCreateTeapot(gEnv.m_pDXDevice->GetD3DDevice(), &gPMesh, 0);

	/**
	 *	Create particle effects.
	 */
	gEnv.m_snowParticle = new CSnowParticle(Vector3(-5.0f, -2.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f), 6000);
	gEnv.m_snowParticle->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	gEnv.m_snowParticle->Init(gEnv.m_pDXDevice, L"snowflake.dds");
	
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
	mtrl.Emissive = D3DCOLOR_RED;
	mtrl.Specular = D3DCOLOR_PUREGREEN;
	mtrl.Power = 2.0f;
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&mtrl);
	gPMesh->DrawSubset(0);

	DrawBasicScene(gEnv.m_pDXDevice->GetD3DDevice(), 1.0f);

	D3DXMatrixIdentity(&W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	gEnv.m_snowParticle->Render();
}

void Direct3DRender(float deltaTime)
{
	gEnv.m_pDXDevice->RenderDirectly(RenderFunc);
}

void RunGame(float deltatTime)
{
	gEnv.m_pMouse->Update(true);
	gEnv.m_pKeyboard->Update(true);
	gEnv.m_pObserver->Update();
	gEnv.m_snowParticle->Update(deltatTime);
}


bool DrawBasicScene(IDirect3DDevice9* device, float scale)
{
	static IDirect3DVertexBuffer9* floor = 0;
	static IDirect3DTexture9*      tex = 0;
	static ID3DXMesh*              pillar = 0;

	HRESULT hr = 0;

	if (device == 0)
	{
		if (floor && tex && pillar)
		{
			// they already exist, destroy them
			SAFE_RELEASE(floor);
			SAFE_RELEASE(tex);
			SAFE_RELEASE(pillar);
		}
	}
	else if (!floor && !tex && !pillar)
	{
		// they don't exist, create them
		device->CreateVertexBuffer(
			6 * sizeof(Vertex),
			0,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&floor,
			0);

		Vertex* v = 0;
		floor->Lock(0, 0, (void**)&v, 0);

		v[0] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex(20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

		v[3] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[4] = Vertex(20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		v[5] = Vertex(20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

		floor->Unlock();

		D3DXCreateCylinder(device, 0.5f, 0.5f, 5.0f, 20, 20, &pillar, 0);

		D3DXCreateTextureFromFile(
			device,
			L"desert.bmp",
			&tex);
	}
	else
	{
		//
		// Pre-Render Setup
		//
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		D3DXVECTOR3 dir(0.707f, -0.707f, 0.707f);
		D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
		D3DLIGHT9 light;
		light.Ambient = col;
		light.Diffuse = col;
		light.Direction = dir;

		device->SetLight(0, &light);
		device->LightEnable(0, true);
		device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		device->SetRenderState(D3DRS_SPECULARENABLE, true);

		//
		// Render
		//

		D3DXMATRIX T, R, P, S;

		D3DXMatrixScaling(&S, scale, scale, scale);

		// used to rotate cylinders to be parallel with world's y-axis
		D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

		D3DMATERIAL9 mtrl;
		mtrl.Ambient = D3DCOLOR_YELLOW;
		mtrl.Diffuse = D3DCOLOR_YELLOW;
		mtrl.Emissive = D3DCOLOR_YELLOW;
		mtrl.Specular = D3DCOLOR_YELLOW;
		mtrl.Power = 2.0f;
		// draw floor
		D3DXMatrixIdentity(&T);
		T = T * S;
		device->SetTransform(D3DTS_WORLD, &T);
		device->SetMaterial(&mtrl);
		device->SetTexture(0, tex);
		device->SetStreamSource(0, floor, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// draw pillars
		mtrl.Ambient = D3DCOLOR_BLUE;
		mtrl.Diffuse = D3DCOLOR_BLUE;
		mtrl.Emissive = D3DCOLOR_BLUE;
		mtrl.Specular = D3DCOLOR_BLUE;
		mtrl.Power = 2.0f;
		device->SetMaterial(&mtrl);
		device->SetTexture(0, 0);
		for (int i = 0; i < 5; i++)
		{
			D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);

			D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);
		}
	}
	return true;
}