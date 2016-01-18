#ifndef _GD_MATRIX_H_
#define _GD_MATRIX_H_

#include "GDMatrix33.h"
#include "GDMatrix34.h"
#include "GDMatrix44.h"

namespace GD
{
	/**
	 *	Matrix is defaultly set with matrix34 typed float.
	 *	*/
	typedef CMatrix44<float> Matrix;
}

#endif