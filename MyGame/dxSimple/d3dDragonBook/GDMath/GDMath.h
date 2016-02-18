/************************************************************************
* Note: The IEEE standard for floating point arithmetic
* 
* Single Precision
* 
*	The IEEE single precision floating point standard representation requires a 32 bit word,
*	which may be represented as number from 0 to 31, left to right.
*	The first 1 bit is the sign bit, 'S';
*	The next 8 bits are the exponent bits, 'E';
*	The final 23 bits are the fraction bits, 'F';
*	
*	S EEEEEEEE FFFFFFFFFFFFFFFFFFFFFFFF
*   0 1      8 9                      31
* 
*	The value V represented by the word may be determined as follows:
*	
*	<1>. If E == 255, and F != 0, then V = NaN(Not A Number);
*	<2>. If E == 255, and F == 0, and S == 1, then V = -Infinity;
*	<3>. If E == 255, and F == 0, and S == 0, then V = Infinity;
*	<4>. If 0<E<255, then V = (-1)^S * 2^(E-127) * (1.F), here '1.F' is intended to 
*	represent the binary number created by prefixing F with an implicit leading 1 and a binary point;
*	<5>. If E == 0, and F != 0, then V = (-1)^S * 2^(-126) * (0.F), which are 'unnomalized' values;
*	<6>. If E == 0 and F == 0, and S == 1, then V = -0;
*	<7>. If E == 0, and F == 0, and S == 0, then V = 0;
*	
* Double Precision
* 
*	The IEEE double precision floating point standard representation requires a 64 bit word, 
*	which may be represented as number from 0 to 63, left to right.
*	The first 1 bit is the sign bit, 'S';
*	The next 11 bits are the exponent bits, 'E';
*	The final 52 bits are the fraction bits, 'F';
*
*	S EEEEEEEEEEE FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEEEEEE
*   0 1        11 12												  63
*
*	<1>. If E == 2047, and F != 0, then V = NaN(Not A Number);
*	<2>. If E == 2047, and F == 0, and S == 1, then V = -Infinity;
*	<3>. If E == 2047, and F == 0, and S == 0, then V = Infinity;
*	<4>. If 0 < E < 2047, then V = (-1)^S * 2^(E-1023) * (1.F), here '1.F' is intended to
*	represent the binary number created by prefixing F with an implicit leading 1 and a binary point;
*	<5>. If E == 0, and F != 0, then V = (-1)^S * 2^(-1022) * (0.F), which are 'unnomalized' values;
*	<6>. If E == 0 and F == 0, and S == 1, then V = -0;
*	<7>. If E == 0, and F == 0, and S == 0, then V = 0;
*
* // [2016/1/7 22:28 Zhaoyu.Wang]  
************************************************************************/
#ifndef _GD_MATH_H_
#define _GD_MATH_H_
#pragma once

#include "GDMatrix.h"
#include "GDVector.h"
#include <stdlib.h>
#include <math.h>

namespace GD
{
	const float PI = 3.14159265354f;

	inline f32 Sqrt(f32 num){ return sqrtf(num); }
	inline f64 Sqrt(f64 num){ return sqrt(num); }

#if defined(_CPU_X86) && defined(_MSC_VER) && !defined(_DEBUG) && !defined(LINUX)
	inline void SinCos(f32 angle, f32* pSin, f32* pCos)
	{
		__asm
		{
			FLD DWORD PTR angle
				FSINCOS
				MOV		eAX, pCos
				MOV		eDX, pSin
				FSTP	DWORD PTR [eAX]
				FSTP	DWORD PTR [eDX]
		}
	}
	inline void SinCos(f64 angle, f64* pSin, f64* pCos)
	{
		__asm
		{
			FLD QWORD PTR angle
				FSINCOS
				MOV		eAX, pCos
				MOV		eDX, pSin
				FSTP	QWORD PTR[eAX]
				FSTP	QWORD PTR[eDX]
		}
	}
#else
	inline void SinCos(f64 angle, f64* pSin, f64* pCos)
	{ 
		*pSin = f64(sin(PI*angle/180)); 
		*pCos = f64(cos(PI*angle/180)); 
	}
	inline void SinCos(f32 angle, f32* pSin, f32* pCos)
	{ 
		*pSin = f32(sin(PI*angle/180)); 
		*pCos = f32(cos(PI*angle/180)); 
	}
#endif
	template<typename F>
	bool IsValidNumber(F num)
	{
		/**
		*	A float number, will be invalid if V = NAN
		*	*/
		if (sizeof(F) == 4)
		{
			unsigned int toInt = *(unsigned int*)(&num);
			unsigned int expMask = 0xFF << 23;
			return !((toInt&expMask) == expMask);
		}
		/**
		*	Double number, will be invalid if V = NAN
		*	*/
		else if (sizeof(F) == 8)
		{
			unsigned long long toInt = *(unsigned long long*)(&num);
			unsigned long long expMask = ((unsigned long long)255) << 55;
			return !((toInt&expMask) == expMask);
		}
		else
		{

		}
		return true;
	}
	inline f32 GetRandomFloat(f32 lowBound, f32 highBound)
	{
		if (lowBound >= highBound)
		{
			return lowBound;
		}
		f32 temp = 0.0001f*(rand() % 10000);
		return (temp*(highBound - lowBound) + lowBound);
	}
	inline Vector3 GetRandomVector(Vector3 const&  lowBound, Vector3 const&  highBound)
	{
		Vector3 temp;
		temp.x = GetRandomFloat(lowBound.x, highBound.x);
		temp.y = GetRandomFloat(lowBound.y, highBound.y);
		temp.z = GetRandomFloat(lowBound.z, highBound.z);
		return temp;
	}
	/**
	 *	Better this method than the upward.
	 */
	inline void GetRandomVector(Vector3& out, const Vector3& lowBound, const Vector3& highBound)
	{
		out.x = GetRandomFloat(lowBound.x, highBound.x);
		out.y = GetRandomFloat(lowBound.y, highBound.y);
		out.z = GetRandomFloat(lowBound.z, highBound.z);
	}
}

#endif