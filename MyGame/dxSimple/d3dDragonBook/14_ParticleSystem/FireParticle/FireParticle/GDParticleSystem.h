/************************************************************************
* Base class of particle system, which can't be directly used. You must
* derivate a subClass from this, and override the ResetParticle method, 
* while Update method is also suggested to override.
* // [2016/2/1 12:21 Zhaoyu.Wang]  
************************************************************************/
#include <string.h>
#include "../../d3dUtility/d3dUtility/DXDevice9.h"
#include "../../../GDTypedef/GDTypedef.h"
#include "../../../GDMath/GDMath.h"
#include <list>

namespace GD
{
	struct SParticleVertex
	{
		Vector3 m_position;
		DWORD m_color;
		static const DWORD FVF;
	};
	struct SParticleAttrib
	{
		SParticleAttrib()
		{
			m_lifeTime = 0.0f;
			m_age = 0.0f;
			m_isAlive = true;
		}
		Vector3 m_position;
		Vector3 m_velocity;
		Vector3 m_acceleration;
		float m_lifeTime;
		float m_age;
		DWORD m_color;
		DWORD m_colorFade;
		bool m_isAlive;
	};
	class IParticleSystem
	{
	public:
		IParticleSystem(){};
		virtual ~IParticleSystem(){};

		virtual bool Init(CDXDevice9* pDevice, std::wstring textFileName) = 0;
		virtual void Reset() = 0;

		/**
		 *	Need to respawn dead particle instead of free & new over and over again,
		 *	which costs high.
		 */
		virtual void ResetParticle(SParticleAttrib* attribute) = 0;
		virtual void AddParticle() = 0;

		virtual void Update(float deltaTime) = 0;
		/**
		 *	Set renderState for pre render;
		 */
		virtual void PreRender() = 0;
		virtual void Render() = 0;
		virtual void PostRender() = 0;

		virtual bool IsEmpty() = 0;
		virtual bool IsDead() = 0;
		virtual void SetPosition(Vector3 pos) = 0;
		virtual Vector3& GetPosition() =0;
	protected:
		virtual void RemoveDeadParticles() = 0;
	};
	class CParticleSystem: public IParticleSystem
	{
	public:
		CParticleSystem();
		virtual ~CParticleSystem();

		virtual bool Init(CDXDevice9* pDevice, std::wstring textFileName) override;
		virtual void Reset() override;

		/**
		*	Need to respawn dead particle instead of free & new over and over again,
		*	which costs high.
		*/
		virtual void ResetParticle(SParticleAttrib* attribute) = 0;
		virtual void AddParticle() override;

		virtual void Update(float deltaTime) override;
		/**
		*	Set renderState for pre render;
		*/
		virtual void PreRender() override;
		virtual void Render() override;
		virtual void PostRender() override;

		virtual bool IsEmpty() override;
		virtual bool IsDead() override;
		virtual void SetPosition(Vector3 pos)override;
		virtual Vector3& GetPosition() override;
	protected:
		virtual void RemoveDeadParticles() override;
	
		SParticleAttrib m_attributes;
		CDXDevice9* m_pDXDevice;
		Vector3 m_origin;

		float m_emitRate;	//Rate new particles are added to system.
		float m_size;		//Size of particles.
		LPDIRECT3DTEXTURE9 m_pTex;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		std::list<SParticleAttrib> m_particles;
		int32					m_maxParticles;
		DWORD m_vertexBufferSize;
		DWORD m_vertexBufferOffset;
		DWORD m_vertexBufferBatchSize;
	};
}