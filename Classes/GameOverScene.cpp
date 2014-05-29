//
//  GameOverScene.cpp
//  TileGame
//
//  Created by xiekun on 5/29/14.
//
//

#include "GameOverScene.h"
#include "HelloWorldScene.h"

using namespace cocos2d;



bool GameOverLayer::init(){
    
    // 1. super init first
    if( !CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)) ) //RGBA
    {
        return false;
    }
    
    this->setColor(ccc3(255, 255, 255));
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    
    _label = CCLabelTTF::create("", "Arial", 32);
    _label->setColor(ccc3(0,0,0));
    _label->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(_label);
    
    CCCallFuncN *actionMoveDone = new CCCallFuncN;
    actionMoveDone->initWithTarget(this, callfuncN_selector(GameOverLayer::gameOverDone));
    CCSequence* sequence=CCSequence::create(CCDelayTime::create(3),actionMoveDone,NULL);
    this->runAction(sequence);
    
    return true;
}

void GameOverLayer::gameOverDone() {
    CCDirector::sharedDirector()->replaceScene(HelloWorld::scene());
}



GameOverScene* GameOverScene::scene()
{
    GameOverScene *gameOverScene = GameOverScene::create();
    gameOverScene->_layer = new GameOverLayer();
    gameOverScene->_layer->init();
    gameOverScene->addChild(gameOverScene->_layer);
    
    return gameOverScene;
}

void GameOverScene::setString(const char *str)
{
    _layer->_label->setString(str);
}