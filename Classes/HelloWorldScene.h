#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

USING_NS_CC;

#define PTM_RATIO 32.0
#define TAG_CIRCLE 10
#define TAG_LABEL  20
#define BALL_RADIUS 50.0
#define SHOT_POWER 10.0

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    virtual void update(float dt);
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    /** Touch listener */
    void enableTouchEvent(bool enabled);
    CC_SYNTHESIZE(EventListenerTouchOneByOne*, _touchListener, TouchListener);
    // touch delegate
    virtual bool onTouchBegan(Touch *touch, Event *event);
    virtual void onTouchMoved(Touch *touch, Event *event);
    virtual void onTouchEnded(Touch *touch, Event *event);
    virtual void onTouchCancelled(Touch *touch, Event *event);
    
    void initPhysics();
    Size _winSize;
    Touch* _touch;
    Vec2 _touchStartPos;
    void placeCue(Vec2 position);
    Sprite* _arrow;
    float _pullBack;
    b2World* _world;
    Sprite* _ball;
    b2Body* _ballBody;
};

#endif // __HELLOWORLD_SCENE_H__
