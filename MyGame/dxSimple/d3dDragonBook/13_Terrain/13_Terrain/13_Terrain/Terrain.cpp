/************************************************************************
*                                                                   
************************************************************************/
#include "Terrain.h"
#include "../../../GDMacro/GDMacro.h"
#include <fstream>

using namespace GD;
const GD::DWORD CTerrain::Vertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

CTerrain::CTerrain(CDXDevice9* pDevice)
{
	CTerrain(pDevice, L"coastMountain64.raw", 64, 64, 10, 0.5f);
};

CTerrain::CTerrain(CDXDevice9* pDXDevice, std::wstring heightmapFileName, uint32 numVertsPerRow, uint32 numVertsPerCol, uint32 cellSpacing, f32 heightScale)
{
	m_pDXDevice = pDXDevice;
	m_numVertsPerRow = numVertsPerRow;
	m_numVertsPerCol = numVertsPerCol;
	m_cellSpacing = cellSpacing;
	m_heightScale = heightScale;

	m_numCellsPerRow = m_numVertsPerRow - 1;
	m_numCellsPerCol = m_numVertsPerCol - 1;
	m_width = m_numCellsPerRow*m_cellSpacing;
	m_depth = m_numCellsPerCol*m_cellSpacing;

	m_numVertices = m_numVertsPerRow*m_numVertsPerCol;
	m_numTriangles = m_numCellsPerRow*m_numCellsPerCol * 2;
	
	/**
	 *	Load heightmap.
	 */
	if (!ReadRawFile(heightmapFileName))
	{
		::MessageBoxA(0, "ReadRawHeightmap()-Failed!", "Error", MB_OK);
		::PostQuitMessage(0);
	}

	/**
	 *	Then we need to scale heightmap.
	 */
	for (uint32 i = 0; i < m_heightmap.size(); i++)
	{
		m_heightmap[i] = (int)(m_heightScale*m_heightmap[i]);
	}
	/**
	 *	Compute vertices.
	 */
	if (!ComputeVertices())
	{
		::MessageBoxA(0, "ComputeVertices()-Failed!", "Error", MB_OK);
		::PostQuitMessage(0);
	}
	/**
	 *	And then compute indices.
	 */
	if (!ComputeIndices())
	{
		::MessageBoxA(0, "ComputeIndices()-Failed!", "Error", MB_OK);
		::PostQuitMessage(0);
	}
	/**
	 *	Here we've finished all computing, and the terrain mesh is filled
	 *	with its vertices & indices, we need only to draw it in renderFunc.
	 */
}

CTerrain::~CTerrain()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pTex);
}

void CTerrain::SetHeightmapUnit(int32 row, int32 col, int32 value)
{
	m_heightmap[row*m_numVertsPerRow + col] = value;
}

int32 CTerrain::GetHeightmapUnit(int32 row, int32 col)
{
	return m_heightmap[row*m_numVertsPerRow + col];
}

bool CTerrain::ComputeVertices()
{
	if (FAILED(m_pDXDevice->GetD3DDevice()->CreateVertexBuffer(
		m_numVertices*sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&m_pVertexBuffer,
		0)))
	{
		return false;
	}

	int32 startX = -m_width / 2;
	int32 startZ = m_depth / 2;
	int32 endX = m_width / 2;
	int32 endZ = -m_depth / 2;

	f32 uCoordIncrementSize = 1.0f / (f32)m_numCellsPerRow;
	f32 vCoordIncrementSize = 1.0f / (f32)m_numCellsPerCol;

	Vertex* v = nullptr;
	m_pVertexBuffer->Lock(0, 0, (void**)&v, 0);

	int32 i = 0;
	for (int32 z = startZ; z >= endZ; z-=m_cellSpacing)
	{
		int32 j = 0;
		for (int32 x = startX; x <= endX; x += m_cellSpacing)
		{
			int32 index = i * m_numVertsPerRow + j;
			v[index] = Vertex(
				(float)x,
				(float)m_heightmap[index],
				(float)z,
				(float)j*uCoordIncrementSize,
				(float)i*vCoordIncrementSize);
			j++;
		}
		i++;
	}
	m_pVertexBuffer->Unlock();
	return true;
}

