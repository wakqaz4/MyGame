#include "StartScene.h"
#include "../mainScene/MainScene.h"
#include "audio/include/SimpleAudioEngine.h"
#include "../../extensions/GUI/CCControlExtension/CCControlSlider.h"


USING_NS_CC;

SINGLE_INSTANCE_IMPLEMENT(CStartScene);

bool CStartScene::init()
{
	if (!Scene::init())
	{
		return false;
	}
	Size winSize = Director::getInstance()->getWinSize();
	/*
		set the background of start scene & its looping animation.
	*/
	Sprite* background = Sprite::create("bg.png");
	background->setScale(2.0f);
	background->setPosition(Vec2(winSize.width / 2, 0));
	this->addChild(background);
	ActionInterval* action1 = MoveTo::create(2, Vec2(winSize.width / 2 - winSize.width / 8, 0));
	ActionInterval* action2 = MoveTo::create(2, Vec2(winSize.width / 2 + winSize.width / 8, 0));
	ActionInterval* action3 = MoveTo::create(2, Vec2(winSize.width / 2 + winSize.width / 8, winSize.height / 2));
	ActionInterval* action4 = MoveTo::create(2, Vec2(0 + winSize.width / 2 - winSize.width / 8, winSize.height / 2));
	ActionInterval* seq1 = Sequence::create(action1, action2, action3, action4, nullptr);
	background->runAction(RepeatForever::create(seq1));

	Sprite* bgStar = Sprite::create("moon.png");
	bgStar->setAnchorPoint(Vec2(0.5, 0));
	bgStar->setScale(0.5f);
	bgStar->setPosition(Vec2(winSize.width / 3 * 2, 0));
	this->addChild(bgStar, 1, 1);
	/*
		set the title 
	*/
	Node* title = Node::create();
	title->setContentSize(CCSizeMake(winSize.width - 40, 50));
	Sprite* ptmLabel = Sprite::create("meowstar.png");
	ptmLabel->setScale(0.5);
	ptmLabel->setPosition(Vec2(0, 30));
	title->addChild(ptmLabel);

	Sprite* ptbLabel = Sprite::create("battle.png");
	ptbLabel->setScale(0.5f);
	ptbLabel->setPosition(Vec2(0, -30));
	title->addChild(ptbLabel);
	title->setPosition(winSize.width / 2, winSize.height - 80);
	this->addChild(title, 2, 2);
	/*
		set the UI layer which contains four button sprites.
		it's almost useless.
	*/
	m_UILayer = Layer::create();
	this->addChild(m_UILayer);

	m_startMenu = MenuItemImage::create("newgameB.png","newgameA.png");
	m_startMenu->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + 120));
	m_startMenu->setScale(1.0f);
	m_UILayer->addChild(m_startMenu);


	m_continueMenu = MenuItemImage::create("continueA.png", "continueB.png");
	m_continueMenu->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + 60));
	m_continueMenu->setScale(1.0f);
	m_UILayer->addChild(m_continueMenu);
	
	Label* settingLabel = Label::create();
	settingLabel->setString("Setting");
	m_settingMenu = MenuItemLabel::create(settingLabel);
	m_settingMenu->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + 0));
	m_settingMenu->setScale(3.0f);
	m_UILayer->addChild(m_settingMenu);
	
	Label* exitLabel = Label::create();
	exitLabel->setString("Exit");
	m_exitMenu = MenuItemLabel::create(exitLabel);
	m_exitMenu->setPosition(Vec2(winSize.width / 2, winSize.height / 2 - 60));
	m_exitMenu->setScale(3.0f);
	m_UILayer->addChild(m_exitMenu);
	
	/*
		register listener of the scene.
		maybe register individual listener is better?
	*/
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesEnded = CC_CALLBACK_2(CStartScene::OnMenuClicked, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	/*
		setting layer for volumn
	*/
	m_settingLayer = Layer::create();
	this->addChild(m_settingLayer);
	m_settingLayer->setPosition(winSize.width / 2, winSize.height / 2);
	Sprite* settingBackground = Sprite::create("extensions/background.png");
	m_settingLayer->addChild(settingBackground);
	m_settingLayer->setVisible(false);

	m_volumnSlider = extension::ControlSlider::create("extensions/sliderTrack.png", "extensions/sliderProgress.png", "extensions/sliderThumb.png");
	m_volumnSlider->setValue(0);
	m_volumnSlider->setMinimumValue(0);
	m_volumnSlider->setMaximumValue(100);
	m_volumnSlider->setPosition(0, 10);
	m_settingLayer->addChild(m_volumnSlider);

	MenuItemImage* closeSetting = MenuItemImage::create("CloseNormal.png","CloseSelected.png", CC_CALLBACK_1(CStartScene::OnCloseSetting, this));
	closeSetting->setPosition(150, 100);
	MenuItemImage* onSound = MenuItemImage::create("sound-on-A.png", "sound-on-B.png");
	MenuItemImage* offSound = MenuItemImage::create("sound-off-A.png", "sound-off-B.png");
	MenuItemToggle* toggleVolumn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CStartScene::ToggleSound, this), onSound, offSound, nullptr);
	toggleVolumn->setPosition(0, -40);
	Menu* closeSettingMenu = Menu::create(closeSetting, toggleVolumn, nullptr);	
	m_settingLayer->addChild(closeSettingMenu);
	closeSettingMenu->setPosition(Vec2(0, 0));

	/*
		preload music  and play
	*/
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("background.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("background.mp3", true);
	this->scheduleUpdate();

	return true;
}
void CStartScene::ToggleSound(Ref* ref)
{	
	if (CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}
	else
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("background.mp3", true);
	}
}

void CStartScene::update(float delta)
{
	if (m_settingLayer->isVisible())
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(m_volumnSlider->getValue() / m_volumnSlider->getMaximumValue());
	}
}

void CStartScene::OnCloseSetting(Ref* ref)
{
	m_settingLayer->setVisible(false);
}
void CStartScene::OnMenuClicked(const std::vector<Touch*>& touches, Event* event)
{
	if (m_settingLayer->isVisible())
	{
		return;
	}

	Touch* touch = touches[0];
	Vec2 touchPos = touch->getLocation();
	if (m_startMenu && m_startMenu->getBoundingBox().containsPoint(touchPos))
	{
		Scene* mainScene = CMainScene::create();
		event->stopPropagation();
		Director::getInstance()->replaceScene(mainScene);				
	}
	else if (m_continueMenu && m_continueMenu->getBoundingBox().containsPoint(touchPos))
	{

	}
	else if (m_settingMenu && m_settingMenu->getBoundingBox().containsPoint(touchPos))
	{
		m_settingLayer->setVisible(true);
	}
	else if (m_exitMenu && m_exitMenu->getBoundingBox().containsPoint(touchPos))
	{
		Director::getInstance()->end();
	}

	event->stopPropagation();	
}