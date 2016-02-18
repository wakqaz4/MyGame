/************************************************************************
* Bounding box, not AABB     
* // [2016/1/29 11:03 Zhaoyu.Wang]  
************************************************************************/
#ifndef _GD_BOUNDING_BOX_H_
#define _GD_BOUNDING_BOX_H_
#pragma once

#include "../../../GDMath/GDVector.h"

namespace GD
{
	class CBoundingBox
	{
	public: 
		inline CBoundingBox() 
		{};
		~CBoundingBox()
		{};
	public:
		inline bool IsPointInside(Vector3 point)
		{
			if (point.x < m_min.x || point.y < m_min.y || point.z < m_min.z ||
				point.x > m_max.x || point.y > m_max.y || point.z > m_max.z)
			{
				return false;
			}
			return true;
		};
	public:
		Vector3 m_min;
		Vector3 m_max;
	};
}

#endif