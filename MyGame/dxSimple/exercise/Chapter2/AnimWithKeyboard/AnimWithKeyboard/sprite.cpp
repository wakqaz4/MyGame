#include "sprite.h"

bool CSprite::SetBitmap(HBITMAP& bmp)
{
	mBitmap = &bmp;
	return true;
}
HBITMAP CSprite::GetBitMap()
{
	if (mBitmap != nullptr)
	{
		return *mBitmap;
	}
	return nullptr;
}