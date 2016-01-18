/************************************************************************
* Camera is so often used in 3D games. So we must be very very very  
* careful with implementation of this class.
* 
* 
* I use a 'right-handed' coordinate system, where the positive X-Axis 
* points to the right, the positive Y-Axis points away from viewer and 
* the positive Z-Axis points up.
* 
*	Z-Axis
*		|	Y-Axis
*		|   /
*		|  /
*		| /
*		|/
*		+---------------- X-Axis
*		
* I use this because 3Dmax use this kind of system. And as you know, 
* DX and OpenGL used a different coordinate system: DX left handed, 
* OpengGL right handed.(So we have three coordinations in total, waiting
* for a boom of matrix transformation!)
* To transform this coordinate system to which DX used, we just simply 
* make a clockwise rotation around X-Axis, where Coordination will be 
* like this:
* 	
* 			Z-Axis
*		    /
*		   /
*		  /
*		 /
*		+---------------- X-Axis
*		|	
*		|
*		|
*		|
*		|
*		Y-Axis
* This conversion happens in render.
* 
* The 6 DOFs are stored in m_matrix(CMatrix34). The 3 orientation-DOFs are
* stored in 3*3 part, while 3 position-DOFs are stored in 1*3 part(A CVector3).
* 
* * Here is a question: is a matrix34 enough for camera member?
* We'll often visit camera's position and rotation, so it's a good
* idea to store position as a Vector3 variable and rotation as a quat
* so that we could improve efficiency a little.
* And thus another question appears: how to guarantee that m_pos & m_rot
* is synchronized with m_matrix34? Maybe ... when we GetPos & GetRot,
* check if the members are the same with m_matrix34?
* No no no, that's a little costing. Maybe this way is better: when we
* SetMatrix, we set Pos&Rot at the same time...
* No, we'll do a lot of operations when moving camera, which may be much
* more than Getting cameraInfo...
* It seems no need to use m_Pos & m_Rot at all?
* I took reference of CE3::CCamera, it did like this: use only m_matrix34(>_<)
* // [2016/1/9 15:56 Zhaoyu.Wang] 
* 
* Another problem merges: Should frustum set as a member of CCamera? Of course.
* So I need to add frustum the next step...
* // [2016/1/9 21:30 Zhaoyu.Wang]  
* 
* Maybe no need to add frustum at first. I think I can use a projection matrix34 
* instead of frustum. Of course, relevant methods on frustum should be added.
* 1stly include methods on viewMatrix, includes SetViewMatrix, GetViewMatrix, 
* GetLookDir, GetUpDir, GetRightDir;
* 2dly include methods on projMatrix, include SetProjMatrix, GetProjMatrix, 
* 
* (The matrix used matrix34 while not 44 to save space. A static cast to 
* matrix44/D3DMATRIX/D3DXMATRIX will be found in DXUT;)
* // [2016/1/10 16:49 Zhaoyu.Wang] 
* 
* Just make the camera in DX coordination system to make code simple.
* // [2016/1/13 12:16 Zhaoyu.Wang]  
* 
************************************************************************/
#ifndef _GD_CAMERA_H_
#define _GD_CAMERA_H_

#include <math.h>
#include "../GDMath/GDMatrix.h"
#include "../GDMath/GDVector.h"

