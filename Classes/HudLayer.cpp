//
//  HudLayer.cpp
//  TileGame
//
//  Created by xiekun on 5/27/14.
//
//

#include "HudLayer.h"
#include "HelloWorldScene.h"
using namespace cocos2d;

bool HudLayer::init()
{
    if (CCLayer::init()) {
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        
        _label = new CCLabelTTF();
        _label->initWithString("0", "Verdana-Bold", 18.0);
        _label->setColor(ccc3(0,0,0));
        
        int margin = 10;
        _label->setPosition(ccp(winSize.width - (_label->getContentSize().width/2) - margin, _label->getContentSize().height/2 + margin));
        this->addChild(_label);
        
        CCMenuItemImage *btnOn =CCMenuItemImage::create("projectile-button-on.png" ,"projectile-button-on.png");
        
        CCMenuItemImage *btnOff=CCMenuItemImage::create("projectile-button-off.png","projectile-button-off.png");
        
        CCMenuItemToggle *btnSnd =CCMenuItemToggle::createWithTarget(this,menu_selector(HudLayer::projectileButtonTapped),btnOn, btnOff,NULL);
        
        CCMenu *toggleMenu = CCMenu::create(btnSnd,NULL);
        toggleMenu->setPosition(ccp(300, 32));
        this->addChild(toggleMenu);
    }
    
    return true;
}

void HudLayer::projectileButtonTapped(CCNode *node){
    if (_gameLayer->_mode == 1) {
        _gameLayer->_mode = 0;
    } else {
        _gameLayer->_mode = 1;
    }
}

void HudLayer::numCollectedChanged(int numCollected)
{
    CCString *labelCollected = new CCString();
    labelCollected->initWithFormat("%d",numCollected);
    _label->setString(labelCollected->getCString());
}
