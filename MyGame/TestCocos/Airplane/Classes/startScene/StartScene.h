#include "../common/CannonCommon.h"
#include "../../extensions/GUI/CCControlExtension/CCControlSlider.h"
#pragma once
USING_NS_CC;

class CStartScene : public Scene
{
public:
	virtual bool init() override;
	// three callback funcs corresponding to three menuLabels
	void OnMenuClicked(const std::vector<cocos2d::Touch*>& touches, Event* event);
	void OnStartMenuClicked();
	void OnCloseSetting(Ref*);
	void OnOffSound(Ref*);
	void OnOnSound(Ref*);
	void ToggleSound(Ref*);
	void update(float delta);
	
private:
	//a layer to hold all menus
	Layer* m_UILayer;
	//a layer used to setting env such as volumn ratio, on/off
	Layer* m_settingLayer;
	//three menuLabels 
	MenuItemImage* m_startMenu;
	MenuItemImage* m_continueMenu;
	MenuItemLabel* m_settingMenu;
	MenuItemLabel* m_exitMenu;

	//
	extension::ControlSlider* m_volumnSlider;

	SINGLE_INSTANCE(CStartScene);
};