namespace GD
{
	const float PI = 3.14159265354f;
	enum CAMERA_TYPE
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,
		CAMERA_TYPE_COUNT
	};
	struct ViewPort
	{
		int m_x;
		int m_y;
		int m_width;
		int m_height;
		float m_minZ;
		float m_maxZ;
	};

	template<typename F = float>
	class CCamera
	{
	public:
		/**
		 *	Camera will be defaultly constructed, looking at Y-Axis(0, 1, 0)(where in DX it 
		 *	will be Rotated around X 90 degrees looking at Z-Axis), with FOV 60 degrees, 
		 *	*/
		inline CCamera<F>():
			m_fov(PI / 4.0f),
			m_aspect(4.0f/3.0f),
			m_zNear(1.0f),
			m_zFar(1000.0f),
			m_width(800.0f),
			m_height(600.0f),
			m_cameraType(PERSPECTIVE)
			{
				m_viewMatrix.Identity();
				UpdateProjMatrix();
				SetViewPort(0, 0, 800, 600, 0.0f, 1.0f);
			}
		;
		inline ~CCamera<F>(){};

	//////////////////////////////////////////////////////////////////////////
	// View Matrix.  
	// Note: viewMatrix can be divided into 4 parts(Left handed coordination system):
	// 1. RightVector	(Local X-Axis)	-the 1rd column;
	// 2. LookVector	(Local Y-Axis)	-the 2nd column;(also called forward vector, forward direction)
	// 3. UpVector		(Local Z-Axis)	-the 3rd column;
	// 4. Position						-the -4th column ;
	//////////////////////////////////////////////////////////////////////////
	public:
		/**
		*	Use up vector, camera's position(vEye), and goal's position(vAt) to form view matrix;
		*	*/		
		void SetViewMatrix(const CVector3<F>& vUp, const CVector3<F>& vEye, const CVector3<F>& vAt)
		{
			CVector3<F> vDir = vEye - vAt;
			SetViewMatrix(vUp, vDir);
		}

		/**
		*	Use up vector, and look vector to form view matrix, camera's position will be set as (0,0,0)
		*	*/		
		void SetViewMatrix(CVector3<F>& vUp, CVector3<F>& vDir)
		{
			m_viewMatrix.CreateFromVector(vUp, vDir);
		}
		
		void SetViewMatrix(const CMatrix34<F>& mat)
		{
			m_viewMatrix = mat;
		}
		CMatrix34<F> GetViewMatrix(){ return m_viewMatrix; }

		//View matrix used in directX, left handed
		CMatrix44<F> GetViewMatrixGL()
		{
			return GetViewMatrixDX().GetFliped();
		}
		//View matrix used in OpenGL, right handed
		CMatrix44<F> GetViewMatrixDX()
		{	
			CMatrix44<F> dxViewMatrix(m_viewMatrix);
			dxViewMatrix.m03 = 0;
			dxViewMatrix.m13 = 0;
			dxViewMatrix.m23 = 0;
			dxViewMatrix.m30 = -GetPosition()*m_viewMatrix.GetColumn(0);
			dxViewMatrix.m31 = -GetPosition()*m_viewMatrix.GetColumn(1);
			dxViewMatrix.m32 = -GetPosition()*m_viewMatrix.GetColumn(2);
			return dxViewMatrix;
		}

		void LookAtLH(CVector3<F>& vEye, CVector3<F>& vAt, CVector3<F>& vUp)
		{
			CVector3<F>& vLookDir = vAt - vEye;
			assert(!vLookDir.IsZero());

			vLookDir.Normalize();
			SetViewMatrix(vUp, vLookDir);
			SetPosition(vEye);
		}
		
		void SetPosition(CVector3<F>& vec)
		{
			m_viewMatrix.SetColumn(3, vec);
		}

		void SetPosition(F x, F y, F z)
		{
			m_viewMatrix.SetColumn(3, CVector3<F>(x, y, z));
		}
		
		CVector3<F> GetPosition()
		{
			return m_viewMatrix.GetColumn(3);
		}

		/**
		*	Base Translation & Rotation methods
		*	*/

		//In world coord		
		void TranslateAlongAxis(CVector3<F>& axis, F dist)
		{
			CVector3<F>& translation = axis*dist;
			m_viewMatrix.Translate(translation);
		}

		//In world coord
		/**
		*	It's easy to do like this: m_viewMatrix = m_viewMatrix^(-1)*matrix34<F>(vUp, axis)*RotateAroundY(degree)
		*	However, it cost too much. So better to do like following:
		*	1.	View axis as Y-Axis;
		*	2.	axis.Cross(vEye - vPoint) should be X-Axis;
		*	3.	Get Z-Axis;
		*	4.	Create transformMatrix;
		*	5.	Rotate vEye around Y-Axis with degree;
		*	*/		
		void RotateAroundArbitraryAxis(CVector3<F> point, CVector3<F>& axis, F degree, bool LookAtPoint)
		{
			CVector3<F> vector = normalize(arbitAxis);
			assert(vector != 0);

			float u = vector.x;
			float v = vector.y;
			float w = vector.z;
			float a = p.x;
			float b = p.y;
			float c = p.z;

			CVector3<F> rightAxis	(u*u + (v*v + w*w)*cos(t),		u*v*(1 - cos(t)) + w*sin(t),	u*w*(1 - cos(t)) - v*sin(t));
			CVector3<F> fwdAxis		(u*v*(1 - cos(t)) - w*sin(t),	v*v + (u*u + w*w)*cos(t),		v*w*(1 - cos(t)) + u*sin(t));
			CVector3<F> upAxis		(u*w*(1 - cos(t)) + v*sin(t),	v*w*(1 - cos(t)) - u*sin(t),	w*w + (u*u + v*v)*cos(t));
			CVector3<F> translation((a*(v*v + w*w) - u*(b*v + c*w))*(1 - cos(t)) + (b*w - c*v)*sin(t),
				(b*(u*u + w*w) - v*(a*u + c*w))*(1 - cos(t)) + (c*u - a*w)*sin(t),
				(c*(u*u + v*v) - w*(a*u + b*v))*(1 - cos(t)) + (a*v - b*u)*sin(t));
			CVector3<F> temp;
			temp.x = rightAxis.x*vEye.x + fwdAxis.x*vEye.y + upAxis.x*vEye.z + translation.x;
			temp.y = rightAxis.y*vEye.x + fwdAxis.y*vEye.y + upAxis.y*vEye.z + translation.y;
			temp.z = rightAxis.z*vEye.x + fwdAxis.z*vEye.y + upAxis.z*vEye.z + translation.z;
			vEye = temp;
			if (LookAtPoint)
			{
				LookAt(point);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// This function means the axis's zero point is camera's position,
		// so the camera rotates around the axis that crosses itself.
		// In fact, we could use 2 methods as below shows.
		//	1st: calculate the matrix newMat with axis set as Y-Axis, and the result
		//	should be set as: newMat*RotateAroundY(degree)*newMat(T) (as newMat is a 
		//	orthogonal/isotropic matrix)
		//	2nd: Use Euler theorem, which is much faster than the 1st;
		//////////////////////////////////////////////////////////////////////////
		void RotateAroundAxis(CVector3<F>& vec, F degree)
		{
			/**
			*	The 1st method, really simple, but a little costing
			*	*/
			//CVector3<F> yAxis = axis.Normalize();
			//assert(yAxis.GetSquareLength());
			//if (yAxis.Cross(m_viewMatrix.GetUpDir()) != 0)
			//{
			//	CMatrix34 newMat(0);
			//	newMat.CreateFromVector(m_viewMatrix.GetUpDir(), CVector3<F>(0, 0, 1));
			//	newMat.RotateAroundY(degree);
			//	newMat *= newMat.Revert();
			//	m_viewMatrix = newMat;
			//}
			//else if (yAxis == m_viewMatrix.GetUpDir().Normalize())
			//{
			//	RotateAroundZ(degree);
			//	return;
			//}
			//else
			//{
			//	RotateAroundZ(-degree);
			//	return;
			//}
			//} 

			/**
			*	The 2nd method, Euler theorem
			*	*/
			CVector3<F> axis = vec;
			axis.Normalize();
			F s;
			F c;
			SinCos(degree, &s, &c);
			F mc = 1.0 - c;
			F mcx = mc*axis.x;	F mcy = mc*axis.y;	F mcz = mc*axis.z;
			F tcx = axis.x*s;	F tcy = axis.y*s;	F tcz = axis.z*s;
			m_viewMatrix.m00 = mcx*axis.x + c;		m_viewMatrix.m01 = mcx*axis.y - tcz; m_viewMatrix.m02 = mcx*axis.z + tcy;
			m_viewMatrix.m10 = mcy*axis.x + tcz;	m_viewMatrix.m11 = mcy*axis.y + c;	 m_viewMatrix.m12 = mcy*axis.z - tcx;
			m_viewMatrix.m20 = mcz*axis.x - tcy;	m_viewMatrix.m21 = mcz*axis.y + tcx; m_viewMatrix.m22 = mcz*axis.z + c;

		}
		/**
		 *	In world coordination system
		 *	*/
		void TranslateAlongX(F dist)
		{
			m_viewMatrix.m30 += dist;
		}		
		void TranslateAlongY(F dist)
		{
			m_viewMatrix.m31 += dist;
		}
		void TranslateAlongZ(F dist)
		{
			m_viewMatrix.m32 += dist;
		}		

		/**
		 *	In local coordination system
		 *	*/
		void TranslateForward(F dist)
		{
			m_viewMatrix.Translate(m_viewMatrix.GetColumn(2)*dist);
		}		
		void TranslateUpward(F dist)
		{
			m_viewMatrix.Translate(m_viewMatrix.GetColumn(1)*dist);
		}
		void TranslateRightward(F dist)
		{
			m_viewMatrix.Translate(m_viewMatrix.GetColumn(0)*dist);
		}
		void RotateAroundX(F degree)
		{
			m_viewMatrix.RotateAroundX(degree);
		}		
		void RotateAroundY(F degree)
		{
			m_viewMatrix.RotateAroundY(degree);
		}
		void RotateAroundZ(F degree)
		{
			m_viewMatrix.RotateAroundZ(degree);
		}		
		void RotateAroundForward(F degree)
		{
			m_viewMatrix.RotateAroundY(degree);
		}		
		void RotateAroundUpward(F degree)
		{
			m_viewMatrix.RotateAroundZ(degree);
		}		
		void RotateAroundRightward(F degree)
		{
			m_viewMatrix.RotateAroundX(degree);
		}

		//////////////////////////////////////////////////////////////////////////
		// Projection Matrix  
		//////////////////////////////////////////////////////////////////////////
		/**
		*	Use FOV, aspectRatio, Z-Far, Z-Near ,width, height to form the projection
		*	matrix, which could be applied as DX projection matrix directly.
		*	*/		
		void UpdateProjMatrix()
		{
			if (m_cameraType == ORTHOGRAPHIC)
			{
				F zLen = m_zFar - m_zNear;
				m_projMatrix.Identity();
				m_projMatrix.m00 = (F)2.0f / m_width;
				m_projMatrix.m11 = (F)2.0f / m_height;
				m_projMatrix.m22 = (F)1.0f / zLen;
				m_projMatrix.m23 = (F)(-m_zNear) / zLen;
				m_projMatrix.m33 = (F)1.0f;
			}
			else if (m_cameraType == PERSPECTIVE)
			{
				F zLen = m_zFar - m_zNear;
				m_projMatrix.Identity();
				m_projMatrix.m00 = 1 / (tan(m_fov*0.5f)* m_aspect);
				m_projMatrix.m11 = 1 / tan(m_fov*0.5f);
				m_projMatrix.m22 = m_zFar / zLen;
				m_projMatrix.m23 = -m_zFar*m_zNear / zLen;
				m_projMatrix.m32 = (F)1.0f;
				m_projMatrix.m33 = 0;
			}
			else
			{
				//Other types, to be added...
			}
		}
		void SetPerspectiveProjMatrix(F fov, F aspect, F zNear, F zFar)
		{
			m_fov = fov;
			m_aspect = aspect;
			m_zFar = zFar;
			m_zNear = zNear;
			UpdateProjMatrix();
		}
		void SetOrthographicProjMatrix(F width, F height, F zNear, F zFar)
		{
			m_width = width;
			m_height = height;
			m_zFar = zFar;
			m_zNear = zNear;
			UpdateProjMatrix();
		}
		void SetProjMatrix(const CMatrix34<F>& mat)
		{
			m_projMatrix = mat;
		}
		void SetProjMatrixFOV(F fov)
		{
			m_fov = fov;
			SetProjMatrix();
		}
		void SetProjMatrixAspect(F aspect)
		{
			m_aspect = aspect;
			SetProjMatrix();
		}
		void SetProjMatrixZNear(F zNear)
		{
			m_zNear = zNear;
			SetProjMatrix();
		}
		void SetProjMatrixZFar(F zFar)
		{
			m_zFar = zFar;
			SetProjMatrix();
		}

		CMatrix44<F> GetProjMatrix(){ return m_projMatrix; }

		void SetViewPort(int x, int y, int width, int height, float zMin, float zMax)
		{
			m_viewPort.m_x = x;
			m_viewPort.m_y = y;
			m_viewPort.m_width = width;
			m_viewPort.m_height = height;
			m_viewPort.m_minZ = zMin;
			m_viewPort.m_maxZ = zMax;
		}
		ViewPort GetViewPort(){ return m_viewPort; }

	public:
		CMatrix34<F>	m_viewMatrix;
		/**
		 *	In order to save projection matrix, we'll have to used matrix44
		 *	instead of matrix34;
		 *	*/
		CMatrix44<F>	m_projMatrix;
		/**
		 *	enum is stored as int in memory, so the data structure is aligned,
		 *	no need to put it to the end.
		 *	*/
		CAMERA_TYPE		m_cameraType;
		/**
		 *	Really not sure if these vars should exist...
		 *	Better have than not.
		 *	*/
		F				m_fov;
		F				m_aspect;
		F				m_zNear;
		F				m_zFar;
		F				m_width;
		F				m_height;
		/**
		 *	View port, used to store the info of the window
		 *	*/

		ViewPort m_viewPort;
	};
}

#endif