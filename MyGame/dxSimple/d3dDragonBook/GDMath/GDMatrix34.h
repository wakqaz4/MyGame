/************************************************************************
* Use CMatrix34 instead of CMatirx44 to save some space for animation.
* And also for easier usage of quat;
* Mainly includes + - * /, += -= *= /=, == =, Identity, Rotate, Scale, 
* Translate.
* // [2016/1/8 14:07 Zhaoyu.Wang]  
* Add GetTranslation, SetTranslation; 
* Add SetRotationX, SetRotationY SetRotationZ, SetRotationXYZ;
* // [2016/1/9 15:23 Zhaoyu.Wang]  
************************************************************************/

#ifndef _GD_MATRIX34_H_
#define _GD_MATRIX34_H_

#include <assert.h>

namespace GD
{
	extern const float VECTOR3_EPSILON;
	template<typename F> class CMatrix33;
	template<typename F> class CMatrix44;
	template<typename F> class CVector3;
	template<typename F> class CVector4;

	/**
	 *	Use default type float, as __m128 suits better with float[4]
	 *	And I don't know how to use __m128 with template type double...
	 *	*/
	template<typename F = float>
	class CMatrix34
	{
#if defined _GD_VECTOR4_H_

#else
	public:
		CMatrix34<F>()
		{
#ifdef _DEBUG
			m00 = 0; m01 = 0; m02 = 0; m03 = 0;
			m10 = 0; m11 = 0; m12 = 0; m13 = 0;
			m20 = 0; m21 = 0; m22 = 0; m23 = 0;			
#endif
		};
		CMatrix34<F>(F m)
		{
			m00 = m; m01 = m; m02 = m; m03 = m;
			m10 = m; m11 = m; m12 = m; m13 = m;
			m20 = m; m21 = m; m22 = m; m23 = m;			
		};
		CMatrix34<F>(CMatrix34<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = mat.m03;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = mat.m13;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = mat.m23;			
		}
		CMatrix34<F>(CMatrix33<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = 0;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = 0;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = 0;
		}
		CMatrix34<F>(CMatrix44<F>& mat)
		{
			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = mat.m03;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = mat.m13;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = mat.m23;			
		}

		CMatrix34<F>& operator +(const CMatrix34<F>& mat)
		{
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			CMatrix34<F> retMat(*this);
			retMat.m00 += mat.m00; retMat.m01 += mat.m01; retMat.m02 += mat.m02; retMat.m03 += mat.m03;
			retMat.m10 += mat.m10; retMat.m11 += mat.m11; retMat.m12 += mat.m12; retMat.m13 += mat.m13;
			retMat.m20 += mat.m20; retMat.m21 += mat.m21; retMat.m22 += mat.m22; retMat.m23 += mat.m23;			

			return retMat;
		}
		CMatrix34<F>& operator +=(const CMatrix34<F>& mat)
		{
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			m00 += mat.m00; m01 += mat.m01; m02 += mat.m02; m03 += mat.m03;
			m10 += mat.m10; m11 += mat.m11; m12 += mat.m12; m13 += mat.m13;
			m20 += mat.m20; m21 += mat.m21; m22 += mat.m22; m23 += mat.m23;			

			return *this;
		}
		inline CMatrix34<F>& operator -(const CMatrix34<F>& mat)
		{
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			CMatrix34<F> retMat(*this);
			retMat.m00 -= mat.m00; retMat.m01 -= mat.m01; retMat.m02 -= mat.m02; retMat.m03 -= mat.m03;
			retMat.m10 -= mat.m10; retMat.m11 -= mat.m11; retMat.m12 -= mat.m12; retMat.m13 -= mat.m13;
			retMat.m20 -= mat.m20; retMat.m21 -= mat.m21; retMat.m22 -= mat.m22; retMat.m23 -= mat.m23;			

			return retMat;
		}
		inline CMatrix34<F>& operator -()
		{
			assert(IsValidNumber(this->m00));

			m00 = -m00; m01 = -m01; m02 = -m02; m03 = -m03;
			m10 = -m10; m11 = -m11; m12 = -m12; m13 = -m13;
			m20 = -m20; m21 = -m21; m22 = -m22; m23 = -m23;			

			return *this;
		}
		inline CMatrix34<F>& operator -=(const CMatrix34<F>& mat)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			m00 -= mat.m00; m01 -= mat.m01; m02 -= mat.m02; m03 -= mat.m03;
			m10 -= mat.m10; m11 -= mat.m11; m12 -= mat.m12; m13 -= mat.m13;
			m20 -= mat.m20; m21 -= mat.m21; m22 -= mat.m22; m23 -= mat.m23;			

