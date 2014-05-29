//
//  GameOverScene.h
//  TileGame
//
//  Created by xiekun on 5/29/14.
//
//

#ifndef __TileGame__GameOverScene__
#define __TileGame__GameOverScene__

#include "cocos2d.h"
using namespace cocos2d;

class GameOverLayer : public cocos2d::CCLayerColor
{

public:
    CCLabelTTF *_label;
    
    CREATE_FUNC(GameOverLayer);
    virtual bool init();
    void gameOverDone();

};

class GameOverScene : public CCScene
{
private:

public:
    static GameOverScene* scene();
    CREATE_FUNC(GameOverScene);
    GameOverLayer *_layer;
    void setString(const char *str);
};

#endif /* defined(__TileGame__GameOverScene__) */
