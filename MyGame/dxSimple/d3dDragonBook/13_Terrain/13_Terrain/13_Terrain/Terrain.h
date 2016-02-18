/************************************************************************
* Implements a terrain class using original DX APIs.
* 
* Terrain should not be singleton, considering that multi heightMap and 
* different kinds of terrains required, while also considering vertex buffer
* limits.
* Terrain should contain material, texture, lightening, of course Terrain
* itself should be a mesh that generated from heightMap.
* In order to save computation, we need to make terrain pre-lightened. However,
* terrain-real-time-lightening has already been implemented by senior engines 
* such as CE3 or UE4. Better take a reference of them.
* 
* I think I'll implement the upward content, and then take a look at CE3's terrain,
* which will be referenced in the GD Engine. As long as UE4 is concerned...
* have not gathered enough courage to study it yet...
* // [2016/1/21 21:07 Zhaoyu.Wang]  
************************************************************************/
#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#pragma once

#include "../../../d3dUtility/d3dUtility/d3dUtility/DXDevice9.h"
#include "../../../GDTypedef/GDTypedef.h"
#include "../../../GDMacro/GDMacro.h"
#include "../../../GDMath/GDMath.h"
#include <vector>

namespace GD
{
	/**
	 *	Which class should CTerrain public from?
	 *	*/
	class CTerrain
	{
		struct Vertex
		{
			Vertex(){};
			Vertex(f32 x, f32 y, f32 z, f32 u, f32 v)
			{
				m_x = x; m_y = y; m_z = z; m_u = u; m_v = v;
			}
			f32 m_x;
			f32 m_y;
			f32 m_z;
			f32 m_u;
			f32 m_v;
			static const DWORD FVF;
		};

	public:
		CTerrain(CDXDevice9* pDXDevice);
		CTerrain(CDXDevice9* pDXDevice,
			std::wstring heightmapFileName,
			uint32 numVertsPerRow,
			uint32 numVertsPerCol,
			uint32 cellSpacing,
			f32 heightScale);
		~CTerrain();

		int32	GetHeightmapUnit(int32 row, int32 col);
		void	SetHeightmapUnit(int32 row, int32 col, int32 value);
		f32		GetHeight(f32 x, f32 z);
		bool	LoadTexture(std::wstring fileName);
		bool	GenTexture(const Vector3& lightDir);
		bool	Draw(Matrix worldMat, bool drawTris);

		bool	ReadRawFile(std::wstring fileName);
		bool	ComputeVertices();
		bool	ComputeIndices();
		bool	LightTerrain(const Vector3& lightDir);
		float	ComputeShadow(int32 cellRow, int32 cellCol, const Vector3& lightDir);

	private:
		std::vector<int32> m_heightmap;

		int32 m_numVertsPerRow;
		int32 m_numVertsPerCol;
		int32 m_cellSpacing;
		f32	  m_heightScale;

		int32 m_numCellsPerRow;
		int32 m_numCellsPerCol;
		int32 m_width;
		int32 m_depth;
		int32 m_numVertices;
		int32 m_numTriangles;

		CDXDevice9*				m_pDXDevice;
		LPDIRECT3DTEXTURE9		m_pTex;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER9	m_pIndexBuffer;
	};	
}
#endif