			return *this;
		}
		inline CMatrix34<F> operator *(const CMatrix44<F>& mat)
		{
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(mat.m00));

			/**
			*	There must be several better ways...But maybe this is good considering efficiency
			*	*/
			CMatrix34<F> retMat;
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

			return retMat;
		}
		CMatrix34<F>& operator *=(const CMatrix44<F>& mat)
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
			
			*this = retMat;
			return *this;
		}
		inline CMatrix34<F>& operator *(const F& m)
		{
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));

			CMatrix44<F> retMat(*this);
			{
				int count = 0;
				for (F* iter = &retMat.m00; count < 12; count++)
				{
					*iter = (*iter) * m;
					iter++;
				}
			}
			return retMat;
		}
		inline CMatrix34<F>& operator *=(const F& m)
		{

			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));

			int count = 0;
			for (F* iter = &m00; count < 12; count++)
			{
				*iter = (*iter) * m;
				iter++;
			}

			return *this;
		}
		inline CMatrix34<F>& operator /(const F& m)
		{
			assert(IsValidNumber(this->m00));
			assert(IsValidNumber(m));
			assert(m);

			CMatrix44<F> retMat(*this);
			{
				int count = 0;
				for (F* iter = &retMat.m00; count < 12; count++)
				{
					*iter = (*iter) / m;
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
			for (F* iter = &m00; count < 12; count++)
			{
				*iter = (*iter) / m;
				iter++;
			}

			return retMat;
		}
		inline CMatrix34<F>& operator =(const CMatrix34<F>& mat)
		{
			assert(IsValidNumber(mat.m00));

			m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = mat.m03;
			m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = mat.m13;
			m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = mat.m23;		
			return (*this);
		}
		inline bool operator ==(const CMatrix34<F>& mat)
		{
			assert(IsValidNumber(mat.m00));
			assert(IsValidNumber(this->m00));

			const F e = (F)MATRIX44_EPSILON;
			if (sizeof(F) == 4)
			{
				return ((fabsf(m00 - mat.m00) < e) && (fabsf(m01 - mat.m01) < e) && (fabsf(m02 - mat.m02) < e) && (fabsf(m03 - mat.m03) < e) &&
					(fabsf(m10 - mat.m10) < e) && (fabsf(m11 - mat.m11) < e) && (fabsf(m12 - mat.m12) < e) && (fabsf(m13 - mat.m13) < e) &&
					(fabsf(m20 - mat.m20) < e) && (fabsf(m21 - mat.m21) < e) && (fabsf(m22 - mat.m22) < e) && (fabsf(m23 - mat.m23) < e) &&					
					)

			}
			else if (sizeof(F) == 8)
			{
				return ((fabs(m00 - mat.m00) < e) && (fabs(m01 - mat.m01) < e) && (fabs(m02 - mat.m02) < e) && (fabsf(m03 - mat.m03) < e) &&
					(fabs(m10 - mat.m10) < e) && (fabs(m11 - mat.m11) < e) && (fabs(m12 - mat.m12) < e) && (fabs(m13 - mat.m13) < e) &&
					(fabs(m20 - mat.m20) < e) && (fabs(m21 - mat.m21) < e) && (fabs(m22 - mat.m22) < e) && (fabs(m23 - mat.m23) < e) &&					
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
		}

		/**
		 *	This series of SetRotation methods work worse than Rotate methods, 
		 *	so when there is a lot of computation, consider Rotate rather than these.
		 *	*/		
		void RotateAroundX(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix44<F> mat;
			mat.Identity();
			//...							//...
				mat.m11 = c;	mat.m12 = s;
				mat.m21 = -s;	mat.m22 = c;
			//...							//...
			(*this) *= mat;
		}
		
		void RotateAroundY(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix44<F> mat;
			mat.Identity();
			//...								//...
			mat.m00 = c;	mat.m02 = -s;
			mat.m20 = s;	mat.m22 = c;
			//...								//...
			(*this) *= mat;

		}
		
		void RotateAroundZ(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix44<F> mat;
			mat.Identity();
			//...								//...
			mat.m00 = c;	mat.m01 = s;
			mat.m10 = -s;	mat.m11 = c;
			//...								//...
			(*this) *= mat;
		}
		
		void Scale(F scale)
		{
			m00 *= scale; m01 *= scale; m02 *= scale;
			m10 *= scale; m11 *= scale; m12 *= scale;
			m20 *= scale; m21 *= scale; m22 *= scale;
		}
		
		void Scale(F sx, F sy, F sz)
		{
			m00 *= sx; m01 *= sy; m02 *= sz;
			m10 *= sx; m11 *= sy; m12 *= sz;
			m20 *= sx; m21 *= sy; m22 *= sz;
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
		
		void SetRotationX(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix34<F> mat;
			mat.Identity();
			//...							//...
			mat.m11 = c;	mat.m12 = s;
			mat.m21 = -s;	mat.m22 = c;
			//...							//...
			CVector3<F> tranlation = GetTranslation();
			(*this) = mat;
			SetTranslation(translation);
		}
		
		void SetRotationY(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix34<F> mat;
			mat.Identity();
			//...							//...
			mat.m00 = c;	mat.m02 = -s;
			mat.m20 = s;	mat.m22 = c;
			//...							//...
			CVector3<F> tranlation = GetTranslation();
			(*this) = mat;
			SetTranslation(translation);
		}
		
		void SetRotationZ(F degree)
		{
			F s;
			F c;
			SinCos(degree, &s, &c);
			CMatrix34<F> mat;
			mat.Identity();
			//...							//...
			mat.m00 = c;	mat.m01 = s;
			mat.m10 = -s;	mat.m11 = c;
			//...							//...
			CVector3<F> tranlation = GetTranslation();
			(*this) = mat;
			SetTranslation(translation);
		}
		
		void SetRotationXYZ(F degX, F degY, F degZ)
		{
			F sx, cx;
			F sy, cy;
			F sz, cz;
			SinCos(degX, &sx, &cx);
			SinCos(degY, &sy, &cy);
			SinCos(degZ, &sz, &cz);
			CMatrix34<F> mat;
			mat.Identity();
			mat.m00 = cy*cz; mat.m01 = sy*cz*sx - cx*sz; mat.m02 = sy*cz*cx + sx*sz;
			mat.m10 = cy*sz; mat.m11 = sy*sz*sx + cx*cz; mat.m12 = sy*sz*cx - sx*cz;
			mat.m20 = -sy;   mat.m21 = cy*sx;			 mat.m22 = cy*cx;
			//...							//...
			CVector3<F> tranlation = GetTranslation();
			(*this) = mat;
			SetTranslation(translation);
		}
		
		void SetColumn(__int32 index, CVector3<F>& vec)
		{
			switch (index)
			{
			case 0:
				m00 = vec.x;
				m10 = vec.y;
				m20 = vec.z;
				break;
			case 1:
				m01 = vec.x;
				m11 = vec.y;
				m21 = vec.z;
				break;
			case 2:
				m02 = vec.x;
				m12 = vec.y;
				m22 = vec.z;
				break;
			case 3:
				m03 = vec.x;
				m13 = vec.y;
				m23 = vec.z;
				break;
			default:
				break;
			}
		}
		
		CVector3<F> GetColumn(__int32 index)
		{
			switch (index)
			{
			case 0:
				return CVector3<F>(m00, m10, m20);
			case 1:
				return CVector3<F>(m01, m11, m21);
			case 2:
				return CVector3<F>(m02, m12, m22);
			case 3:
				return CVector3<F>(m03, m13, m23);
			default:
				return CVector3<F>(0.0f, 0.0f, 0.0f);
			}
		}

		void CreateFromVector(CVector3<F>& vUp, CVector3<F>& vLook)
		{
			if (vLook.IsZero())
			{
				//Invalid forward orientation
				return;
			}
			CVector3<F> zAxis = vLook;
			zAxis.Normalize();

			CVector3<F> yAxis = vUp;
			yAxis.Normalize();
			assert(!yAxis.IsZero());

			//Avoid Euler's dead lock
			if (yAxis == zAxis)
			{
				yAxis.Set(0, -zAxis.z, 0);
				CVector3<F> xAxis(1.0f, 0, 0);
				SetColumn(0, xAxis);
				SetColumn(1, yAxis);
				SetColumn(2, zAxis);
				return;
			}
			CVector3<F> xAxis = yAxis.Cross(zAxis);
			SetColumn(0, xAxis);
			SetColumn(1, yAxis);
			SetColumn(2, zAxis);
		}


	public:
		F m00, m01, m02, m03;
		F m10, m11, m12, m13;
		F m20, m21, m22, m23;
#endif
	};
}

#endif