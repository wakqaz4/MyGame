#ifndef _GD_MACRO_H_
#define _GD_MACRO_H_
#pragma once

namespace GD
{
#define SAFE_DELETE(p) if(nullptr != (p)) {delete (p); p = nullptr;}
#define SAFE_RELEASE(p) if (nullptr != (p)){(p)->Release();}


// RGBA typed color often used
#define D3DCOLOR_WHITE D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
#define D3DCOLOR_BLACK D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)
#define D3DCOLOR_RED D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)
#define D3DCOLOR_GREEN D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)
#define D3DCOLOR_BLUE D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)
#define D3DCOLOR_YELLOW D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f)
#define D3DCOLOR_MAGENTA D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f)
#define D3DCOLOR_CYAN D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)
#define D3DCOLOR_BEACH_SAND D3DXCOLOR(1.0f, 0.98f, 0.59f, 1.0f)
#define D3DCOLOR_DESERT_SAND D3DXCOLOR(0.98f, 0.80f, 0.54f, 1.0f)
#define D3DCOLOR_LIGHTGREEN D3DXCOLOR(0.24f, 0.74f, 0.48f, 1.0f)
#define D3DCOLOR_PUREGREEN D3DXCOLOR(0.0f, 0.66f, 0.32f, 1.0f)
#define D3DCOLOR_DARKGREEN D3DXCOLOR(0.0f, 0.46f, 0.22f, 1.0f)
#define D3DCOLOR_LIGHT_YELLOW_GREEN D3DXCOLOR(0.49f, 0.79f, 0.46f, 1.0f)
#define D3DCOLOR_PURE_YELLOW_GREEN D3DXCOLOR(0.26f, 0.72f, 0.29f, 1.0f)
#define D3DCOLOR_DARK_YELLOW_GREEN D3DXCOLOR(0.09f, 0.48f, 0.19f, 1.0f)
#define D3DCOLOR_LIGHTBROWN D3DXCOLOR(0.79f, 0.61f, 0.43f, 1.0f)
#define D3DCOLOR_DARKBROWN D3DXCOLOR(0.45f, 0.39f, 0.34f, 1.0f)

}

#endif