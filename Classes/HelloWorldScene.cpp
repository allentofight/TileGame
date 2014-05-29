#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GameOverScene.h"

using namespace cocos2d;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    
    HudLayer *hud = new HudLayer();
    hud->init();
    scene->addChild(hud);
    layer->_hud = hud;
    
    hud->_gameLayer = layer;
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    
    _mode = 0;
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("pickup.caf");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("hit.caf");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("move.caf");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("TileMap.caf");
    
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    _tileMap = new CCTMXTiledMap();
    _tileMap->initWithTMXFile("TileMap.tmx");
    _background = _tileMap->layerNamed("Background");
    _foreground = _tileMap->layerNamed("Foreground");
    
    _meta = _tileMap->layerNamed("Meta");
    _meta->setVisible(false);
    
    this->addChild(_tileMap);
    
    CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("Objects");
    
    if(objectGroup == NULL){
        CCLog("tile map has no objects object layer");
        return false;
    }
    
    CCDictionary *spawnPoint = objectGroup->objectNamed("SpawnPoint");

    int x = ((CCString)*spawnPoint->valueForKey("x")).intValue();
    int y = ((CCString)*spawnPoint->valueForKey("y")).intValue();
    
    _player = new CCSprite();
    _player->initWithFile("Player.png");
    _player->setPosition(ccp(x,y));
    
    this->addChild(_player);
    this->setViewPointCenter(_player->getPosition());
    _enemies = new CCArray();
    _projectiles = new CCArray();
    //Add Enemy
    CCArray *objArray = objectGroup->getObjects();
    CCObject* obj;
    
    CCARRAY_FOREACH(objArray,obj)
    {
        CCDictionary *enemySpawnPoint = (CCDictionary *)obj;

        int enemyType = ((CCString)*enemySpawnPoint->valueForKey("Enemy")).intValue();
        if (enemyType == 1) {
            int x = ((CCString)*enemySpawnPoint->valueForKey("x")).intValue();
            int y = ((CCString)*enemySpawnPoint->valueForKey("y")).intValue();
            CCSprite *enemy = new CCSprite();
            enemy->initWithFile("enemy1.png");
            enemy->setPosition(ccp(x,y));
            this->addChild(enemy);
            this->animateEnemy(enemy);
            _enemies->addObject(enemy);
        }
    }
    

    this->schedule(schedule_selector(HelloWorld::testCollisions));
    testCollisions:
    this->setTouchEnabled(true);
    return true;
}



void HelloWorld::animateEnemy(CCSprite *enemy){
    // speed of the enemy
    // immediately before creating the actions in animateEnemy
    // rotate to face the player
    CCPoint diff = ccpSub(_player->getPosition(),enemy->getPosition());
    float angleRadians = atanf((float)diff.y / (float)diff.x);
    float angleDegrees = CC_RADIANS_TO_DEGREES(angleRadians);
    float cocosAngle = -1 * angleDegrees;
    if (diff.x < 0) {
        cocosAngle += 180;
    }
    enemy->setRotation(cocosAngle);
    
    CCMoveBy *actionBy = CCMoveBy::create(0.3, ccpMult(ccpNormalize(ccpSub(_player->getPosition(),enemy->getPosition())), 10));
    
    CCCallFuncN *instant = new CCCallFuncN;
    instant->initWithTarget(this, callfuncN_selector(HelloWorld::enemyMoveFinished));
    CCSequence* sequence=CCSequence::create(actionBy,instant,NULL);
    
    enemy->runAction(sequence);
    
    // Create the actions
}

void HelloWorld::enemyMoveFinished(CCSprite* pSender)
{
    animateEnemy(pSender);
}


CCPoint HelloWorld::tileCoordForPosition(CCPoint position)
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return ccp(x, y);
}

void HelloWorld::setViewPointCenter(CCPoint position) {
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    int x = MAX(position.x, winSize.width/2);
    int y = MAX(position.y, winSize.height/2);
    CCLog("width = %f",this->_tileMap->getTileSize().width);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    CCPoint actualPosition = ccp(x, y);
    
    CCPoint centerOfView = ccp(winSize.width/2, winSize.height/2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    this->setPosition(viewPoint);
}

#pragma mark - handle touches

void HelloWorld::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

bool HelloWorld::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    return true;
}

void HelloWorld::setPlayerPosition(CCPoint position)
{
    CCPoint tileCoord = this->tileCoordForPosition(position);
    int tileGid = _meta->tileGIDAt(tileCoord);
    if (tileGid) {
        CCDictionary *properties = _tileMap->propertiesForGID(tileGid);
        if (properties) {
            CCString *collision = new CCString();
            *collision = *properties->valueForKey("Collidable");
            if (collision && (collision->compare("True") == 0)) {
                CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("hit.caf");
                return;
            }
            
            CCString *collectible = new CCString();
            *collectible = *properties->valueForKey("Collectable");
            if (collectible && (collectible->compare("True") == 0)) {
                _meta->removeTileAt(tileCoord);
                _foreground->removeTileAt(tileCoord);
                
                _numCollected++;
                _hud->numCollectedChanged(_numCollected);
                CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("pickup.caf");
            }
        }
    }
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("move.caf");

    _player->setPosition(position);
    
    if (_numCollected == 2) {
        this->win();
    }
}

void HelloWorld::win(){
    GameOverScene *gameOverScene = GameOverScene::scene();
    gameOverScene->setString("You Win!");
    
    CCDirector::sharedDirector()->replaceScene(gameOverScene);
}

void HelloWorld::lose(){
    GameOverScene *gameOverScene = GameOverScene::scene();
    gameOverScene->setString("You Lose!");
    
    CCDirector::sharedDirector()->replaceScene(gameOverScene);
}

