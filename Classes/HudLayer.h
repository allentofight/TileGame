//
//  HudLayer.h
//  TileGame
//
//  Created by xiekun on 5/27/14.
//
//

#ifndef __TileGame__HudLayer__
#define __TileGame__HudLayer__

#include "cocos2d.h"
//#include "HelloWorldScene.h"
class HelloWorld;
using namespace cocos2d;

class HudLayer : public cocos2d::CCLayer
{
private:
    CCLabelTTF *_label;

    
    
public:
    HelloWorld *_gameLayer;
    
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HudLayer);
    
    void numCollectedChanged (int numCollected);
    
    void projectileButtonTapped(CCNode *node);
};

#endif /* defined(__TileGame__HudLayer__) */
