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
LPDIRECT3DTEXTURE9 gPFloorTex = nullptr;
LPDIRECT3DTEXTURE9 gPWallTex = nullptr;
LPDIRECT3DTEXTURE9 gPMirrorTex = nullptr;
D3DMATERIAL9 gFloorMtrl;
D3DMATERIAL9 gWallMtrl;
D3DMATERIAL9 gMirrorMtrl;
D3DMATERIAL9 gTeapotMtrl;

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

	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"), &gPFont)))
	{
		return false;
	}
	PlaySound(_T("Kalafina - believe.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);

	D3DXCreateTeapot(gEnv.m_pDXDevice->GetD3DDevice(), &gPTeapot, 0);

	if (FAILED(gEnv.m_pDXDevice->GetD3DDevice()->CreateVertexBuffer(24 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&gPVertexBuffer,
		0)))
	{
		return false;
	}
	Vertex* v;
	gPVertexBuffer->Lock(0, 24 * sizeof(Vertex), (void**)&v, 0);
	// floor
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// wall
	v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Note: We leave gap in middle of walls for mirror

	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// mirror
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	gPVertexBuffer->Unlock();


	gTeapotMtrl.Ambient = D3DXCOLOR(1.0f, 0.1f, 0.6f, 1.0f);
	gTeapotMtrl.Diffuse = D3DXCOLOR(0.3f, 0.6f, 0.0f, 1.0f);
	gTeapotMtrl.Specular = D3DXCOLOR(1.0f, 0.9f, 0.3f, 1.0f);
	gTeapotMtrl.Power = 2.0f;	

	gFloorMtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gFloorMtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gFloorMtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gFloorMtrl.Power = 2.0f;

	gWallMtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gWallMtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gWallMtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gWallMtrl.Power = 2.0f;

	gMirrorMtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gMirrorMtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gMirrorMtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gMirrorMtrl.Power = 2.0f;

	D3DXCreateTextureFromFile(gEnv.m_pDXDevice->GetD3DDevice(), _T("checker.jpg"), &gPFloorTex);
	D3DXCreateTextureFromFile(gEnv.m_pDXDevice->GetD3DDevice(), _T("brick0.jpg"), &gPWallTex);
	D3DXCreateTextureFromFile(gEnv.m_pDXDevice->GetD3DDevice(), _T("ice.bmp"), &gPMirrorTex);

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
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_VIEW, pMatView); //Ӧ��ȡ���任����

	D3DMATRIX* pProjMatrix;
	pProjMatrix = (D3DMATRIX*)(&gEnv.m_pCamera->GetProjMatrix().GetFliped());
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, pProjMatrix);  //����ͶӰ�任����

	D3DVIEWPORT9* pViewPort;
	pViewPort = (D3DVIEWPORT9*)(&gEnv.m_pCamera->GetViewPort());
	gEnv.m_pDXDevice->GetD3DDevice()->SetViewport(pViewPort); //�ӿڵ�����

	WCHAR TempName[60] = L"��ǰ�Կ��ͺţ�";   //����һ����ʱ�ַ������ҷ����˰�"��ǰ�Կ��ͺţ�"�ַ����������ǵ�Ŀ���ַ�����
	WCHAR adapterName[30]{0};
	D3DADAPTER_IDENTIFIER9 Adapter;  //����һ��D3DADAPTER_IDENTIFIER9�ṹ�壬���ڴ洢�Կ���Ϣ
	LPDIRECT3D9  pD3D = NULL; //Direct3D�ӿڶ���Ĵ���
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION))) //��ʼ��Direct3D�ӿڶ��󣬲�����DirectX�汾Э��
	{
		return;
	}
	pD3D->GetAdapterIdentifier(0, 0, &Adapter);//����GetAdapterIdentifier����ȡ�Կ���Ϣ
	int len = ::MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);//�Կ����������Ѿ���Adapter.Description���ˣ�������Ϊchar���ͣ�����Ҫ����תΪwchar_t����
	::MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, adapterName, len);//�ⲽ������ɺ�g_strAdapterName�о�Ϊ��ǰ���ǵ��Կ���������wchar_t���ַ�����
	wcscat_s(TempName, adapterName);//�ѵ�ǰ���ǵ��Կ����ӵ�����ǰ�Կ��ͺţ����ַ������棬�������TempName��	
	pD3D->Release();

	gPFont->DrawText(nullptr, TempName, -1, nullptr, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(124, 24, 222));

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetFVF(Vertex::FVF);
	gEnv.m_pDXDevice->GetD3DDevice()->SetStreamSource(0, gPVertexBuffer, 0, sizeof(Vertex));

	/**
	 *	Draw floor
	 *	*/
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gFloorMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, gPFloorTex);
	gEnv.m_pDXDevice->GetD3DDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	/**
	 *	Draw wall
	 *	*/
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gWallMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, gPWallTex);
	gEnv.m_pDXDevice->GetD3DDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);
	/**
	 *	Draw mirror
	 *	*/
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gMirrorMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, gPMirrorTex);
	gEnv.m_pDXDevice->GetD3DDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
	
	// Set teapot position
	static D3DXMATRIX T;
	static float x = 0.0f;
	static float y = 3.0f;
	static float z = -7.5f;	

	const float delta = 0.01f;
	// increase/decrease alpha via keyboard input
	if (gEnv.m_pKeyBoard->IsKeyDown(DIK_UPARROW))
	{
		y += delta;
	}
	if (gEnv.m_pKeyBoard->IsKeyDown(DIK_DOWNARROW))
	{
		y -= delta;
	}
	if (gEnv.m_pKeyBoard->IsKeyDown(DIK_LEFTARROW))
	{
		x += delta;
	}
	if (gEnv.m_pKeyBoard->IsKeyDown(DIK_RIGHTARROW))
	{
		x -= delta;
	}

	//Draw teapot
	D3DXMatrixTranslation(&T,
		x,
		y,
		z);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &T);
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gTeapotMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, 0);
	gPTeapot->DrawSubset(0);

	/**
	 *	Enable stencil, disable write to z-buffer and back-buffer
	 *	*/
		//Enable stencil, so that the mirror fragment which passed z-test's stencil are set to 0x01
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILENABLE, true);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILREF, 0x01);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		//Disable write to z-buffer, use blend to disable write to color-buffer
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		//Finished setting, draw mirror to stencil-buffer;
	gEnv.m_pDXDevice->GetD3DDevice()->SetStreamSource(0, gPVertexBuffer, 0, sizeof(Vertex));
	gEnv.m_pDXDevice->GetD3DDevice()->SetFVF(Vertex::FVF);
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gMirrorMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, gPMirrorTex);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &I);
	gEnv.m_pDXDevice->GetD3DDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	/**
	 *	Then we need to use the modified stencil, disable z-test(why not disable z-test), and use blend to draw mirrored teapot
	 *	*/
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	gEnv.m_pDXDevice->GetD3DDevice()->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);


	D3DXMATRIX R;
	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f);
	D3DXMatrixReflect(&R, &plane);
	T = T*R;
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &T);
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gTeapotMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, 0);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	gPTeapot->DrawSubset(0);

	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILENABLE, false);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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