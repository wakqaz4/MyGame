#ifndef _GD_OBSERVER_H_
#define _GD_OBSERVER_H_
#pragma once

#include "../GDCamera/GDCamera.h"
#include "../GDMacro/GDMacro.h"
#include "../dInputUtility/dInputUtility/dInputUtility/Keyboard.h"
#include "../dInputUtility/dInputUtility/dInputUtility/Mouse.h"

namespace GD
{
	class CObserver
	{
	public:
		enum MODE
		{
			FREE_MODE,
			LAND_MODE,
			MODE_COUNT
		};
	public:
		CObserver():
			m_pCamera(nullptr), 
			m_pMouse(nullptr),
			m_pKeyboard(nullptr),
			m_mode(FREE_MODE)
		{};
		~CObserver()
		{ 
			// No need to release pointers to camera, mouse and keyboard,
			// case they'll be deleted where they are newed
			// And now I know why we need smart_ptr so desperately.
		}

		inline bool Init()
		{ 
			return true;
		};
		inline void Update()
		{
			if ((m_pCamera == nullptr) || (m_pMouse == nullptr) || (m_pKeyboard == nullptr))
			{
				return;
			}

			float dx = m_pMouse->GetDeltaX();
			float dy = m_pMouse->GetDeltaY();
			float dz = m_pMouse->GetDeltaZ();
			m_pCamera->TranslateForward(dz / 20);
			m_pCamera->RotateAroundX(-dy / 20);
			m_pCamera->RotateAroundY(-dx / 20);

			float delta = 0.0f;
			if (m_pKeyboard->IsKeyDown(DIK_LSHIFT) || m_pKeyboard->IsKeyDown(DIK_RSHIFT))
			{
				delta = 0.0002f;
			}
			else
			{
				delta = 0.0001f;
			}
			if (m_mode == FREE_MODE)
			{
				if (m_pKeyboard->IsKeyDown(DIK_W))
				{
					m_pCamera->TranslateForward(delta);
				}
				if (m_pKeyboard->IsKeyDown(DIK_S))
				{
					m_pCamera->TranslateForward(-delta);
				}
				if (m_pKeyboard->IsKeyDown(DIK_A))
				{
					m_pCamera->TranslateRightward(-delta);
				}
				if (m_pKeyboard->IsKeyDown(DIK_D))
				{
					m_pCamera->TranslateRightward(delta);
				}
			}
			else if (m_mode == LAND_MODE)
			{
				if (m_pKeyboard->IsKeyDown(DIK_W))
				{
					Vector3 lookDir = m_pCamera->GetViewMatrix().GetColumn(2);
					lookDir.y = 0;
					lookDir.Normalize();
					m_pCamera->TranslateAlongAxis(lookDir, delta);
				}
				if (m_pKeyboard->IsKeyDown(DIK_S))
				{
					Vector3 lookDir = m_pCamera->GetViewMatrix().GetColumn(2);
					lookDir.y = 0;
					lookDir.Normalize();
					m_pCamera->TranslateAlongAxis(lookDir, -delta);
				}
				if (m_pKeyboard->IsKeyDown(DIK_A))
				{
					Vector3 rightDir = m_pCamera->GetViewMatrix().GetColumn(0);
					rightDir.y = 0;
					rightDir.Normalize();
					m_pCamera->TranslateAlongAxis(rightDir, -delta);
				}
				if (m_pKeyboard->IsKeyDown(DIK_D))
				{
					Vector3 rightDir = m_pCamera->GetViewMatrix().GetColumn(0);
					rightDir.y = 0;
					rightDir.Normalize();
					m_pCamera->TranslateAlongAxis(rightDir, delta);
				}
			}
			else
			{
				/**
				 *	If observer is in MODE_COUNT, then do nothing.
				 */
			}
		}
	public:
		CCamera<float>* m_pCamera;
		CMouse* m_pMouse;
		CKeyboard* m_pKeyboard;
		MODE m_mode;
	};
}

#endif