
#include <windows.h>
#pragma once

class CSprite
{
public:
	enum DIRECTION
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

public:
	CSprite()
	{
		mPosX = 0;
		mPosY = 0;
		mWidth = 60;
		mHeight = 108;
		mBitmap = nullptr;
		mDirection = DOWN;
		mIsActive = true;
	};
	~CSprite(){};
	bool SetBitmap(HBITMAP&);
	HBITMAP GetBitMap();

public:
	HBITMAP* mBitmap;
	int mPosX;
	int mPosY;
	int mWidth;
	int mHeight;
	DIRECTION mDirection;
	bool mIsActive;
};