#include "TutorialLayer.h"
#include "base/ccTypes.h"

USING_NS_CC;

CTutorialLayer::~CTutorialLayer()
{
	stopAllActions();
	removeAllChildren();
	removeFromParent();
}

void CTutorialLayer::Init()
{
	//Init superclass with a color (clear & transparent)
	if ( !LayerColor::initWithColor( Color4B( 0, 0, 0, 0 ) ) )
	{
		return;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//Define the image properties
	Sprite* pcTutorialImage = Sprite::create("Loose/TutorialImages/TutorialTouch.png");
	pcTutorialImage->setPosition( Vec2 (	origin.x + ( visibleSize.width * 0.5f ),
											origin.y + ( visibleSize.height * 0.5f ) ) );
	pcTutorialImage->setContentSize( visibleSize );
	pcTutorialImage->setCascadeOpacityEnabled( true );
	pcTutorialImage->setName("TutorialTouch");

	//Add a screen listener with touch callbacks. 
	auto listenerScreen = EventListenerTouchOneByOne::create();
	listenerScreen->setEnabled( true );
	listenerScreen->setSwallowTouches( false );
	listenerScreen->onTouchBegan = CC_CALLBACK_2( CTutorialLayer::onTouchBegan, this );
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority( listenerScreen, this );

	//Layer properties
	this->addChild( pcTutorialImage, 1 );
}


bool CTutorialLayer::onTouchBegan( cocos2d::Touch * pcTouch, cocos2d::Event * pcEvent )
{
	auto fadeOut = FadeOut::create(1.0f);
	auto CB_RemoveSelf = CallFunc::create( [&] () 
	{
		this->removeFromParentAndCleanup( true );
	});
	auto sequence = Sequence::create( fadeOut, CB_RemoveSelf, nullptr );
	Sprite* pcSprite = static_cast< Sprite* >( getChildByName( "TutorialTouch" ) );
	pcSprite->runAction( sequence );
	return true;
}