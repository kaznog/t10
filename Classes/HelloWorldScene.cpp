#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    
    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    _winSize = Director::getInstance()->getWinSize();
    auto drawNode = DrawNode::create();
    drawNode->drawSegment(Point(0, _winSize.height/2), Point(_winSize.width, _winSize.height/2), 1, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    drawNode->drawSegment(Point(_winSize.width/2, _winSize.height), Point(_winSize.width/2, 0), 1, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    
    Point centerPoint = Point(_winSize.width/2, _winSize.height/2);
    float R = 128;
    std::vector<Color4F> colors = {
        Color4F(1.0f, 0.0f, 0.0f, 0.6f), Color4F(0.0f, 1.0f, 0.0f, 0.6f),
        Color4F(0.0f, 0.0f, 1.0f, 0.6f), Color4F(0.5f, 0.5f, 0.0f, 0.6f),
        Color4F(0.0f, 0.5f, 0.5f, 0.6f), Color4F(0.5f, 0.5f, 0.5f, 0.5f),
    };
    for (float angles = 0, i = 0; angles < 360; angles += 60, i++) {
        float radian = CC_DEGREES_TO_RADIANS(angles + 90);
        float x = R * -cos(radian);
        float y = R * sin(radian);
        drawNode->drawDot(centerPoint + Point(x, y), R/2, colors[i]);
        auto label = LabelTTF::create("angle:" + std::to_string((int)(angles)), "Arial", 24);
        label->setPosition(centerPoint + Point(x, y));
        drawNode->addChild(label);
    }
    this->addChild(drawNode, 1, 1);
    
    _arrow = Sprite::create("Arrow.png");
    this->addChild(_arrow);
    _arrow->setPosition(Vec2(_winSize.width/2, _winSize.height/2));
    _arrow->setVisible(false);
    
    this->initPhysics();
    this->scheduleUpdate();
    this->enableTouchEvent(true);
    
    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif
    
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

#pragma mark touch events
void HelloWorld::enableTouchEvent(bool enabled)
{
    if (this->_touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(this->_touchListener);
        this->_touchListener = nullptr;
    }
    if (enabled) {
        this->_touchListener = EventListenerTouchOneByOne::create();
        _touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
        _touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
        _touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
        _touchListener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, this);
    }
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *event)
{
    Vec2 location = touch->getLocation();
    Rect touchRect = Rect(location.x, location.y, 1, 1);
    if (_ball->getBoundingBox().intersectsRect(touchRect)) {
        _touch = touch;
        _arrow->setPosition(_ball->getPosition());
        float angle = CC_RADIANS_TO_DEGREES( atan2((location.x - _ball->getPosition().x), (location.y - _ball->getPosition().y)) );
        angle = fmodf(180.0 + angle, 360.0f);
        _arrow->setRotation(angle);
        float distance = _ball->getPosition().distance(location);
        float scale = distance / 64.0f;
        if (scale < 1.0f) {
            scale = 1.0f;
        }
        _arrow->setScaleY(scale);
        _arrow->setVisible(true);
    }
    
    return true;
}

void HelloWorld::onTouchMoved(Touch *touch, Event *event)
{
    Vec2 location = touch->getLocation();
    
    placeCue(location);
}

void HelloWorld::onTouchEnded(Touch *touch, Event *event)
{
    if (_touch) {
        Vec2 location = touch->getLocation();
        float distance = _ball->getPosition().distance(location);
        _pullBack = distance;
        float angle = _arrow->getRotation();
        log("TouchEnded angle: %f", angle);
        float radian = CC_DEGREES_TO_RADIANS(angle + 90);
        float x = (_pullBack * -cos(radian)) * SHOT_POWER;
        float y = (_pullBack * sin(radian)) * SHOT_POWER;
        log("x: %f", x);
        log("y: %f", y);
        
        _ballBody->ApplyLinearImpulse(b2Vec2(x, y), _ballBody->GetWorldCenter(), true);
    }
    _arrow->setVisible(false);
    _touch = nullptr;
}

void HelloWorld::onTouchCancelled(Touch *touch, Event *event)
{
    this->onTouchEnded(touch, event);
}

#pragma mark -

void HelloWorld::placeCue(Vec2 position) {
    float angle = CC_RADIANS_TO_DEGREES( atan2((position.x - _ball->getPosition().x), (position.y - _ball->getPosition().y)) );
    angle = fmodf(180.0 + angle, 360.0f);
    
    float distance = _ball->getPosition().distance(position);
    _pullBack = distance * 0.5f;
    float scale = distance / 64.0f;
    if (scale < 1.0f) {
        scale = 1.0f;
    }
    _arrow->setRotation(angle);
    _arrow->setScaleY(scale);
}

void HelloWorld::initPhysics()
{
    Size winSize = Director::getInstance()->getWinSize();
    // 重力を作成
    //    b2Vec2 gravity = b2Vec2(0.0f, -9.8f);
    b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
    // Worldを作成
    _world = new b2World(gravity);
    _world->SetAllowSleeping(true);
    _world->SetContinuousPhysics(true);
    _ball = Sprite::create("ball.png");
    _ball->setPosition(Point(_winSize.width/2, _winSize.height/2));
    this->addChild(_ball);
    
    // b2BodyDef構造体
    b2BodyDef bodyDef;
    /**
     * 動的な剛体とする
     * 動かない静的な剛体の場合は b2_staticBodyにする
     * 静的な物体に設定すると、重力やその他の力を受けてもまったく動かなくなります
     * つまり、壁や地面は静的に設定すればいいわけです
     *  デフォルトでは静的に設定されているので注意
     */
    bodyDef.type = b2_dynamicBody;
    /**
     * userData にスプライトを登録
     * Box2Dに用意されているuserDataは、いわば「なにを入れてもいい変数」です。
     * 個々のアプリケーションのオブジェクトとBodyを結びつけるためだけに用意されたものなので、
     * 使用する状況に応じてなにを入れたってかまいません。
     * userDataにスプライトを入れただけですから、勝手にうごくわけありません。
     */
    bodyDef.userData = _ball;
    /**
     * 位置を設定
     * ここでは、左下から(300px, 300px)の位置に物体があることにする
     */
    bodyDef.position.Set((_winSize.width/2)/PTM_RATIO, (_winSize.height/2)/PTM_RATIO);
    // WorldからBodyを作成
    _ballBody = _world->CreateBody(&bodyDef);
    _ballBody->SetSleepingAllowed(true);
    _ballBody->SetLinearDamping(1.2);
    _ballBody->SetAngularDamping(0.8);
    
    /**
     * 形を定義するクラス
     * ここでは円形を定義するクラスを用いる
     * Circleクラスには「m_p」というBodyとのズレを設定できる
     * .m_p.Set(50.0/PTM_RATIO, 50.0/PTM_RATIO);   //Bodyとのズレ
     * Bodyからx,y方向それぞれ50pxずれている半径50pxの円を作成することになる
     */
    b2CircleShape circle;
    /**
     * 半径50pxの円形とする
     */
    circle.m_radius = BALL_RADIUS / PTM_RATIO;
    
    /**
     * 物体の性質を定義するのに「Fixture」として定義します
     * Fixtureが持つ情報は、
     * ・形     (Spahe)
     * ・密度    (Density)
     * ・摩擦率  (Friction)
     * ・反発係数(Restitution)
     */
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;     // 形
    fixtureDef.density = 5.0f;      // 密度
    fixtureDef.friction = 0.5f;     // 摩擦率
    fixtureDef.restitution = 0.7f;  // 反発係数
    
    /**
     * BodyからFixtureを作成
     * 形だけは b2CircleSpaheなどのb2Shapeクラスを継承した形状クラスを作成して
     * Fixture構造体に設定する必要がある
     */
    _ballBody->CreateFixture(&fixtureDef);
    //    body->ApplyLinearImpulse(b2Vec2(100, 100), body->GetWorldCenter(), true);
    //    body->ApplyForce(b2Vec2(10, 10), body->GetWorldCenter(), true);
    {
        // 長方形を作成
        b2PolygonShape rect;
        /**
         * 横 画面幅px
         * 縦 100px
         * の長方形とする
         */
        rect.SetAsBox((winSize.width/2) / PTM_RATIO, 50 / PTM_RATIO);
        
        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape = &rect;    // 長方形
        boxFixtureDef.density = 0.4;    // 密度
        boxFixtureDef.friction = 0.5;   // 摩擦率
        boxFixtureDef.restitution = 1.0;// 反発係数
        
        b2BodyDef boxBodyDef;
        boxBodyDef.type = b2_staticBody;
        //        boxBodyDef.position.Set((winSize.width/2) / PTM_RATIO, 50 / PTM_RATIO);
        boxBodyDef.position.Set((winSize.width/2) / PTM_RATIO, -50 / PTM_RATIO);
        b2Body* boxBody = _world->CreateBody(&boxBodyDef);
        
        // BodyからFixtureを作成
        boxBody->CreateFixture(&boxFixtureDef);
    }
    {
        // 長方形を作成
        b2PolygonShape rect;
        /**
         * 横 画面幅px
         * 縦 100px
         * の長方形とする
         */
        rect.SetAsBox((winSize.width/2) / PTM_RATIO, 50 / PTM_RATIO);
        
        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape = &rect;    // 長方形
        boxFixtureDef.density = 0.4;    // 密度
        boxFixtureDef.friction = 0.5;   // 摩擦率
        boxFixtureDef.restitution = 1.0;// 反発係数
        
        b2BodyDef boxBodyDef;
        boxBodyDef.type = b2_staticBody;
        //        boxBodyDef.position.Set((winSize.width/2) / PTM_RATIO, 50 / PTM_RATIO);
        boxBodyDef.position.Set((winSize.width/2) / PTM_RATIO, (winSize.height + 50) / PTM_RATIO);
        b2Body* boxBody = _world->CreateBody(&boxBodyDef);
        
        // BodyからFixtureを作成
        boxBody->CreateFixture(&boxFixtureDef);
    }
    {
        // 長方形を作成
        b2PolygonShape rect;
        /**
         * 横 100px
         * 縦 画面高px
         * の長方形とする
         */
        rect.SetAsBox(50 / PTM_RATIO, (winSize.height/2) / PTM_RATIO);
        
        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape = &rect;    // 長方形
        boxFixtureDef.density = 0.4;    // 密度
        boxFixtureDef.friction = 0.5;   // 摩擦率
        boxFixtureDef.restitution = 1.0;// 反発係数
        
        b2BodyDef boxBodyDef;
        boxBodyDef.type = b2_staticBody;
        boxBodyDef.position.Set(-50 / PTM_RATIO, (winSize.height/2) / PTM_RATIO);
        b2Body* boxBody = _world->CreateBody(&boxBodyDef);
        
        // BodyからFixtureを作成
        boxBody->CreateFixture(&boxFixtureDef);
    }
    {
        // 長方形を作成
        b2PolygonShape rect;
        /**
         * 横 100px
         * 縦 画面高px
         * の長方形とする
         */
        rect.SetAsBox(50 / PTM_RATIO, (winSize.height/2) / PTM_RATIO);
        
        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape = &rect;    // 長方形
        boxFixtureDef.density = 0.4;    // 密度
        boxFixtureDef.friction = 0.5;   // 摩擦率
        boxFixtureDef.restitution = 1.0;// 反発係数
        
        b2BodyDef boxBodyDef;
        boxBodyDef.type = b2_staticBody;
        boxBodyDef.position.Set((winSize.width + 50) / PTM_RATIO, (winSize.height/2) / PTM_RATIO);
        b2Body* boxBody = _world->CreateBody(&boxBodyDef);
        
        // BodyからFixtureを作成
        boxBody->CreateFixture(&boxFixtureDef);
    }
    
}
void HelloWorld::update(float dt)
{
    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 10;
    int32 positionIterations = 10;
    _world->Step(timeStep, velocityIterations, positionIterations);
    for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext()) {
        /**
         * 登録されている動的剛体が停止した場合は、
         * IsAwake == falseになるらしい
         */
        if (b->GetType() == b2BodyType::b2_dynamicBody && b->IsAwake() == false) {
            //            b->Dump();
        }
        if (b->GetUserData() != nullptr) {
            Sprite* actor = (Sprite*)b->GetUserData();
            actor->setPosition(Point(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
            /**
             * Box2Dでは角度は時計回りに360°法ですが、
             * Cocos2d-xは反時計回りにラジアン法を用いてるので、
             * CC_RADIANS_TO_DEGREESマクロを使って変換してやる必要があります
             */
            actor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
            b2Vec2 liner = b->GetLinearVelocity();
            float magnitude = pow(liner.x, 2) + pow(liner.y, 2);
            if (magnitude < 0.5f) {
                b->ResetMassData();
                b->SetLinearVelocity(b2Vec2_zero);
                b->SetAngularVelocity(0);
            }
        }
    }
}