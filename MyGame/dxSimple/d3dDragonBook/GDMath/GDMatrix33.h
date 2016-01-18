/************************************************************************
* Define matrix33 for easier usage of matrix34;
* 
* Mainly includes overload of  + - * /, += -= *= /=, == =, 
* Identity, Rotation, Scalation.
* 
* However, matrix33 can't be speeded with SIMD(As long as all I knew...).
* But honestly, I think it's just that I don't know how to use SIMD...
* Will later optimize with SIMD if possible.
* // [2016/1/8 15:16 Zhaoyu.Wang]  
************************************************************************/

#ifndef _GD_MATRIX33_H_
#define _GD_MATRIX33_H_
#pragma once

#include <assert.h>

namespace GD
{
	extern const float VECTOR3_EPSILON;
	template<typename F> class CMatrix44;
	template<typename F> class CMatrix34;
	template<typename F> class CVector3;

	const float MATRIX33_EPSILON = VECTOR3_EPSILON;
	template<typename F>
	class CMatrix33
	{
#if defined _SIMD_


#else
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
		inline CMatrix33<F>()
		{
#ifdef _DEBUG
			m00 = 0; m01 = 0; m02 = 0; 
			m10 = 0; m11 = 0; m12 = 0; 
			m20 = 0; m21 = 0; m22 = 0; 
#endif
		};
		inline CMatrix33<F>(F m)
		{
			m00 = m; m01 = m; m02 = m;
			m10 = m; m11 = m; m12 = m;
			m20 = m; m21 = m; m22 = m;
		};
		inline CMatrix33<F>(CMatrix33<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; 
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; 
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; 
		}
		inline CMatrix33<F>(CMatrix34<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; 
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; 
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; 			
		}
		inline CMatrix33<F>(CMatrix44<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; 
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; 
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; 			
		}

		inline CMatrix33<F>& operator +(const CMatrix33<F>& mat)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));
#endif
			CMatrix44<F> retMat(*this);
			retMat.m00 += mat.m00; retMat.m01 += mat.m01; retMat.m02 += mat.m02; 
			retMat.m10 += mat.m10; retMat.m11 += mat.m11; retMat.m12 += mat.m12; 
			retMat.m20 += mat.m20; retMat.m21 += mat.m21; retMat.m22 += mat.m22; 
			return retMat;
		}
		inline CMatrix33<F>& operator +=(const CMatrix33<F>& mat)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));
#endif
			m00 += mat.m00; m01 += mat.m01; m02 += mat.m02; 
			m10 += mat.m10; m11 += mat.m11; m12 += mat.m12; 
			m20 += mat.m20; m21 += mat.m21; m22 += mat.m22; 
			return *this;
		}
		inline CMatrix33<F>& operator -(const CMatrix33<F>& mat)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));
#endif
			CMatrix44<F> retMat(*this);
			retMat.m00 -= mat.m00; retMat.m01 -= mat.m01; retMat.m02 -= mat.m02; 
			retMat.m10 -= mat.m10; retMat.m11 -= mat.m11; retMat.m12 -= mat.m12; 
			retMat.m20 -= mat.m20; retMat.m21 -= mat.m21; retMat.m22 -= mat.m22; 
			return retMat;
		}
		inline CMatrix33<F>& operator -()
		{
			assert(IsValidNumber(this->m00));

			m00 = -m00; m01 = -m01; m02  = - m02; 
			m10 = -m10; m11 = -m11; m12  = - m12; 
			m20 = -m20; m21 = -m21; m22  = - m22; 

			return *this;
		}
		inline CMatrix33<F>& operator -=(const CMatrix33<F>& mat)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));
#endif			
			m00 -= mat.m00; m01 -= mat.m01; m02 -= mat.m02; 
			m10 -= mat.m10; m11 -= mat.m11; m12 -= mat.m12; 
			m20 -= mat.m20; m21 -= mat.m21; m22 -= mat.m22; 
			return *this;
		}
		inline CMatrix33<F>& operator *(const CMatrix33<F>& mat)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));
