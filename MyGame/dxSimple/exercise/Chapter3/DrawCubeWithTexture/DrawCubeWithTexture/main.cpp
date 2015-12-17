/************************************************************************
* This program draws a cube with vertices&indices, while using a texture
* to fit on the cube's surface
************************************************************************/

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include "MyCommon.h"
#include "DirectInputClass.h"
#include "D3DDevice.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables  
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool ObjectInit();
extern void Direct3DRender(HWND hwnd);
extern void Direct3DUpdate(HWND hwnd);
extern bool Direct3DClearup();
extern void MatrixSet();

CD3DDevice gD3DDevice;
CDirectInputClass gDirectInput;
LPD3DXFONT gPFont = nullptr;
LPDIRECT3DVERTEXBUFFER9 gPVertexBuffer = nullptr;
LPDIRECT3DINDEXBUFFER9 gPIndexBuffer = nullptr;
D3DMATERIAL9* gPMaterial = nullptr;
DWORD gDwNumMtrl = 0;
LPDIRECT3DTEXTURE9 gPTexture = nullptr;
LPD3DXMESH gPMesh;

struct CUSTOM_VERTEX
{
	FLOAT _x, _y, _z;               // �����λ��
	FLOAT _u, _v;                   // ��������
	CUSTOM_VERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v)
		: _x(x), _y(y), _z(z), _u(u), _v(v) {}
};
#define D3DFVF_CUSTOM_VERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };							//��WINDCLASSEX������һ��������
	wndClass.cbSize = sizeof(WNDCLASSEX);			//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;					//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra = 0;								//������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra = 0;							//���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //ָ��������Ĺ������
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	//��2�����ڴ����Ĳ���֮����ע�ᴰ����
	if (!RegisterClassEx(&wndClass))				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;

	//��3�����ڴ����Ĳ���֮������ʽ��������
	HWND hwnd = CreateWindowW(L"ForTheDreamOfGameDevelop", L"What the title!",				//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!gD3DDevice.Init(hwnd))
	{
		return -1;
	}
	if (!gDirectInput.Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))
	{
		return -1;
	}
	if (!ObjectInit())
	{
		return -1;
	}

	TCHAR ch[30] = TEXT("����");
	MessageBox(nullptr, ch, ch, MB_OK);

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
			Direct3DUpdate(hwnd);
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
		Direct3DUpdate(hwnd);
		Direct3DRender(hwnd);
		ValidateRect(hwnd, nullptr);
		break;
	case WM_DESTROY:
		Direct3DClearup();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

