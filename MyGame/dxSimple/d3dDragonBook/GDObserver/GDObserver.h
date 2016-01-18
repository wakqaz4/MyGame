#ifndef _GD_OBSERVER_H_
#define _GD_OBSERVER_H_
#pragma once

#include "../GDCamera/GDCamera.h"
#include "../GDMacro/GDMacro.h"
#include "../dInputUtility/dInputUtility/dInputUtility/KeyBoard.h"
#include "../dInputUtility/dInputUtility/dInputUtility/Mouse.h"

namespace GD
{
	class CObserver
	{
	public:
		CObserver():
			m_pCamera(nullptr), 
			m_pMouse(nullptr),
			m_pKeyBoard(nullptr)
		{};
		~CObserver()
		{ 
			// No need to release pointers to camera, mouse and keyboard,
			// case they'll be deleted where they are newed
			// And now I know why we need smart_ptr so desperately.
		}

		inline bool Init(){ return true; };
		inline void Update()
		{
			if ((m_pCamera == nullptr) || (m_pMouse == nullptr) || (m_pKeyBoard == nullptr))
			{
				return;
			}

			float dx = m_pMouse->GetDeltaX();
			float dy = m_pMouse->GetDeltaY();
			float dz = m_pMouse->GetDeltaZ();
			m_pCamera->TranslateForward(dz / 20);
			m_pCamera->RotateAroundX(-dy / 20);
			m_pCamera->RotateAroundY(-dx / 20);

			float delta = 0.0001f;
			if (m_pKeyBoard->IsKeyDown(DIK_W))
			{
				m_pCamera->TranslateForward(delta);
			}
			if (m_pKeyBoard->IsKeyDown(DIK_S))
			{
				m_pCamera->TranslateForward(-delta);
			}
			if (m_pKeyBoard->IsKeyDown(DIK_A))
			{
				m_pCamera->TranslateRightward(-delta);
			}
			if (m_pKeyBoard->IsKeyDown(DIK_D))
			{
				m_pCamera->TranslateRightward(delta);
			}
		}
	public:
		CCamera<float>* m_pCamera;
		CMouse* m_pMouse;
		CKeyBoard* m_pKeyBoard;
	};
}

#endif