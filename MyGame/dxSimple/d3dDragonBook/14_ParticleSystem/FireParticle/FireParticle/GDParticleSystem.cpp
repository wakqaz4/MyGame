#include "GDParticleSystem.h"
#include "../../../GDMacro/GDMacro.h"
#include <time.h>
using namespace GD;

const GD::DWORD SParticleVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

CParticleSystem::CParticleSystem()
{

}

CParticleSystem::~CParticleSystem()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pTex);
}

bool CParticleSystem::Init(CDXDevice9* pDevice, std::wstring textFileName)
{
	m_pDXDevice = pDevice;
	HRESULT hr = 0;
	hr = pDevice->GetD3DDevice()->CreateVertexBuffer(
		m_vertexBufferSize*sizeof(SParticleVertex),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		SParticleVertex::FVF,
		D3DPOOL_DEFAULT,
		&m_pVertexBuffer,
		0);
	if (FAILED(hr))
	{
		::MessageBoxA(nullptr, "CreateParticleVertexBuffer()-Failed", "Error", MB_OK);
		return false;
	}
	
	hr = D3DXCreateTextureFromFile(
		pDevice->GetD3DDevice(),
		textFileName.c_str(),
		&m_pTex);

	if (FAILED(hr))
	{
		::MessageBoxA(nullptr, "CreateParticleTexture()-Failed", "Error", MB_OK);
		return false;
	}

	return true;
}

void CParticleSystem::Reset()
{	
	for(auto i : m_particles)
	{
		ResetParticle(&i);
	}
}

void CParticleSystem::AddParticle()
{
	SParticleAttrib attrib;
	ResetParticle(&attrib);
	m_particles.push_back(attrib);
}


void CParticleSystem::PreRender()
{
	float f1 = 0.0f;
	float f2 = 1.0f;
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, false);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSIZE, *(DWORD*)(&m_size));
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSIZE_MIN, *(DWORD*)(&f1));

	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSCALE_A, *(DWORD*)(&f1));
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSCALE_B, *(DWORD*)(&f1));
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSCALE_C, *(DWORD*)(&f2));

	m_pDXDevice->GetD3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDXDevice->GetD3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

}

void CParticleSystem::PostRender()
{
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, true);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pDXDevice->GetD3DDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void CParticleSystem::Render()
{
	if (m_particles.empty())
	{
		return;
	}
	
	PreRender();
	m_pDXDevice->GetD3DDevice()->SetTexture(0, m_pTex);
	m_pDXDevice->GetD3DDevice()->SetFVF(SParticleVertex::FVF);
	m_pDXDevice->GetD3DDevice()->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SParticleVertex));

	if (m_vertexBufferOffset >= m_vertexBufferSize)
	{
		m_vertexBufferOffset = 0;
	}
	SParticleVertex* pVertex = 0;
	m_pVertexBuffer->Lock(
		m_vertexBufferOffset*sizeof(SParticleVertex),
		m_vertexBufferBatchSize*sizeof(SParticleVertex),
		(void**)&pVertex,
		m_vertexBufferOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
	DWORD numParticlesInBatch = 0;
	for (auto i : m_particles)
	{
		if (i.m_isAlive)
		{
			pVertex->m_position = i.m_position;
			pVertex->m_color = i.m_color;
			pVertex++;
			numParticlesInBatch++;

			/**
			 *	batch is full
			 */
			if (numParticlesInBatch == m_vertexBufferBatchSize)
			{
				m_pVertexBuffer->Unlock();
				m_pDXDevice->GetD3DDevice()->DrawPrimitive(
					D3DPT_POINTLIST,
					m_vertexBufferOffset,
					m_vertexBufferBatchSize);

				m_vertexBufferOffset += m_vertexBufferBatchSize;
				if (m_vertexBufferOffset >= m_vertexBufferSize)
				{
					m_vertexBufferOffset = 0;
				}
				m_pVertexBuffer->Lock(
					m_vertexBufferOffset*sizeof(SParticleVertex),
					m_vertexBufferBatchSize*sizeof(SParticleVertex),
					(void**)&pVertex,
					m_vertexBufferOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
				numParticlesInBatch = 0;
			}
		}
	}
	m_pVertexBuffer->Unlock();

	if (numParticlesInBatch)
	{
		m_pDXDevice->GetD3DDevice()->DrawPrimitive(
			D3DPT_POINTLIST,
			m_vertexBufferOffset,
			numParticlesInBatch);
	}
	m_vertexBufferOffset += m_vertexBufferBatchSize;

	PostRender();
}

bool CParticleSystem::IsEmpty()
{
	return m_particles.empty();
}

bool CParticleSystem::IsDead()
{
	for (auto i : m_particles)
	{
		if (i.m_isAlive)
		{
			return false;
		}
	}
	return true;
}

void CParticleSystem::RemoveDeadParticles()
{
	std::list<SParticleAttrib>::iterator i;
	for (i = m_particles.begin(); i != m_particles.end();)
	{
		if (i->m_isAlive)
		{
			i = m_particles.erase(i);
		}
		else
		{
			i++;
		}
	}
}


void CParticleSystem::Update(float deltaTime)
{
	for (auto i : m_particles)
	{
		if (i.m_isAlive)
		{
			i.m_position += i.m_velocity*deltaTime/1000.0f;
			i.m_age += deltaTime;
			if (i.m_age > i.m_lifeTime)
			{
				i.m_isAlive = false;
			}
		}
	}
}

void CParticleSystem::SetPosition(Vector3 pos)
{
	m_origin = pos;
}

Vector3& CParticleSystem::GetPosition()
{
	return m_origin;
}