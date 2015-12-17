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
#include <dinput.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SAFE_RELEASE(p) {if(p){p->Release(); p = nullptr;}} 

//////////////////////////////////////////////////////////////////////////
// Declare external functions and global variables  
//////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern bool Direct3DInit(HWND hwnd, HINSTANCE hInstance);
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
LPDIRECTINPUT8 gPDirectInput = nullptr;
DIMOUSESTATE gDiMouseState;
LPDIRECTINPUTDEVICE8 gPMouseInput = nullptr;
LPDIRECTINPUTDEVICE8 gPKeyboardInput = nullptr;
char g_pKeyStateBuffer[256] = { 0 };

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//��1�����ڴ����Ĳ���֮һ����ʼ���һ�������Ĵ�����
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

	if (!Direct3DInit(hwnd, hInstance))
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

bool Direct3DInit(HWND hwnd, HINSTANCE hInstance)
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

	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&gPDirectInput, nullptr);
	gPDirectInput->CreateDevice(GUID_SysMouse, &gPMouseInput, nullptr);
	gPMouseInput->SetDataFormat(&c_dfDIMouse);
	gPMouseInput->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	gPMouseInput->Acquire();

	gPDirectInput->CreateDevice(GUID_SysKeyboard, &gPKeyboardInput, nullptr);
	gPKeyboardInput->SetDataFormat(&c_dfDIKeyboard);
	gPKeyboardInput->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NONEXCLUSIVE);
	gPKeyboardInput->Acquire();

	if (!ObjectInit(hwnd))
	{
		return false;
	}
	SAFE_RELEASE(pD3D9);

	return true;
}
BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize)
{
	HRESULT hr;
	while (true)
	{
		pDIDevice->Poll();              // ��ѯ�豸
		pDIDevice->Acquire();           // ��ȡ�豸�Ŀ���Ȩ
		if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;
		if (FAILED(pDIDevice->Acquire())) return FALSE;
	}
	return TRUE;
}
void InputUpdate()
{
	// ��ȡ������Ϣ������������Ӧ�����ģʽ  
	if (g_pKeyStateBuffer[DIK_1] & 0x80)         // �����ּ�1�����£�����ʵ�����  
		gPD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (g_pKeyStateBuffer[DIK_2] & 0x80)         // �����ּ�2�����£������߿����  
		gPD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// ��ȡ�������
	ZeroMemory(&gDiMouseState, sizeof(gDiMouseState));
	Device_Read(gPMouseInput, (LPVOID)&gDiMouseState, sizeof(gDiMouseState));

	// ��ȡ��������
	ZeroMemory(g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));
	Device_Read(gPKeyboardInput, (LPVOID)g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));


	// ��ס���������϶���Ϊƽ�Ʋ���
	static FLOAT fPosX = 0.0f, fPosY = 30.0f, fPosZ = 0.0f;
	if (gDiMouseState.rgbButtons[0] & 0x80)
	{
		fPosX += gDiMouseState.lX *  0.08f;
		fPosY += gDiMouseState.lY * -0.08f;
	}

	//�����֣�Ϊ�۲����������
	fPosZ += gDiMouseState.lZ * 0.02f;

	// ƽ������
	if (g_pKeyStateBuffer[DIK_A] & 0x80) fPosX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_D] & 0x80) fPosX += 0.005f;
	if (g_pKeyStateBuffer[DIK_W] & 0x80) fPosY += 0.005f;
	if (g_pKeyStateBuffer[DIK_S] & 0x80) fPosY -= 0.005f;


	D3DXMatrixTranslation(&gMatWorld, fPosX, fPosY, fPosZ);


	// ��ס����Ҽ����϶���Ϊ��ת����
	static float fAngleX = 0.15f, fAngleY = -(float)D3DX_PI;
	if (gDiMouseState.rgbButtons[1] & 0x80)
	{
		fAngleX += gDiMouseState.lY * -0.01f;
		fAngleY += gDiMouseState.lX * -0.01f;
	}
	// ��ת����
	if (g_pKeyStateBuffer[DIK_UP] & 0x80) fAngleX += 0.005f;
	if (g_pKeyStateBuffer[DIK_DOWN] & 0x80) fAngleX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_LEFT] & 0x80) fAngleY -= 0.005f;
	if (g_pKeyStateBuffer[DIK_RIGHT] & 0x80) fAngleY += 0.005f;


	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, fAngleX);
	D3DXMatrixRotationY(&Ry, fAngleY);

	//�õ����յľ�������
	gMatWorld = Rx * Ry * gMatWorld;//�õ����յľ���
	gPD3DDevice->SetTransform(D3DTS_WORLD, &gMatWorld);//�����������
}

