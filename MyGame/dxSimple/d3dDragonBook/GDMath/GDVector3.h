/************************************************************************
* Vector3.
* Here I implemented overLoad of operator + - * /, += -= *= /=, == =, 
* and Cross, Normali8ze.
* //  [1/7/2016 Zhaoyu.Wang]
************************************************************************/

#ifndef _GD_VECTOR3_H_
#define _GD_VECTOR3_H_
#pragma once

namespace GD
{
	const float VECTOR3_EPSILON = 0.001f;
	const double D_VECTOR3_EPSILON = 0.0001;
	
	template<typename F>
	class CVector3
	{
	public:
		inline CVector3<F>() :
			x(0),
			y(0),
			z(0){}
		inline CVector3<F>(F _x, F _y, F _z){ x = _x; y = _y; z = _z; }
		inline CVector3<F>(const CVector3<F>& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}

		inline CVector3<F> operator +(CVector3<F> vec)
		{
			return CVector3<F>(x + vec.x, y + vec.y, z + vec.z);
		}
		inline CVector3<F> operator -(void)
		{
			return CVector3<F>(-x, -y, -z);
		}
		inline CVector3<F> operator -(CVector3<F> vec)
		{
			return CVector3<F>(x - vec.x, y - vec.y, z - vec.z);
		}
		inline F operator *(CVector3<F> vec)
		{
			return (x*vec.x + y*vec.y + z*vec.z);
		}
		inline CVector3<F> operator *(F m)
		{
			return CVector3<F>(x*m, y*m, z*m);
		}
		inline CVector3<F> operator /(F m)
		{
			return CVector3<F>(x / m, y / m, z / m);
		}
		inline CVector3<F>& operator +=(CVector3<F> vec)
		{
			x += vec.x;
			y += vec.y;
			z += vec.z;
			return *this;
		}
		inline CVector3<F>& operator -=(CVector3<F> vec)
		{
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			return *this;
		}
		inline CVector3<F>& operator *=(F m)
		{
			x *= m;
			y *= m;
			z *= m;
			return *this;
		}
		inline CVector3<F>& operator /=(F d)
		{
			assert(d);
			this *= (F)(1.0f / d);
			return *this;
		}
		inline bool operator ==(CVector3<F> vec)
		{
			switch (sizeof (F))
			{
			case 4:
				return (fabsf(x - vec.x)<VECTOR3_EPSILON && fabsf(y - vec.y) < VECTOR3_EPSILON && fabsf(z - vec.z) < VECTOR3_EPSILON);
			case 8:
				return (fabs(x - vec.x)<D_VECTOR3_EPSILON && fabs(y - vec.y) < D_VECTOR3_EPSILON && fabs(z - vec.z) < D_VECTOR3_EPSILON);
			default:
				return true;
			}			
		}
		inline bool operator ==(F m)
		{
			switch (sizeof (F))
			{
			case 4:
				return (fabsf(x - m) < VECFOR3_EPSILON && fabsf(y - m) < VECFOR3_EPSILON && fabsf(z - m) < VECFOR3_EPSILON);
			case 8:
				return (fabs(x - m) < D_VECFOR3_EPSILON && fabs(y - m) < D_VECFOR3_EPSILON && fabs(z - m) < D_VECFOR3_EPSILON);
			default:
				return true;
			}
		}
		inline bool IsZero()
		{
			switch (sizeof (F))
			{
			case 4:
				return (fabsf(x) < VECTOR3_EPSILON && fabsf(y) < VECTOR3_EPSILON && fabsf(z) < VECTOR3_EPSILON);
			case 8:
				return (fabs(x) < D_VECTOR3_EPSILON && fabs(y) < D_VECTOR3_EPSILON && fabs(z) < D_VECTOR3_EPSILON);
			default:
				return true;
			}
		}
		inline CVector3<F>& operator = (CVector3<F> vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			return *this;
		}
		/**
		 *	Overload % as vector's cross method.
		 *	*/
		inline CVector3<F> operator %(CVector3<F> vec)
		{
			return CVector3<F>(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
		}
		inline CVector3<F> Cross(CVector3<F> vec)
		{
			return CVector3<F>(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
		}
		inline F GetSquareLength()
		{
			return x*x + y*y + z*z;
		}
		inline F GetLength()
		{
			return sqrt(GetSquareLength());
		}
		/**
		 *	Should this function return ?
		 *	*/
		inline void Normalize()
		{
			F len = GetLength();
			assert(len);
			x /= len;
			y /= len;
			z /= len;
		}
		/**
		 *	Maybe no need to have these methods...
		 *	*/
		inline void SetX(F _x){ x = _x; };
		inline void SetY(F _y){ y = _y; };
		inline void SetZ(F _z){ z = _z; };
		inline void Set(F _x, F _y, F _z){ x = _x; y = _y; z = _z; }
		
	public:
		F x;
		F y;
		F z;
	};
}

#endif