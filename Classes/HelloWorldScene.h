
#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "HudLayer.h"
using namespace cocos2d;
class HelloWorld : public cocos2d::CCLayer
{
private:
    CCTMXTiledMap *_tileMap;
    CCTMXLayer *_background;
    CCSprite *_player;
    CCTMXLayer *_meta;
    CCTMXLayer *_foreground;
    
    HudLayer *_hud;
    int _numCollected;
    CCArray *_enemies;
    CCArray *_projectiles;

    
public:
    int _mode;
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    void setViewPointCenter(CCPoint position);
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);
    
    void registerWithTouchDispatcher();
    
    void setPlayerPosition(CCPoint position);
    
    
    bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    
    void ccTouchEnded(CCTouch *touch, CCEvent *event);
    
    void animateEnemy(CCSprite *enemy);
    
    void enemyMoveFinished(CCSprite* pSender);
    
    CCPoint tileCoordForPosition(CCPoint position);
    
    void projectileMoveFinished(CCSprite *sprite);
    
    void testCollisions(CCTime dt);
    
    void win();
    void lose();
};

#endif // __HELLOWORLD_SCENE_H__
