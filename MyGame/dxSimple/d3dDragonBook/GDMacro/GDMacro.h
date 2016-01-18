#ifndef _GD_MACRO_H_
#define _GD_MACRO_H_
#pragma once

namespace GD
{
#define SAFE_DELETE(p) if(nullptr == (p)) {delete (p);}

}

#endif