bool ObjectInit()
{
	PlaySound(_T("���ߥͥꥵ - Resuscitated Hope.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);
	srand(unsigned(time(nullptr)));
	
	D3DXCreateFont(gD3DDevice.m_pD3DDevice, 36, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"), &gPFont);


	if (FAILED((gD3DDevice.m_pD3DDevice)->CreateVertexBuffer(24 * sizeof(CUSTOM_VERTEX), 0, D3DFVF_CUSTOM_VERTEX, D3DPOOL_DEFAULT, &gPVertexBuffer, nullptr)))
	{
		return false;
	}
	CUSTOM_VERTEX* pVertices;
	if (FAILED(gPVertexBuffer->Lock(0, 24 * sizeof(CUSTOM_VERTEX), (void**)&pVertices, 0)))
		return false;

	// ���涥������
	pVertices[0] = CUSTOM_VERTEX(-10.0f, 10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[1] = CUSTOM_VERTEX(10.0f, 10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[2] = CUSTOM_VERTEX(10.0f, -10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOM_VERTEX(-10.0f, -10.0f, -10.0f, 0.0f, 1.0f);

	// ���涥������
	pVertices[4] = CUSTOM_VERTEX(10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[5] = CUSTOM_VERTEX(-10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[6] = CUSTOM_VERTEX(-10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[7] = CUSTOM_VERTEX(10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// ���涥������
	pVertices[8] = CUSTOM_VERTEX(-10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[9] = CUSTOM_VERTEX(10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[10] = CUSTOM_VERTEX(10.0f, 10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[11] = CUSTOM_VERTEX(-10.0f, 10.0f, -10.0f, 0.0f, 1.0f);

	// ���涥������
	pVertices[12] = CUSTOM_VERTEX(-10.0f, -10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[13] = CUSTOM_VERTEX(10.0f, -10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[14] = CUSTOM_VERTEX(10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[15] = CUSTOM_VERTEX(-10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// ����涥������
	pVertices[16] = CUSTOM_VERTEX(-10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[17] = CUSTOM_VERTEX(-10.0f, 10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[18] = CUSTOM_VERTEX(-10.0f, -10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[19] = CUSTOM_VERTEX(-10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// �Ҳ��涥������
	pVertices[20] = CUSTOM_VERTEX(10.0f, 10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[21] = CUSTOM_VERTEX(10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[22] = CUSTOM_VERTEX(10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[23] = CUSTOM_VERTEX(10.0f, -10.0f, -10.0f, 0.0f, 1.0f);

	gPVertexBuffer->Unlock();

	if (FAILED((gD3DDevice.m_pD3DDevice)->CreateIndexBuffer(36 * sizeof(WORD), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &gPIndexBuffer, nullptr)))
	{
		return false;
	}
	WORD *pIndices = nullptr;
	gPIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);
	// ������������
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

	// ������������
	pIndices[6] = 4; pIndices[7] = 5; pIndices[8] = 6;
	pIndices[9] = 4; pIndices[10] = 6; pIndices[11] = 7;

	// ������������
	pIndices[12] = 8; pIndices[13] = 9; pIndices[14] = 10;
	pIndices[15] = 8; pIndices[16] = 10; pIndices[17] = 11;

	// ������������
	pIndices[18] = 12; pIndices[19] = 13; pIndices[20] = 14;
	pIndices[21] = 12; pIndices[22] = 14; pIndices[23] = 15;

	// �������������
	pIndices[24] = 16; pIndices[25] = 17; pIndices[26] = 18;
	pIndices[27] = 16; pIndices[28] = 18; pIndices[29] = 19;

	// �Ҳ�����������
	pIndices[30] = 20; pIndices[31] = 21; pIndices[32] = 22;
	pIndices[33] = 20; pIndices[34] = 22; pIndices[35] = 23;

	gPIndexBuffer->Unlock();

	D3DXCreateTextureFromFileEx(gD3DDevice.m_pD3DDevice, L"pal5q.jpg", 0, 0, 6, 0, D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, 0, 0, &gPTexture);

	// ���ò���
	D3DMATERIAL9 mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gD3DDevice.m_pD3DDevice->SetMaterial(&mtrl);

	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	light.Direction = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	gD3DDevice.m_pD3DDevice->SetLight(0, &light); //���ù�Դ  
	gD3DDevice.m_pD3DDevice->LightEnable(0, true);//���ù���  

	// ��ʼ������Ⱦ״̬
	gD3DDevice.m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //������������
	gD3DDevice.m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //���û�����

	//�������Թ���
	gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 3);
	gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

	return true;
}

void Direct3DUpdate(HWND hwnd)
{
	D3DXMATRIX matWorld;
	gDirectInput.Update();

	if (gDirectInput.IsKeyDown(DIK_1))  //������1��������
	{
		// �����ظ�����Ѱַģʽ
		gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
	if (gDirectInput.IsKeyDown(DIK_2)) //������2��������
	{
		// ���þ�������Ѱַģʽ
		gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
		gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
	}
	if (gDirectInput.IsKeyDown(DIK_3)) //������3��������
	{
		// ���ü�ȡ����Ѱַģʽ
		gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
	if (gDirectInput.IsKeyDown(DIK_4)) //������4��������
	{
		// ���ñ߿�����Ѱַģʽ
		gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		gD3DDevice.m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	}

	static float translationX = 0;
	static float translationY = 0;
	static float translationZ = 0;
	static float rotationX = 0;
	static float rotationY = 0;
	static float rotationZ = 0;
	if (gDirectInput.IsKeyDown(DIK_W))
	{
		translationY += 0.05f;
	}
	if (gDirectInput.IsKeyDown(DIK_S))
	{
		translationY -= 0.05f;
	}
	if (gDirectInput.IsKeyDown(DIK_A))
	{
		translationX -= 0.05f;
	}
	if (gDirectInput.IsKeyDown(DIK_D))
	{
		translationX += 0.05f;
	}
	if (gDirectInput.IsKeyDown(DIK_UP))
	{
		rotationX += 0.0005f;
	}
	if (gDirectInput.IsKeyDown(DIK_DOWN))
	{
		rotationX -= 0.0005f;
	}
	if (gDirectInput.IsKeyDown(DIK_LEFT))
	{
		rotationY -= 0.0005f;
	}
	if (gDirectInput.IsKeyDown(DIK_RIGHT))
	{
		rotationY += 0.0005f;
	}



	D3DXMATRIX matRx;
	D3DXMATRIX matRy;
	D3DXMatrixRotationX(&matRx, rotationX);
	D3DXMatrixRotationY(&matRy, rotationY);
	D3DXMatrixTranslation(&matWorld, translationX, translationY, translationZ);
	matWorld = matRx*matRy*matWorld;
	gD3DDevice.m_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	MatrixSet();

}

bool Direct3DClearup()
{
	gD3DDevice.Clearup();
	return true;
}

void MatrixSet()
{

	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 0.0f, -50.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	gD3DDevice.m_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 6.0f, WINDOW_WIDTH / WINDOW_HEIGHT, 1.0f, 1000.0f);
	gD3DDevice.m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DVIEWPORT9 viewPort;
	viewPort.X = 0;
	viewPort.Y = 0;
	viewPort.Width = WINDOW_WIDTH;
	viewPort.Height = WINDOW_HEIGHT;
	viewPort.MinZ = 0.0f;
	viewPort.MaxZ = 1.0f;
	gD3DDevice.m_pD3DDevice->SetViewport(&viewPort);
}
void Direct3DRender(HWND hwnd)
{
	gD3DDevice.m_pD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	Direct3DUpdate(hwnd);
	gD3DDevice.m_pD3DDevice->BeginScene();

	gD3DDevice.m_pD3DDevice->SetStreamSource(0, gPVertexBuffer, 0, sizeof(CUSTOM_VERTEX));
	gD3DDevice.m_pD3DDevice->SetFVF(D3DFVF_CUSTOM_VERTEX);
	gD3DDevice.m_pD3DDevice->SetIndices(gPIndexBuffer);
	gD3DDevice.m_pD3DDevice->SetTexture(0, gPTexture);
	gD3DDevice.m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//��������������϶��㻺�����ͼ��

	gPFont->DrawTextW(nullptr, _T("Fake"), -1, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(23, 144, 2));

	gD3DDevice.m_pD3DDevice->EndScene();
	gD3DDevice.m_pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}