bool CTerrain::ComputeIndices()
{
	if (FAILED(m_pDXDevice->GetD3DDevice()->CreateIndexBuffer(
		m_numTriangles * 3 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIndexBuffer,
		0)))
	{
		return false;
	}

	WORD* pIndex= 0;
	m_pIndexBuffer->Lock(0, 0, (void**)&pIndex, 0);
	int32 k = 0;
	for (int32 i = 0; i < m_numCellsPerCol; i++)
	{
		for (int32 j = 0; j < m_numCellsPerRow; j++)
		{
			int32 baseNum = i*m_numVertsPerRow + j;
			// The first triangle
			pIndex[k++] = baseNum;
			pIndex[k++] = baseNum + 1;
			pIndex[k++] = baseNum + m_numVertsPerRow;
			//The second triangle
			pIndex[k++] = baseNum + m_numVertsPerRow;
			pIndex[k++] = baseNum + 1;
			pIndex[k++] = baseNum + m_numVertsPerRow + 1;
		}
	}
	m_pIndexBuffer->Unlock();

	return true;
}

bool CTerrain::LoadTexture(std::wstring fileName)
{
	if (FAILED(D3DXCreateTextureFromFileW
		(m_pDXDevice->GetD3DDevice(),
		fileName.c_str(),
		&m_pTex
		)))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/**
 *	In fact, this method does not generate a new texture. Instead, it 
 *	fill the vertices with different kinds of manual-made texture.
 */
bool CTerrain::GenTexture(const Vector3& lightDir)
{
	HRESULT hr = 0;
	int32 texWidth = m_numCellsPerRow;
	int32 texHeight = m_numCellsPerCol;
	/**
	 *	Create empty texture
	 */
	hr = D3DXCreateTexture(
		m_pDXDevice->GetD3DDevice(),
		texWidth,
		texHeight,
		0,
		0,
		D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED,
		&m_pTex);
	if (FAILED(hr))
	{
		return false;
	}
	/**
	 *	Make sure that the texture has requested format
	 */
	D3DSURFACE_DESC textureDesc;
	m_pTex->GetLevelDesc(0, &textureDesc);
	if (textureDesc.Format != D3DFMT_X8R8G8B8)
	{
		return false;
	}
	/**
	 *	Lock texture buffer and write into pixel
	 */
	D3DLOCKED_RECT lockedRect;
	m_pTex->LockRect(0, &lockedRect, 0, 0);
	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for (int32 i = 0; i < texHeight; i++)
	{
		for (int32 j = 0; j < texWidth; j++)
		{
			D3DXCOLOR c;
			float height = (float)GetHeightmapUnit(i, j);
			if (height < 42.5f)
			{
				c = D3DCOLOR_BEACH_SAND;
			}
			else if (height < 85.0f)
			{
				c = D3DCOLOR_LIGHT_YELLOW_GREEN;
			}
			else if (height < 127.5f)
			{
				c = D3DCOLOR_PUREGREEN;
			}
			else if (height < 170.0f)
			{
				c = D3DCOLOR_DARK_YELLOW_GREEN;
			}
			else if (height < 212.5f)
			{
				c = D3DCOLOR_DARKBROWN;
			}
			else
			{
				c = D3DCOLOR_WHITE;
			}
			imageData[i*lockedRect.Pitch / 4 + j] = (D3DCOLOR)c;
		}
	}
	m_pTex->UnlockRect(0);
	if (!LightTerrain(lightDir))
	{
		::MessageBox(0, L"LightTerrain()-Failed", 0, MB_OK);
		return false;
	}
	hr = D3DXFilterTexture(
		m_pTex,
		0,
		0,
		D3DX_DEFAULT);
	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXFilterTexture()-FAILED", 0, MB_OK);
		return false;
	}

	return true;
}

bool CTerrain::LightTerrain(const Vector3& lightDir)
{
	HRESULT hr = 0;
	D3DSURFACE_DESC textureDesc;
	m_pTex->GetLevelDesc(0, &textureDesc);
	if (textureDesc.Format != D3DFMT_X8R8G8B8)
	{
		return false;
	}

	D3DLOCKED_RECT lockedRect;
	m_pTex->LockRect(0,
		&lockedRect,
		0,
		0);
	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for (uint32 i = 0; i < textureDesc.Height; i++)
	{
		for (uint32 j = 0; j < textureDesc.Width; j++)
		{
			int32 index = i*lockedRect.Pitch / 4 + j;
			D3DXCOLOR c(imageData[index]);
			c *= ComputeShadow(i, j, lightDir);
			imageData[index] = (D3DCOLOR)c;
		}
	}
	m_pTex->UnlockRect(0);

	return true;
}
/**
 *	This is not a shadow...In fact, it should be called shade, which means the 
 *	degree of light and dark.
 */