void Direct3DRender(HWND hwnd)
{
	gPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	gPD3DDevice->BeginScene();
	
	InputUpdate();
	MatrixSet();

	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	for (int i = 0; i < gDwNumMtrl; i++)
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
	swprintf_s(strInfo, -1, L"ģ������: (%.2f, %.2f, %.2f)", gMatWorld._41, gMatWorld._42, gMatWorld._43);
	gPTextHelperFont->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));

	formatRect.top = WINDOW_HEIGHT * 2 / 3;
	gPTextInfoFont->DrawTextW(nullptr, _T("����˵����"), -1, &formatRect, DT_NOCLIP|DT_LEFT | DT_SINGLELINE, D3DCOLOR_XRGB(23, 25, 111));
	formatRect.top += 35;
	formatRect.left += 50;
	gPTextHelperFont->DrawText(NULL, L"��ס���������϶���ƽ��ģ��", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"��ס����Ҽ����϶�����תģ��", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"���������֣�����ģ��", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"W��S��A��D����ƽ��ģ�� ", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"�ϡ��¡����ҷ��������תģ�� ", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
	formatRect.top += 25;
	gPTextHelperFont->DrawText(NULL, L"ESC�� : �˳�����", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));


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
	PlaySound(_T("���ߥͥꥵ - Resuscitated Hope.wav"), nullptr, SND_ASYNC | SND_FILENAME | SND_LOOP);
	D3DXCreateFont(gPD3DDevice, 30, 0, 0, 0, 0, 0, 0, 0, 0, _T("΢���ź�"), &gPTextAdapterFont);
	D3DXCreateFont(gPD3DDevice, 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("��������"), &gPTextHelperFont);
	D3DXCreateFont(gPD3DDevice, 30, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"), &gPTextInfoFont);
	D3DXCreateFont(gPD3DDevice, 36, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"), &gPTextFPSFont);

	//////////////////////////////////////////////////////////////////////////
	// Load mesh and materials here  
	//////////////////////////////////////////////////////////////////////////
	LPD3DXBUFFER pAdjBuffer;
	LPD3DXBUFFER pMtrlBuffer;
	D3DXLoadMeshFromX(_T("loli.X"), D3DXMESH_MANAGED, gPD3DDevice, &pAdjBuffer, &pMtrlBuffer, nullptr, &gDwNumMtrl, &gPCharacter);
	D3DXMATERIAL* pMaterial = (D3DXMATERIAL*)(pMtrlBuffer->GetBufferPointer());
	gPMaterial = new D3DMATERIAL9[gDwNumMtrl];
	gPTexture = new LPDIRECT3DTEXTURE9[gDwNumMtrl];
	for (DWORD i = 0; i < gDwNumMtrl; i++)
	{
		gPMaterial[i] = pMaterial->MatD3D;
		gPMaterial[i].Ambient = gPMaterial[i].Diffuse;
		gPTexture[i] = nullptr;
		D3DXCreateTextureFromFileA(gPD3DDevice, pMaterial[i].pTextureFilename, &gPTexture[i]);
	}

	pAdjBuffer->Release();
	SAFE_RELEASE(pMtrlBuffer);
	// ������Ⱦ״̬
	gPD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //������������
	gPD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //���û�����

	return true;
}

void MatrixSet()
{
	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮һ��������任���������
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮������ȡ���任���������
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //����һ������
	D3DXVECTOR3 vEye(0.0f, 0.0f, -250.0f);  //�������λ��
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //�۲���λ��
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//���ϵ�����
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //�����ȡ���任����
	gPD3DDevice->SetTransform(D3DTS_VIEW, &matView); //Ӧ��ȡ���任����

	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮������ͶӰ�任���������
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj; //����һ������
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)((double)WINDOW_WIDTH / WINDOW_HEIGHT), 1.0f, 1000.0f); //����ͶӰ�任����
	gPD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);  //����ͶӰ�任����

	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮�ġ����ӿڱ任������
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //ʵ����һ��D3DVIEWPORT9�ṹ�壬Ȼ��������������������ֵ�Ϳ�����
	vp.X = 0;		//��ʾ�ӿ�����ڴ��ڵ�X����
	vp.Y = 0;		//�ӿ���ԶԴ��ڵ�Y����
	vp.Width = WINDOW_WIDTH;	//�ӿڵĿ��
	vp.Height = WINDOW_HEIGHT; //�ӿڵĸ߶�
	vp.MinZ = 0.0f; //�ӿ�����Ȼ����е���С���ֵ
	vp.MaxZ = 1.0f;	//�ӿ�����Ȼ����е�������ֵ
	gPD3DDevice->SetViewport(&vp); //�ӿڵ�����

}