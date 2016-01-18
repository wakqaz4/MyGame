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

	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"), &gPFont)))
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
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_VIEW, pMatView); //Ӧ��ȡ���任����

	D3DMATRIX* pProjMatrix;
	pProjMatrix = (D3DMATRIX*)(&gEnv.m_pCamera->GetProjMatrix().GetFliped());
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, pProjMatrix);  //����ͶӰ�任����

	D3DVIEWPORT9* pViewPort; 
	pViewPort = (D3DVIEWPORT9*)(&gEnv.m_pCamera->GetViewPort());
	gEnv.m_pDXDevice->GetD3DDevice()->SetViewport(pViewPort); //�ӿڵ�����

	//D3DXMATRIX matView; //����һ������
	//D3DXVECTOR3 vEye(0.0f, 0.0f, -50.0f);  //�������λ��
	//D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //�۲���λ��
	//D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//���ϵ�����
	//D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //�����ȡ���任����
	//gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_VIEW, &matView); //Ӧ��ȡ���任����

	//D3DXMATRIX matProj; //����һ������
	//D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)((double)800 / 600), 1.0f, 1000.0f); //����ͶӰ�任����
	//gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &matProj);  //����ͶӰ�任����

	//D3DVIEWPORT9 vp; //ʵ����һ��D3DVIEWPORT9�ṹ�壬Ȼ��������������������ֵ�Ϳ�����
	//vp.X = 0;		//��ʾ�ӿ�����ڴ��ڵ�X����
	//vp.Y = 0;		//�ӿ���ԶԴ��ڵ�Y����
	//vp.Width = 800;	//�ӿڵĿ��
	//vp.Height = 600; //�ӿڵĸ߶�
	//vp.MinZ = 0.0f; //�ӿ�����Ȼ����е���С���ֵ
	//vp.MaxZ = 1.0f;	//�ӿ�����Ȼ����е�������ֵ
	//gEnv.m_pDXDevice->GetD3DDevice()->SetViewport(&vp); //�ӿڵ�����


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