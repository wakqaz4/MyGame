/************************************************************************
* Use CreateMeshFVF to create mesh and fill in the vertices, indices, attributes
* buffer, which draws a cube on screen.
* Dragon book example in chapter 10.
* // [2016/1/19 14:46 Zhaoyu.Wang]  
************************************************************************/
#include "tchar.h"
#include "../../../win32Utility/win32Utility/win32Utility/win32Utility.h"
#include "../../../dInputUtility/dInputUtility/dInputUtility/KeyBoard.h"
#include "../../../dInputUtility/dInputUtility/dInputUtility/Mouse.h"
#include "../../../d3dUtility/d3dUtility/d3dUtility/DXDevice9.h"
#include "../../../GDCamera/GDCamera.h"
#include "../../../GDMath/GDMath.h"
#include "../../../GDObserver/GDObserver.h"
#include <vector>
#include <fstream>

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
void OutputVertices(std::ofstream& , LPD3DXMESH);
void OutputIndices(std::ofstream&, LPD3DXMESH);
void OutputAttributeTable(std::ofstream&, LPD3DXMESH);
void OutputAttributeBuffer(std::ofstream&, LPD3DXMESH);
void OutputAdjacencyBuffer(std::ofstream&, LPD3DXMESH);

//////////////////////////////////////////////////////////////////////////
// Put global variables here.  
//////////////////////////////////////////////////////////////////////////
LPD3DXFONT gPFont = nullptr;
LPD3DXMESH gPTeapot = nullptr;
LPD3DXMESH gPCube = nullptr;
D3DMATERIAL9 gTeapotMtrl;
LPDIRECT3DTEXTURE9 gPTexIce = nullptr;
LPDIRECT3DTEXTURE9 gPTexChecker = nullptr;
D3DMATERIAL9 gCubeMtrl;

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

	if (FAILED(D3DXCreateFont(gEnv.m_pDXDevice->GetD3DDevice(), 20, 0, 0, 0, 0, 0, 0, 0, 0, _T("楷体"), &gPFont)))
	{
		return false;
	}
	PlaySound(_T("日笠陽子 - 美しき残酷な世界.wav"), nullptr, SND_LOOP | SND_FILENAME | SND_ASYNC);

	D3DXCreateTeapot(gEnv.m_pDXDevice->GetD3DDevice(), &gPTeapot, 0);

	D3DXCreateTextureFromFile(gEnv.m_pDXDevice->GetD3DDevice(), L"ice.bmp", &gPTexIce);
	/*
	 * Create mesh with api
	 * */
	D3DXCreateMeshFVF(12, 24, D3DXMESH_MANAGED, Vertex::FVF, gEnv.m_pDXDevice->GetD3DDevice(), &gPCube);	
	/**
	 *	Fill in the mesh with vertices, indices, attribute buffer
	 *	*/
	Vertex* v = 0;
	gPCube->LockVertexBuffer(0, (void**)&v);
	// fill in the front face vertex data
	v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[1] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[2] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	v[3] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	// fill in the back face vertex data
	v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[5] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[7] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	// fill in the top face vertex data
	v[8] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[9] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[10] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[11] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	// fill in the bottom face vertex data
	v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[13] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[15] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	// fill in the left face vertex data
	v[16] = Vertex(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[17] = Vertex(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[18] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// fill in the right face vertex data
	v[20] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[21] = Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[22] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[23] = Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gPCube->UnlockVertexBuffer();

	WORD* i = 0;
	gPCube->LockIndexBuffer(0, (void**)&i);
	// fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;
	// fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;
	// fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;
	// fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;
	// fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;
	// fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;
	gPCube->UnlockIndexBuffer();
	/**
	*	I think after the previous step, there already exists attributes in
	*	attribute buffer. And we'll assign different ID for the first 6 and 
	*	the last 6 triangles seperately.
	*	*/
	DWORD* pAttrib = nullptr;
	gPCube->LockAttributeBuffer(0, &pAttrib);
	for (int k = 0; k <= 1; k++)
	{
		for (int j = 0; j < 6; j++)
		{
			pAttrib[j + k*6] = k;
		}
	}
	gPCube->UnlockAttributeBuffer();
	/**
	 *	After this step, we can already draw the cube. 
	 *	However, you may have already noticed that some vertices
	 *	took the same position. So it's better to make some 
	 *	optimization.
	 *	*/
	std::vector<DWORD> adjacencyBuffer(gPCube->GetNumFaces() * 3);
	gPCube->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);
	gPCube->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		&adjacencyBuffer[0],
		0, 0, 0);
	/**
	 *	And I'll suggest you take a look at the vertices, indices, 
	 *	and attributes, adjacency buffers. So, let's print them out!
	 *	*/
	std::ofstream outFile;
	outFile.open("meshInfo.txt");
	OutputVertices(outFile, gPCube);
	OutputIndices(outFile, gPCube);
	OutputAttributeTable(outFile, gPCube);
