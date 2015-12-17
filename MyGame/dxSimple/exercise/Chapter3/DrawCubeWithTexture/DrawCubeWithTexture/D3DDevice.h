#include "MyCommon.h"
#include <d3d9.h>
#include <d3dx9.h>

class CD3DDevice
{
public:
	LPDIRECT3D9 m_pD3D9;
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	D3DMATERIAL9* m_pMaterial;
	LPDIRECT3DTEXTURE9* m_pTexture;

public:
	CD3DDevice();
	~CD3DDevice();
	bool Init(HWND hwnd);
	void Render(HWND hwnd, pFunc func);
	bool Clearup();
	void MatrixSet();

};