#endif
			/**
			*	There must be several better ways...
			*	*/
			CMatrix33<F> retMat(*this);
			retMat.m00 = this->m00*mat.m00 + this->m01*mat.m10 + this->m02*mat.m20;
			retMat.m01 = this->m00*mat.m01 + this->m01*mat.m11 + this->m02*mat.m21;
			retMat.m02 = this->m00*mat.m02 + this->m01*mat.m12 + this->m02*mat.m22			

			retMat.m10 = this->m10*mat.m00 + this->m11*mat.m10 + this->m12*mat.m20;
			retMat.m11 = this->m10*mat.m01 + this->m11*mat.m11 + this->m12*mat.m21;
			retMat.m12 = this->m10*mat.m02 + this->m11*mat.m12 + this->m12*mat.m22;			

			retMat.m20 = this->m20*mat.m00 + this->m21*mat.m10 + this->m22*mat.m20;
			retMat.m21 = this->m20*mat.m01 + this->m21*mat.m11 + this->m22*mat.m21;
			retMat.m22 = this->m20*mat.m02 + this->m21*mat.m12 + this->m22*mat.m22;			
			
			return retMat;
		}
		inline CMatrix34<F> operator *(const CMatrix34<F>& mat)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));
#endif
			/**
			*	There must be several better ways...
			*	*/
			CMatrix34<F> retMat(*this);
			retMat.m00 = this->m00*mat.m00 + this->m01*mat.m10 + this->m02*mat.m20;
			retMat.m01 = this->m00*mat.m01 + this->m01*mat.m11 + this->m02*mat.m21;
			retMat.m02 = this->m00*mat.m02 + this->m01*mat.m12 + this->m02*mat.m22;
			retMat.m03 = this->m00*mat.m03 + this->m01*mat.m13 + this->m02*mat.m23;

			retMat.m10 = this->m10*mat.m00 + this->m11*mat.m10 + this->m12*mat.m20;
			retMat.m11 = this->m10*mat.m01 + this->m11*mat.m11 + this->m12*mat.m21;
			retMat.m12 = this->m10*mat.m02 + this->m11*mat.m12 + this->m12*mat.m22;
			retMat.m13 = this->m10*mat.m03 + this->m11*mat.m13 + this->m12*mat.m23;

			retMat.m20 = this->m20*mat.m00 + this->m21*mat.m10 + this->m22*mat.m20;
			retMat.m21 = this->m20*mat.m01 + this->m21*mat.m11 + this->m22*mat.m21;
			retMat.m22 = this->m20*mat.m02 + this->m21*mat.m12 + this->m22*mat.m22;
			retMat.m23 = this->m20*mat.m03 + this->m21*mat.m13 + this->m22*mat.m23;

			return retMat;
		}


		inline CMatrix33<F>& operator *=(const CMatrix33<F>& mat)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));
#endif
			/**
			*	There must be several better ways...
			*	*/
			CMatrix33<F> retMat(*this);
			retMat.m00 = this->m00*mat.m00 + this->m01*mat.m10 + this->m02*mat.m20;
			retMat.m01 = this->m00*mat.m01 + this->m01*mat.m11 + this->m02*mat.m21;
			retMat.m02 = this->m00*mat.m02 + this->m01*mat.m12 + this->m02*mat.m22;			

			retMat.m10 = this->m10*mat.m00 + this->m11*mat.m10 + this->m12*mat.m20;
			retMat.m11 = this->m10*mat.m01 + this->m11*mat.m11 + this->m12*mat.m21;
			retMat.m12 = this->m10*mat.m02 + this->m11*mat.m12 + this->m12*mat.m22;			

			retMat.m20 = this->m20*mat.m00 + this->m21*mat.m10 + this->m22*mat.m20;
			retMat.m21 = this->m20*mat.m01 + this->m21*mat.m11 + this->m22*mat.m21;
			retMat.m22 = this->m20*mat.m02 + this->m21*mat.m12 + this->m22*mat.m22;			

			*this = retMat;
			return *this;
		}
		inline CMatrix33<F>& operator *(const F& m)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));
#endif
			CMatrix33<F> retMat(*this);
			{
				int count = 0;
				for (F* iter = &retMat.m00; count < 9; count++)
				{
					*iter *= m;
					iter++;
				}
			}
			return retMat;
		}
		inline CMatrix33<F>& operator *=(const F& m)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));
