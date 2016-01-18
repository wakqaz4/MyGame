/************************************************************************
* Implementation of vector4.
* Use this class for improve the efficiency of 
************************************************************************/
#ifndef _GD_VECTOR4_H_
#define _GD_VECTOR4_H_

#include <emmintrin.h>

namespace GD
{
	typedef __m128 vec4;

	template<typename F>
	class CVector4
	{
	public:

	public: 
		vec4 m_vec4;
	};
}

#endif
#undef _GD_VECTOR4_H_