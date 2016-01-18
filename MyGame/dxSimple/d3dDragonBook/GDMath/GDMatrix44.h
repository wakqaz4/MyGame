/************************************************************************
* Implementation of matrix44, which will be set as default matrix in GDEngine.
* 
* Mainly includes: overload of operator + - * /, += -= *= /=, = ==, 
* And Identity, 
* 
* Other operations such as scaling, rotating, translating will be implemented 
* in its sub classes such as camera, as matrix itself will never rotate. 
* // [2016/1/7 17:42 Zhaoyu.Wang]  
* 
* I've made a mistake. Operations such as scale, rotate, translate should be part of 
* matrix, cause not only camera will use these. 
* Add RotateX, RotateY, RotateZ, Translate, Scale;
* // [2016/1/8 11:28 Zhaoyu.Wang]  
************************************************************************/
#ifndef _GD_MATRIX44_H_
#define _GD_MATRIX44_H_
#pragma once

#if defined _GD_VECTOR4_H_
#include <emmintrin.h>
#endif

namespace GD
{
	extern const float VECTOR3_EPSILON;
	template<typename F> class CMatrix33;
	template<typename F> class CMatrix34;
	template<typename F> class CVector3;
	template<typename F> class CVector4;

	const float MATRIX44_EPSILON = VECTOR3_EPSILON;
	/**
	 *	Here I used F instead of T because we only use f32 or f64 as matrix template mostly 
	 *	*/
	template <typename F>
	class CMatrix44
	{
		/**
		 *	Use SSE instruments to implement CMatrix44( based on CVector4),
		 *	which is strongly recommended.
		 *	*/
#ifdef _GD_VECTOR4_H_

#else
		/**
		 *	Use ordinary C++ to implement CMatrix
		 *	*/
	public:
		/**
		 *	In fact, I'm really confused here...
		 *		If the member data should be set 0 in constructor? Considering the huge amount of 
		 *		operations that will be done, there must be many matrices be constructed and destructed.
		 *		So it may be a good idea to do nothing in constructor in order to save instruments.
		 *		However, then in every operation, we'll have to check matrix's member validity, which 
		 *		seems to cost a lot higher.
		 *		So, at last, as you will see, I used the latter idea. 
		 *		However, still used a _DEBUG macro to wrap all validations, and also constructor. 
		 *	*/
		CMatrix44<F>()
		{
#ifdef _DEBUG
			m00 = 0; m01 = 0; m02 = 0; m03 = 0;
			m10 = 0; m11 = 0; m12 = 0; m13 = 0;
			m20 = 0; m21 = 0; m22 = 0; m23 = 0;
			m30 = 0; m31 = 0; m32 = 0; m33 = 0;
#endif
		};
		CMatrix44<F>(F m)
		{
			m00 = m; m01 = m; m02 = m; m03 = m;
			m10 = m; m11 = m; m12 = m; m13 = m;
			m20 = m; m21 = m; m22 = m; m23 = m;
			m30 = m; m31 = m; m32 = m; m33 = m;
		};
		CMatrix44<F>(CMatrix44<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = mat.m03;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = mat.m13;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = mat.m23;
			m30 = mat.m30; m31 = mat.m31; m32 = mat.m32; m33 = mat.m33;
		}
		CMatrix44<F>(CMatrix33<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = 0;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = 0;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = 0;
			m30 = 0;	   m31 = 0;       m32 = 0;		 m33 = 1;
		}
		CMatrix44<F>(CMatrix34<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = mat.m03;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = mat.m13;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = mat.m23;
			m30 = 0;	   m31 = 0;		  m32 = 0;		 m33 = 1;
		}

		inline CMatrix44<F>& operator +(const CMatrix44<F>& mat)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			CMatrix44<F> retMat(*this);
			retMat.m00 += mat.m00; retMat.m01 += mat.m01; retMat.m02 += mat.m02; retMat.m03 += mat.m03;
			retMat.m10 += mat.m10; retMat.m11 += mat.m11; retMat.m12 += mat.m12; retMat.m13 += mat.m13;
			retMat.m20 += mat.m20; retMat.m21 += mat.m21; retMat.m22 += mat.m22; retMat.m23 += mat.m23;
			retMat.m30 += mat.m30; retMat.m31 += mat.m31; retMat.m32 += mat.m32; retMat.m33 += mat.m33;

			return retMat;
		}
		inline CMatrix44<F>& operator +=(const CMatrix44<F>& mat)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			m00 += mat.m00; m01 += mat.m01; m02 += mat.m02; m03 += mat.m03;
			m10 += mat.m10; m11 += mat.m11; m12 += mat.m12; m13 += mat.m13;
			m20 += mat.m20; m21 += mat.m21; m22 += mat.m22; m23 += mat.m23;
			m30 += mat.m30; m31 += mat.m31; m32 += mat.m32; m33 += mat.m33;

