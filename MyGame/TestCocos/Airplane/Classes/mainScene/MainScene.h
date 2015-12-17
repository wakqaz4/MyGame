#ifndef _MAIN_SCENE_H_
#define _MAIN_SCENE_H_
#pragma once
#include "cocos2d.h"
#include "../common/CannonCommon.h"


class CMainScene: public cocos2d::Scene
{
public:    

    virtual bool init() override;    
    
    // implement the "static create()" method manually
	SINGLE_INSTANCE(CMainScene);

};

#endif // __HELLOWORLD_SCENE_H__