#endif					
			int count = 0;
			for (F* iter = &m00; count < 9; count++)
			{
				*iter *= m;
				iter++;
			}
			return *this;
		}
		inline CMatrix33<F>& operator /(const F& m)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));
			assert(m);
#endif
			CMatrix33<F> retMat(*this);
			{
				int count = 0;
				for (F* iter = &retMat.m00; count < 9; count++)
				{
					*iter = (*iter) / m;
					iter++;
				}
			}

			return retMat;
		}
		inline CMatrix33<F>& operator /=(const F& m)
		{
#ifdef _DEBUG
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));
			assert(m);
#endif
			int count = 0;
			for (F* iter = &m00; count < 9; count++)
			{
				*iter = (*iter) / m;
				iter++;
			}

			return retMat;
		}
		inline CMatrix33<F>& operator =(const CMatrix33<F>& mat)
		{
			assert(IsValidNumber(mat.m00));

			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22;			
		}
		inline bool operator ==(const CMatrix33<F>& mat)
		{
			assert(IsValidNumber(mat.m00));
			assert(IsValidNumber(this->m00));

			const F e = (F)MATRIX33_EPSILON;
			if (sizeof(F) == 4)
			{
				return ((fabsf(m00 - mat.m00) < e) && (fabsf(m01 - mat.m01) < e) && (fabsf(m02 - mat.m02) < e) &&
					(fabsf(m10 - mat.m10) < e) && (fabsf(m11 - mat.m11) < e) && (fabsf(m12 - mat.m12) < e) && 
					(fabsf(m20 - mat.m20) < e) && (fabsf(m21 - mat.m21) < e) && (fabsf(m22 - mat.m22) < e)  
					)

			}
			else if (sizeof(F) == 8)
			{
				return ((fabs(m00 - mat.m00) < e) && (fabs(m01 - mat.m01) < e) && (fabs(m02 - mat.m02) < e) &&
					(fabs(m10 - mat.m10) < e) && (fabs(m11 - mat.m11) < e) && (fabs(m12 - mat.m12) < e) &&
					(fabs(m20 - mat.m20) < e) && (fabs(m21 - mat.m21) < e) && (fabs(m22 - mat.m22) < e) 					
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
			m00 = (F)1.0f; m01 = (F)0.0f; m02 = (F)0.0f; 
			m10 = (F)0.0f; m11 = (F)1.0f; m12 = (F)0.0f; 
			m20 = (F)0.0f; m21 = (F)0.0f; m22 = (F)1.0f; 
		}
		
		void RotateX(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix33<F> mat;
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
			CMatrix33<F> mat;
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
			CMatrix33<F> mat;
			mat.Identity();
			//...								//...
			mat.m00 = c;	mat.m01 = -s;
			mat.m10 = s;	mat.m11 = c;
			//...								//...
			(*this) = mat*(*this);
		}
		
		void Scale(F sx, F sy, F sz)
		{

			/**
			*	I'm a little unsure about this section:
			*	Are variables of one class always constant in memory? With no exception?
			*/

			//int i = 0;
			//for (F* iter = &m00; i < 16; i++)
			//{
			//	(*iter) *= sx;
			//	iter++;
			//	(*iter) *= sy;
			//	iter++;
			//	(*iter) *= sz;
			//	iter++;
			//	iter++;
			//}

			m00 *= sx; m01 *= sy; m02 *= sz;
			m10 *= sx; m11 *= sy; m12 *= sz;
			m20 *= sx; m21 *= sy; m22 *= sz;

		}

		void Scale(F scale)
		{
			m00 *= scale; m01 *= scale; m02 *= scale;
			m10 *= scale; m11 *= scale; m12 *= scale;
			m20 *= scale; m21 *= scale; m22 *= scale;
		}

		void Flip()
		{
#define SWAP(x,y,var) {var = x;x = y;y = temp;}
			F var = 0;
			SWAP(m01, m10, var);
			SWAP(m02, m20, var);
			SWAP(m12, m21, var);			
		}

	public:
		F m00, m01, m02;
		F m10, m11, m12;
		F m20, m21, m22;

#endif
	};
}

#endif