
#include "../../../GDMacro/GDMacro.h"
#include "SnowParticle.h"
#include <time.h>
using namespace GD;

CSnowParticle::CSnowParticle()
{
	srand((unsigned int)time(0));
	m_boundingBox.m_min = Vector3(0.0f, 0.0f, 0.0f);
	m_boundingBox.m_max = Vector3(2.0f, 2.0f, 2.0f);
	m_size = 0.25f;
	m_vertexBufferSize = 2048;
	m_vertexBufferOffset = 0;
	m_vertexBufferBatchSize = 512;
	for (int32 i = 0; i < 500; i++)
	{
		AddParticle();
	}

};
CSnowParticle::CSnowParticle(CBoundingBox boundingBox, int32 numParticles)
{
	srand((unsigned int)time(0));
	m_boundingBox = boundingBox;
	m_size = 0.25f;
	m_vertexBufferSize = 2048;
	m_vertexBufferOffset = 0;
	m_vertexBufferBatchSize = 512;
	for (int32 i = 0; i < numParticles; i++)
	{
		AddParticle();
	}
}
CSnowParticle::CSnowParticle(Vector3 posMin, Vector3 posMax, int32 numParticles)
{
	srand((unsigned int)time(0));
	m_boundingBox.m_min = posMin;
	m_boundingBox.m_max = posMax;
	m_size = 0.25f;
	m_vertexBufferSize = 2048;
	m_vertexBufferOffset = 0;
	m_vertexBufferBatchSize = 512;
	for (int32 i = 0; i < numParticles; i++)
	{
		AddParticle();
	}
}

/**
 *	Different kinds of particle systems has their own reset methods.
 *	For example, snow particle should be respawned in an AABB, and 
 *	so does fire particle, while particle gun in a sphere.
 */
void CSnowParticle::ResetParticle(SParticleAttrib* attribute)
{
	GetRandomVector(attribute->m_position, m_boundingBox.m_min, m_boundingBox.m_max);
	attribute->m_position.y = m_boundingBox.m_max.y;
	attribute->m_color = D3DCOLOR_WHITE;
	attribute->m_velocity = Vector3(GetRandomFloat(0.0f, 1.0f)*3.0f, -GetRandomFloat(0.0f, 1.0f)*6.0f, 0.0f);
}

void CSnowParticle::Update(float deltaTime)
{
	std::list<SParticleAttrib>::iterator i;
	for (i = m_particles.begin(); i != m_particles.end(); i++)
	{
		(*i).m_position += (*i).m_velocity*deltaTime / 1000.0f;
		if (m_boundingBox.IsPointInside((*i).m_position) == false)
		{
			ResetParticle(&(*i));
		}
	}
}