//	OutputAttributeBuffer(outFile, gPCube);
	OutputAdjacencyBuffer(outFile, gPCube);

	outFile.close();

	DWORD numEntries = 0;
	gPCube->GetAttributeTable(0, &numEntries);

	gTeapotMtrl.Ambient = D3DXCOLOR(1.0f, 0.1f, 0.6f, 1.0f);
	gTeapotMtrl.Diffuse = D3DXCOLOR(0.3f, 0.6f, 0.0f, 1.0f);
	gTeapotMtrl.Specular = D3DXCOLOR(1.0f, 0.9f, 0.3f, 1.0f);
	gTeapotMtrl.Power = 2.0f;

	gCubeMtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gCubeMtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gCubeMtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	gCubeMtrl.Power = 2.0f;

	D3DXCreateTextureFromFile(gEnv.m_pDXDevice->GetD3DDevice(), _T("ice.bmp"), &gPTexIce);

	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	dir.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)* 0.2f;
	dir.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) * 0.6f;
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);

	gEnv.m_pDXDevice->GetD3DDevice()->SetLight(0, &dir);
	gEnv.m_pDXDevice->GetD3DDevice()->LightEnable(0, true);

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
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, gPTexIce);
	gPTeapot->DrawSubset(0);

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &W);
	gEnv.m_pDXDevice->GetD3DDevice()->SetMaterial(&gCubeMtrl);
	gEnv.m_pDXDevice->GetD3DDevice()->SetTexture(0, gPTexIce);
	gEnv.m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	DWORD numEntries = 0;
	gPCube->GetAttributeTable(0, &numEntries);

	for (int j = 0; j < numEntries; j++)
	{
		gPCube->DrawSubset(j);
	}
	
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

void OutputVertices(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Vertices:" << std::endl;
	outFile << "---------" << std::endl << std::endl;

	Vertex* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < mesh->GetNumVertices(); i++)
	{
		outFile << "Vertex " << i << ": (";
		outFile << v[i]._x << ", " << v[i]._y << ", " << v[i]._z << ", ";
		outFile << v[i]._nx << ", " << v[i]._ny << ", " << v[i]._nz << ", ";
		outFile << v[i]._u << ", " << v[i]._v << ")" << std::endl;
	}
	mesh->UnlockVertexBuffer();

	outFile << std::endl << std::endl;
}

void OutputIndices(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Indices:" << std::endl;
	outFile << "--------" << std::endl << std::endl;

	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	for (int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle " << i << ": ";
		outFile << indices[i * 3] << " ";
		outFile << indices[i * 3 + 1] << " ";
		outFile << indices[i * 3 + 2] << std::endl;
	}
	mesh->UnlockIndexBuffer();

	outFile << std::endl << std::endl;
}

void OutputAttributeBuffer(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Attribute Buffer:" << std::endl;
	outFile << "-----------------" << std::endl << std::endl;

	DWORD* attributeBuffer = 0;
	mesh->LockAttributeBuffer(0, &attributeBuffer);

	// an attribute for each face
	for (int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle lives in subset " << i << ": ";
		outFile << attributeBuffer[i] << std::endl;
	}
	mesh->UnlockAttributeBuffer();

	outFile << std::endl << std::endl;
}

void OutputAdjacencyBuffer(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Adjacency Buffer:" << std::endl;
	outFile << "-----------------" << std::endl << std::endl;

	// three enttries per face
	std::vector<DWORD> adjacencyBuffer(mesh->GetNumFaces() * 3);

	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);

	for (int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle's adjacent to triangle " << i << ": ";
		outFile << adjacencyBuffer[i * 3] << " ";
		outFile << adjacencyBuffer[i * 3 + 1] << " ";
		outFile << adjacencyBuffer[i * 3 + 2] << std::endl;
	}

	outFile << std::endl << std::endl;
}

void OutputAttributeTable(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Attribute Table:" << std::endl;
	outFile << "----------------" << std::endl << std::endl;

	// number of entries in the attribute table
	DWORD numEntries = 0;

	mesh->GetAttributeTable(0, &numEntries);

	std::vector<D3DXATTRIBUTERANGE> table(numEntries);

	mesh->GetAttributeTable(&table[0], &numEntries);

	for (int i = 0; i < numEntries; i++)
	{
		outFile << "Entry " << i << std::endl;
		outFile << "-----------" << std::endl;

		outFile << "Subset ID:    " << table[i].AttribId << std::endl;
		outFile << "Face Start:   " << table[i].FaceStart << std::endl;
		outFile << "Face Count:   " << table[i].FaceCount << std::endl;
		outFile << "Vertex Start: " << table[i].VertexStart << std::endl;
		outFile << "Vertex Count: " << table[i].VertexCount << std::endl;
		outFile << std::endl;
	}

	outFile << std::endl << std::endl;
}