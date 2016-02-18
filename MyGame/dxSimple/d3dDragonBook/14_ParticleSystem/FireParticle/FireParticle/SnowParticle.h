/************************************************************************
* Snow particle system.
* Should be used like this:
* CParticleSystem* snowParticle = new CSnowParticle(&pos, 6000);
* snowParticle->Init(pDXDevice, "particleImageName.bmp");
* snowParticle->Render();
* 
* // [2016/1/28 18:47 Zhaoyu.Wang]  
************************************************************************/

#include "GDParticleSystem.h"
#include "GDBoundingBox.h"

namespace GD
{
	class CSnowParticle: public CParticleSystem
	{
	public:
		CSnowParticle();
		CSnowParticle(CBoundingBox boundingBox, int32 numParticles);
		CSnowParticle(Vector3 posMin, Vector3 posMax, int32 numParticles);
		~CSnowParticle(){};

		void ResetParticle(SParticleAttrib* attribute) override;
		void Update(float deltaTime)override;
	private:
		CBoundingBox m_boundingBox;
	};
}