void HelloWorld::testCollisions(CCTime dt){
    
    CCArray *projectilesToDelete = new CCArray();
    
    CCObject* sprite;
    CCARRAY_FOREACH(_projectiles,sprite)
    {
        CCSprite *projectile = (CCSprite *)sprite;
        
        CCRect projectileRect = CCRectMake(
                                           projectile->getPosition().x - (projectile->getContentSize().width/2),
                                           projectile->getPosition().y - (projectile->getContentSize().height/2),
                                           projectile->getContentSize().width,
                                           projectile->getContentSize().height);
        
        CCArray *targetsToDelete = new CCArray();
        
        CCObject* theTarget;
        CCARRAY_FOREACH(_enemies,theTarget)
        {
            CCSprite *target = (CCSprite *)theTarget;
            CCRect targetRect = CCRectMake(
                                           target->getPosition().x - (target->getContentSize().width/2),
                                           target->getPosition().y - (target->getContentSize().height/2),
                                           target->getContentSize().width,
                                           target->getContentSize().height);
            CCLog("tes......");
        // iterate through enemies, see if any intersect with current projectile
            if (projectileRect.intersectsRect(targetRect)) {
                targetsToDelete->addObject(target);
            }
        }
        

        CCLog("delete count = %d", targetsToDelete->count());
        CCARRAY_FOREACH(targetsToDelete,theTarget)
        {
            CCSprite *target = (CCSprite *)theTarget;
            _enemies->removeObject(target);
            this->removeChild(target, true);
        }
        
        // delete all hit enemies
        
        if (targetsToDelete->count() > 0) {
            // add the projectile to the list of ones to remove
            projectilesToDelete->addObject(projectile);
        }
    }

    // remove all the projectiles that hit.
    CCARRAY_FOREACH(projectilesToDelete, sprite)
    {
        _projectiles->removeObject(sprite);
        this->removeChild((CCSprite *)sprite, true);
    }
    
    
    CCARRAY_FOREACH(_enemies, sprite)
    {
        CCSprite *target = (CCSprite *)sprite;
        CCRect targetRect = CCRectMake(target->getPosition().x-(target->getContentSize().width/2), target->getPosition().y - (target->getContentSize().height/2), target->getContentSize().width, target->getContentSize().height);
        if (targetRect.containsPoint(_player->getPosition())) {
            this->lose();
        }
    }
}

void HelloWorld::projectileMoveFinished(CCSprite *sprite) {
    _projectiles->removeObject(sprite);
    this->removeChild(sprite, true);
}

void HelloWorld::ccTouchEnded(CCTouch *touch, CCEvent *event)
{
    if (_mode == 1) {
        // Find where the touch is
        CCPoint touchLocation = touch->getLocationInView();
        touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
        touchLocation = this->convertToNodeSpace(touchLocation);
        
        // Create a projectile and put it at the player's location
        CCSprite *projectile = new CCSprite();
        projectile->initWithFile("Projectile.png");
        projectile->setPosition(_player->getPosition());
        this->addChild(projectile);
        
        // Determine where we wish to shoot the projectile to
        int realX;
        
        // Are we shooting to the left or right?
        CCPoint diff = ccpSub(touchLocation, _player->getPosition());
        if (diff.x > 0)
        {
            realX = (_tileMap->getMapSize().width * _tileMap->getTileSize().width) +
            (projectile->getContentSize().width/2);
        } else {
            realX =  -(projectile->getContentSize().width/2);
        }
        float ratio = (float) diff.y / (float) diff.x;
        int realY = ((realX - projectile->getPosition().x) * ratio) + projectile->getPosition().y;
        CCPoint realDest = ccp(realX, realY);
        
        // Determine the length of how far we're shooting
        int offRealX = realX - projectile->getPosition().x;
        int offRealY = realY - projectile->getPosition().y;
        float length = sqrtf((offRealX*offRealX) + (offRealY*offRealY));
        float velocity = 480/1; // 480pixels/1sec
        float realMoveDuration = length/velocity;
        
        CCCallFuncN *actionMoveDone = new CCCallFuncN;
        actionMoveDone->initWithTarget(this, callfuncN_selector(HelloWorld::projectileMoveFinished));
        
        CCMoveTo *moveTo = CCMoveTo::create(realMoveDuration, realDest);
        CCSequence* sequence=CCSequence::create(moveTo,actionMoveDone,NULL);
        
        projectile->runAction(sequence);
        
        _projectiles->addObject(projectile);
        return;
    }
    
    CCPoint touchLocation = touch->getLocationInView();
    touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
    touchLocation = this->convertToNodeSpace(touchLocation);
    
    CCPoint playerPos = _player->getPosition();
    CCPoint diff = ccpSub(touchLocation, playerPos);
    
    if ( abs(diff.x) > abs(diff.y) ) {
        if (diff.x > 0) {
            playerPos.x += _tileMap->getTileSize().width;
        } else {
            playerPos.x -= _tileMap->getTileSize().width;
        }
    } else {
        if (diff.y > 0) {
            playerPos.y += _tileMap->getTileSize().height;
        } else {
            playerPos.y -= _tileMap->getTileSize().height;
        }
    }
    
    // safety check on the bounds of the map
    if (playerPos.x <= (_tileMap->getMapSize().width * _tileMap->getTileSize().width) &&
        playerPos.y <= (_tileMap->getMapSize().height * _tileMap->getTileSize().height) &&
        playerPos.y >= 0 &&
        playerPos.x >= 0 )
    {
        this->setPlayerPosition(playerPos);
    }
    
    this->setViewPointCenter(_player->getPosition());
}