			return *this;
		}
		inline CMatrix44<F>& operator -(const CMatrix44<F>& mat)
		{
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			CMatrix44<F> retMat(*this);
			retMat.m00 -= mat.m00; retMat.m01 -= mat.m01; retMat.m02 -= mat.m02; retMat.m03 -= mat.m03;
			retMat.m10 -= mat.m10; retMat.m11 -= mat.m11; retMat.m12 -= mat.m12; retMat.m13 -= mat.m13;
			retMat.m20 -= mat.m20; retMat.m21 -= mat.m21; retMat.m22 -= mat.m22; retMat.m23 -= mat.m23;
			retMat.m30 -= mat.m30; retMat.m31 -= mat.m31; retMat.m32 -= mat.m32; retMat.m33 -= mat.m33;

			return retMat;
		}
		inline CMatrix44<F>& operator -()
		{
			assert(IsValidNumber(this->m00));

			m00 = -m00; m01 = -m01; m02 = -m02; m03 = -m03;
			m10 = -m10; m11 = -m11; m12 = -m12; m13 = -m13;
			m20 = -m20; m21 = -m21; m22 = -m22; m23 = -m23;
			m30 = -m30; m31 = -m31; m32 = -m32; m33 = -m33;

			return *this;
		}
		inline CMatrix44<F>& operator -=(const CMatrix44<F>& mat)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));
			
			m00 -= mat.m00; m01 -= mat.m01; m02 -= mat.m02; m03 -= mat.m03;
			m10 -= mat.m10; m11 -= mat.m11; m12 -= mat.m12; m13 -= mat.m13;
			m20 -= mat.m20; m21 -= mat.m21; m22 -= mat.m22; m23 -= mat.m23;
			m30 -= mat.m30; m31 -= mat.m31; m32 -= mat.m32; m33 -= mat.m33;

			return *this;
		}
		inline CMatrix44<F> operator *(const CMatrix44<F>& mat)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			/**
			 *	There must be several better ways...
			 *	*/
			CMatrix44<F> retMat(*this);
			retMat.m00 = this->m00*mat.m00 + this->m01*mat.m10 + this->m02*mat.m20 + this->m03*mat.m30; 
			retMat.m01 = this->m00*mat.m01 + this->m01*mat.m11 + this->m02*mat.m21 + this->m03*mat.m31;
			retMat.m02 = this->m00*mat.m02 + this->m01*mat.m12 + this->m02*mat.m22 + this->m03*mat.m32;
			retMat.m03 = this->m00*mat.m03 + this->m01*mat.m13 + this->m02*mat.m23 + this->m03*mat.m33;

			retMat.m10 = this->m10*mat.m00 + this->m11*mat.m10 + this->m12*mat.m20 + this->m13*mat.m30;
			retMat.m11 = this->m10*mat.m01 + this->m11*mat.m11 + this->m12*mat.m21 + this->m13*mat.m31;
			retMat.m12 = this->m10*mat.m02 + this->m11*mat.m12 + this->m12*mat.m22 + this->m13*mat.m32;
			retMat.m13 = this->m10*mat.m03 + this->m11*mat.m13 + this->m12*mat.m23 + this->m13*mat.m33;

			retMat.m20 = this->m20*mat.m00 + this->m21*mat.m10 + this->m22*mat.m20 + this->m23*mat.m30;
			retMat.m21 = this->m20*mat.m01 + this->m21*mat.m11 + this->m22*mat.m21 + this->m23*mat.m31;
			retMat.m22 = this->m20*mat.m02 + this->m21*mat.m12 + this->m22*mat.m22 + this->m23*mat.m32;
			retMat.m23 = this->m20*mat.m03 + this->m21*mat.m13 + this->m22*mat.m23 + this->m23*mat.m33;

			retMat.m30 = this->m30*mat.m00 + this->m31*mat.m10 + this->m32*mat.m20 + this->m33*mat.m30;
			retMat.m31 = this->m30*mat.m01 + this->m31*mat.m11 + this->m32*mat.m21 + this->m33*mat.m31;
			retMat.m32 = this->m30*mat.m02 + this->m31*mat.m12 + this->m32*mat.m22 + this->m33*mat.m32;
			retMat.m33 = this->m30*mat.m03 + this->m31*mat.m13 + this->m32*mat.m23 + this->m33*mat.m33;
			
			return retMat;
		}
		inline CMatrix44<F>& operator *=(const CMatrix44<F>& mat)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			/**
			*	There must be several better ways...
			*	*/
			CMatrix44<F> retMat(*this);
			retMat.m00 = this->m00*mat.m00 + this->m01*mat.m10 + this->m02*mat.m20 + this->m03*mat.m30;
			retMat.m01 = this->m00*mat.m01 + this->m01*mat.m11 + this->m02*mat.m21 + this->m03*mat.m31;
			retMat.m02 = this->m00*mat.m02 + this->m01*mat.m12 + this->m02*mat.m22 + this->m03*mat.m32;
			retMat.m03 = this->m00*mat.m03 + this->m01*mat.m13 + this->m02*mat.m23 + this->m03*mat.m33;

			retMat.m10 = this->m10*mat.m00 + this->m11*mat.m10 + this->m12*mat.m20 + this->m13*mat.m30;
			retMat.m11 = this->m10*mat.m01 + this->m11*mat.m11 + this->m12*mat.m21 + this->m13*mat.m31;
			retMat.m12 = this->m10*mat.m02 + this->m11*mat.m12 + this->m12*mat.m22 + this->m13*mat.m32;
			retMat.m13 = this->m10*mat.m03 + this->m11*mat.m13 + this->m12*mat.m23 + this->m13*mat.m33;

			retMat.m20 = this->m20*mat.m00 + this->m21*mat.m10 + this->m22*mat.m20 + this->m23*mat.m30;
			retMat.m21 = this->m20*mat.m01 + this->m21*mat.m11 + this->m22*mat.m21 + this->m23*mat.m31;
			retMat.m22 = this->m20*mat.m02 + this->m21*mat.m12 + this->m22*mat.m22 + this->m23*mat.m32;
			retMat.m23 = this->m20*mat.m03 + this->m21*mat.m13 + this->m22*mat.m23 + this->m23*mat.m33;

			retMat.m30 = this->m30*mat.m00 + this->m31*mat.m10 + this->m32*mat.m20 + this->m33*mat.m30;
			retMat.m31 = this->m30*mat.m01 + this->m31*mat.m11 + this->m32*mat.m21 + this->m33*mat.m31;
			retMat.m32 = this->m30*mat.m02 + this->m31*mat.m12 + this->m32*mat.m22 + this->m33*mat.m32;
			retMat.m33 = this->m30*mat.m03 + this->m31*mat.m13 + this->m32*mat.m23 + this->m33*mat.m33;

			*this = retMat;
			return *this;
		}
		inline CMatrix44<F>& operator *(const F& m)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));

			CMatrix44<F> retMat(*this);
			{
				int count = 0;
				for (F* iter = &retMat.m00; count < 16; count++)
				{
					*iter = (*iter) * m;
					iter++;
				}
			}
			return retMat;
		}
		inline CMatrix44<F>& operator *=(const F& m)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));
				
			int count = 0;
			for (F* iter = &m00; count < 16; count++)
			{
				*iter = (*iter) * m;
				iter++;
			}
				
			return *this;
		}
		inline CMatrix44<F>& operator /(const F& m)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));
			assert(m);

			CMatrix44<F> retMat(*this);
			{
				int count = 0;
				for (F* iter = &retMat.m00; count < 16; count++)
				{
					*iter = (*iter)/m;
					iter++;
				}
			}
			
			return retMat;
		}
		inline CMatrix44<F>& operator /=(const F& m)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));
			assert(m);

			int count = 0;
			for (F* iter = &m00; count < 16; count++)
			{
				*iter = (*iter) / m;
				iter++;
			}

			return retMat;
		}
		inline CMatrix44<F>& operator =(const CMatrix44<F>& mat)
		{
			assert(IsValidNumber(mat.m00));

			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = mat.m03;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = mat.m13;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = mat.m23;
			m30 = mat.m30; m31 = mat.m31; m32 = mat.m32; m33 = mat.m33;
			return (*this);
		}
		inline bool operator ==(const CMatrix44<F>& mat)
		{
			assert(IsValidNumber(mat.m00));
			assert(IsValidNumber(this->m00));

			const F e = (F)MATRIX44_EPSILON;
			if (sizeof(F) == 4)
			{
				return ((fabsf(m00 - mat.m00) < e) && (fabsf(m01 - mat.m01) < e) && (fabsf(m02 - mat.m02) < e) && (fabsf(m03 - mat.m03) < e) &&
					(fabsf(m10 - mat.m10) < e) && (fabsf(m11 - mat.m11) < e) && (fabsf(m12 - mat.m12) < e) && (fabsf(m13 - mat.m13) < e) &&
					(fabsf(m20 - mat.m20) < e) && (fabsf(m21 - mat.m21) < e) && (fabsf(m22 - mat.m22) < e) && (fabsf(m23 - mat.m23) < e) &&
					(fabsf(m30 - mat.m30) < e) && (fabsf(m31 - mat.m31) < e) && (fabsf(m32 - mat.m32) < e) && (fabsf(m33 - mat.m33) < e) 
					)
					
			}
			else if (sizeof(F) == 8)
			{
				return ((fabs(m00 - mat.m00) < e) && (fabs(m01 - mat.m01) < e) && (fabs(m02 - mat.m02) < e) && (fabsf(m03 - mat.m03) < e) &&
					(fabs(m10 - mat.m10) < e) && (fabs(m11 - mat.m11) < e) && (fabs(m12 - mat.m12) < e) && (fabs(m13 - mat.m13) < e) &&
					(fabs(m20 - mat.m20) < e) && (fabs(m21 - mat.m21) < e) && (fabs(m22 - mat.m22) < e) && (fabs(m23 - mat.m23) < e) &&
					(fabs(m30 - mat.m30) < e) && (fabs(m31 - mat.m31) < e) && (fabs(m32 - mat.m32) < e) && (fabs(m33 - mat.m33) < e)
					)
			}
			else
			{
				/**
				 *	Not a validate type
				 *	*/
				return true;
			}
		}

		inline void Identity()
		{
			m00 = (F)1.0f; m01 = (F)0.0f; m02 = (F)0.0f; m03 = (F)0.0f;
			m10 = (F)0.0f; m11 = (F)1.0f; m12 = (F)0.0f; m13 = (F)0.0f;
			m20 = (F)0.0f; m21 = (F)0.0f; m22 = (F)1.0f; m23 = (F)0.0f;
			m30 = (F)0.0f; m31 = (F)0.0f; m32 = (F)0.0f; m33 = (F)1.0f;
		}

		//Use DEG2RAD to cast to rad		
		void RotateX(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix44<F> mat;
			mat.Identity();
			//...							//...
				mat.m11 = c;	mat.m12 = -s;
				mat.m21 = s;	mat.m22 = c;
			//...							//...
			(*this) = mat*(*this);
		}
		
		void RotateY(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix44<F> mat;
			mat.Identity();
			//...								//...
			mat.m00 = c;	mat.m02 = s;
			mat.m20 = -s;	mat.m22 = c;
			//...								//...
			(*this) = mat*(*this);
		}

		void RotateZ(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix44<F> mat;
			mat.Identity();
			//...								//...
			mat.m00 = c;	mat.m01 = -s;
			mat.m10 = s;	mat.m11 = c;
			//...								//...
			(*this) = mat*(*this);
		}
		
		void Scale(F scale)
		{
			m00 *= scale; m01 *= scale; m02 *= scale;
			m10 *= scale; m11 *= scale; m12 *= scale;
			m20 *= scale; m21 *= scale; m22 *= scale;
		}
		
		void Scale(F sx, F sy, F sz)
		{
			int i = 0;
			/**
			*	I'm a little unsure about this section:
			*	Are variables of one class always constant in memory? With no exception?
			*/
			//#ifdef _DEBUG
			//	for (F* iter = &m00; i < 16; i++)
			//	{
			//		(*iter) *= sx;
			//		iter++;
			//		(*iter) *= sy;
			//		iter++;
			//		(*iter) *= sz;
			//		iter++;
			//		iter++;
			//	}
			//#else
			m00 *= sx; m01 *= sy; m02 *= sz;
			m10 *= sx; m11 *= sy; m12 *= sz;
			m20 *= sx; m21 *= sy; m22 *= sz;
			//#endif
		}

		void Translate(F x, F y, F z)
		{
			m03 += x;
			m13 += y;
			m23 += z;
		}
		
		void Translate(const CVector3<F>& vec)
		{
			m03 += vec.x;
			m13 += vec.y;
			m23 += vec.z;
		}
		
		void ScaleTranslation(F scale)
		{
			m03 *= scale;
			m13 *= scale;
			m23 *= scale;
		}
		CMatrix44<F> GetFliped()
		{
			CMatrix44<F> retMat;
			retMat.m00 = m00; retMat.m01 = m10; retMat.m02 = m20; retMat.m03 = m30;
			retMat.m10 = m01; retMat.m11 = m11; retMat.m12 = m21; retMat.m13 = m31;
			retMat.m20 = m02; retMat.m21 = m12; retMat.m22 = m22; retMat.m23 = m32;
			retMat.m30 = m03; retMat.m31 = m13; retMat.m32 = m23; retMat.m33 = m33;
			return retMat;
		}

	public:
		F m00, m01, m02, m03;
		F m10, m11, m12, m13;
		F m20, m21, m22, m23;
		F m30, m31, m32, m33;
	};
#endif
}

#endif