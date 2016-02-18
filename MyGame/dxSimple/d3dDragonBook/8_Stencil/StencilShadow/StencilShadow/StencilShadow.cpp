/************************************************************************
* Generate shadow using stencil, while still generate reflected teapot in
* the mirror.
* D3D9 dragon book example in chapter 7.
* // [2016/1/18 10:27 Zhaoyu.Wang]  
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

LPD3DXMESH gPText = nullptr;

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
	gEnv.m_pWindow->Init(WndProc, _T("Shadow Stencil"), 1200, 800);

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
	
	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 36, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}

	HDC hdc = ::CreateCompatibleDC(0);

	// Fill out a LOGFONT structure that describes the font’s properties.
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));

	lf.lfHeight = 25; // in logical units
	lf.lfWidth = 12; // in logical units
	lf.lfWeight = 500; // boldness, range 0(light) - 1000(bold)
	lf.lfItalic = false;
	lf.lfUnderline = false;
	lf.lfStrikeOut = false;
	lf.lfCharSet = DEFAULT_CHARSET;
	::wcscpy_s(lf.lfFaceName, L"Times New Roman"); // font style

	// Create a font and select that font with the device context.
	HFONT hFont;
	HFONT hFontOld;
	hFont = CreateFontIndirect(&lf);
	hFontOld = (HFONT)SelectObject(hdc, hFont);

	// Create the 3D mesh of text.
	D3DXCreateTextW(gEnv.m_pDXDevice->GetD3DDevice(), hdc, L"Direct3D", 0.001f, 0.4f, &gPText, 0, 0);

	//Reselect the old font, and free resources.
	::SelectObject(hdc, hFontOld);
	::DeleteObject(hFont);
	::DeleteDC(hdc);

	::PlaySound(_T("日笠陽子 - 美しき残酷な世界.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);

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

	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gWallMtrl);
	D3DXMATRIX textMat;	
	D3DXMatrixTranslation(&textMat, 10, 0, 0);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &textMat);
	gPText->DrawSubset(0);

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
	W = T*R;
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gTeapotMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, 0);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	gPTeapot->DrawSubset(0);

	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILENABLE, false);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	/**
	 *	And add shadow here, with 2 steps: disable z-test to avoid depth-cause flitering,
	 *	set stencil state to avoid multi-pixel rendering.
	 */
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILREF, 0x00);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
	
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ZENABLE, false);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	D3DXMATRIX shadowProjMat;
	D3DXVECTOR4 lightDirection(0.707f, -0.707f, 0.707f, 0.0f);
	D3DXPLANE shadowPlane(0, -1, 0, 0);
	D3DXMatrixShadow(&shadowProjMat, &lightDirection, &shadowPlane);
	W = T*shadowProjMat;

	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	D3DMATERIAL9 shadowMtrl;
	shadowMtrl.Ambient = D3DXCOLOR(0.1, 0.1, 0.1, 1);
	shadowMtrl.Diffuse = D3DXCOLOR(0.1, 0.1, 0.1, 1);
	shadowMtrl.Emissive = D3DXCOLOR(0.1, 0.1, 0.1, 1);
	shadowMtrl.Specular = D3DXCOLOR(0.1, 0.1, 0.1, 1);
	shadowMtrl.Power = 2.0f;
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&shadowMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, 0);
	gPTeapot->DrawSubset(0);

	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ZENABLE, true);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_STENCILENABLE, false);
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