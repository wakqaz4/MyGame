#include <stdio.h>
#include <Windows.h>
#include <GL/GL.h>
#include "FreeImage.h"

#pragma comment(lib, "FreeImage.lib")

GLubyte*  readImage(const char *filename, GLsizei *width, GLsizei *height)
{
	char *pDst = NULL, *p = NULL;
	char *bits = NULL;
	int j = 0, w = 0, h = 0, pitch = 0, bpp = 0;

	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

	FIBITMAP *bitmap = FreeImage_Load(format, filename, 0);

	bpp = FreeImage_GetBPP(bitmap);
	if(32 != bpp)
	{
		FIBITMAP *temp = FreeImage_ConvertTo32Bits(bitmap);
		if(NULL == temp)
		{
			FreeImage_Unload(bitmap);
			return 0;
		}
		FreeImage_Unload(bitmap);
		bpp = 32;
		bitmap = temp;
	}

	w = FreeImage_GetWidth(bitmap);
	h = FreeImage_GetHeight(bitmap);
	pitch = FreeImage_GetPitch(bitmap);

	p = pDst = (char *)malloc(4 * w * h);
	bits = (char*)FreeImage_GetBits(bitmap);

	for(j = 0; j < h; ++j)
	{
		int i = 0;
		char *pSrc = bits + j * pitch;
		for(i = 0; i < 4 * w; i += 4)
		{
			p[i] = pSrc[i + FI_RGBA_RED];
			p[i + 1] = pSrc[i + FI_RGBA_GREEN];
			p[i + 2] = pSrc[i + FI_RGBA_BLUE];
			p[i + 3] = pSrc[i + FI_RGBA_ALPHA];
		}
		p += 4 * w;
	}

	*width = w;
	*height = h;

	FreeImage_Unload(bitmap);
	return (GLubyte*)pDst;
}