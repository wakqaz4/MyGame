/************************************************************************
* This head file includes some usual macros and definitions which 
* are defined for my personal usage.
* //  [12/10/2015 Zhaoyu.Wang]                                                                  
************************************************************************/
#include <windows.h>
#pragma once

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SAFE_RELEASE(p) {if(p){(p)->Release(); (p) = nullptr;}}
#define SAFE_DELETE(p) {if(p){delete (p); (p) =  nullptr;}}
#define PI 3.14159f

typedef void(*pFunc)(HWND);