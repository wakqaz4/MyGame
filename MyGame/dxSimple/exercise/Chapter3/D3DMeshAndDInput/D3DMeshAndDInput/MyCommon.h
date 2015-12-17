/************************************************************************
* this file includes different kinds of macros and definitions                                                                  
************************************************************************/
#pragma once

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SAFE_RELEASE(p) {if (p){(p)->Release(); (p) = nullptr;}}
#define SAFE_DELETE(p) { if(p) { delete (p);     (p)=nullptr; } }  