float CTerrain::ComputeShadow(int32 cellRow, int32 cellCol, const Vector3& lightDir)
{
	float heightA = (float)GetHeightmapUnit(cellRow, cellCol);
	float heightB = (float)GetHeightmapUnit(cellRow, cellCol + 1);
	float heightC = (float)GetHeightmapUnit(cellRow + 1, cellCol);

	Vector3 u((float)m_cellSpacing, (float)heightB - heightA, 0.0f);
	Vector3 v(0.0f, (float)heightC - heightA, (float)-m_cellSpacing);

	Vector3 n;
	n = u.Cross(v);
	n.Normalize();
	float cosine = n*lightDir;
	if (cosine < 0.0f)
	{
		cosine = 0.0f;
	}
	return cosine;
}
/**
 *	Read bytes into int type, and then copy to m_heightmap;
 */
bool CTerrain::ReadRawFile(std::wstring fileName)
{
	std::ifstream inFile(fileName, std::ios_base::binary);

	std::vector<unsigned char> in(m_numVertices);
	inFile.read(
		(char*)&in[0],
		in.size());
	inFile.close();
	m_heightmap.resize(m_numVertices);
	/**
	 *	Need to figure out that if variables in vector are consistant?
	 *	No, it's not.
	 */
	//memcpy((void*)m_heightmap[0], (void*)in[0], m_numVertices*sizeof(int));
	for (uint32 i = 0; i < in.size(); i++)
	{
		m_heightmap[i] = in[i];
	}
	return true;
}

float CTerrain::GetHeight(f32 x, f32 z)
{
	x = (float)m_width / 2.0f + x;
	z = (float)m_depth / 2.0f - z;

	x /= (float)m_cellSpacing;
	z /= (float)m_cellSpacing;

	float col = ::floorf(x);
	float row = ::floorf(z);
	if (row >= m_numVertsPerRow-1)
	{
		row = m_numVertsPerRow - 2;
	}
	else if (row <= 0)
	{
		row = 0;
	}
	if (col >= m_numVertsPerCol-1)
	{
		col = m_numVertsPerCol - 2;
	}
	else if (col <= 0)
	{
		col = 0;
	}
	float topLeft = (float)GetHeightmapUnit((int32)row, (int32)col);
	float topRight = (float)GetHeightmapUnit((int32)row, (int32)col + 1);
	float bottomLeft = (float)GetHeightmapUnit((int32)row + 1, (int32)col);
	float bottomRight = (float)GetHeightmapUnit((int32)row + 1, (int32)col + 1);

	float dx = x - col;
	float dz = z - row;

	float height = 0.0f;
	if (dz < 1.0f - dx)
	{
		float uy = topRight - topLeft;
		float vy = bottomLeft - topLeft;
		height = topLeft + uy*(1.0f - dx) + vy*(1.0f - dz);
	}
	else
	{
		float uy = bottomLeft - bottomRight;
		float vy = topRight - bottomRight;
		height = bottomRight + uy*(1.0f - dx) + vy*(1.0f - dz);
	}

	return height;
}

bool CTerrain::Draw(Matrix worldMat, bool drawTris)
{
	if (m_pDXDevice == nullptr)
	{
		return false;
	}
	D3DXMATRIX mat = *(D3DXMATRIX*)((void*)(&worldMat));
	m_pDXDevice->GetD3DDevice()->SetTransform(D3DTS_WORLD, &mat);
	m_pDXDevice->GetD3DDevice()->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(Vertex));
	m_pDXDevice->GetD3DDevice()->SetFVF(Vertex::FVF);
	m_pDXDevice->GetD3DDevice()->SetIndices(m_pIndexBuffer);
	m_pDXDevice->GetD3DDevice()->SetTexture(0, m_pTex);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, false);
	m_pDXDevice->GetD3DDevice()->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,
		0,
		0,
		m_numVertices,
		0,
		m_numTriangles);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, true);

	if (drawTris)
	{
		m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pDXDevice->GetD3DDevice()->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			m_numVertices,
			0,
			m_numTriangles);
		m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	return true;
}