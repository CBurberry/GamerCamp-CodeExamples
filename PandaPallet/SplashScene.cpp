#include "SplashScene.h"
#include "MenuScene.h"
#include "ui/UIVideoPlayer.h"

#define SPLASH_DURATION 12.0f

using namespace cocos2d;

SplashScene::~SplashScene()
{
	stopAllActions();
	removeAllChildren();
	removeFromParent();
}


cocos2d::Scene* SplashScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SplashScene::create();

	// add layer as a child to scene
	scene->addChild( layer );

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool SplashScene::init()
{
	if ( !Layer::init() )
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	m_cBackgroundSpriteA = Sprite::create( "Loose/logoinverted.png" );
	m_cBackgroundSpriteA->setName("SplashA");
	m_cBackgroundSpriteA->setPositionNormalized( Vec2(0.5f, 0.5f) );
	m_cBackgroundSpriteA->setCascadeOpacityEnabled(true);
	m_cBackgroundSpriteA->setOpacity(0);

	m_cBackgroundSpriteB = Sprite::create( "Loose/logo.png" );
	m_cBackgroundSpriteB->setName( "SplashB" );
	m_cBackgroundSpriteB->setPositionNormalized( Vec2( 0.5f, 0.5f ) );
	m_cBackgroundSpriteB->setCascadeOpacityEnabled( true );
	m_cBackgroundSpriteB->setOpacity( 0 );

	return true;
}

void SplashScene::onEnter()
{
	Layer::onEnter();

	this->addChild( m_cBackgroundSpriteA, 0 );
	this->addChild( m_cBackgroundSpriteB, 0 );
	auto fadeIn = FadeIn::create( 1.0f );
	auto delay = DelayTime::create(1.0f);
	auto fadeOut = FadeOut::create(1.5f);

	auto CB_PlayVideo = CallFunc::create( [&] () 
	{
		//Cocos2DX VideoPlayer class is experiemental and only works on mobile operating systems.
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			const cocos2d::Size visibleSize( cocos2d::Director::getInstance()->getVisibleSize() );
			auto videoPlayer = cocos2d::experimental::ui::VideoPlayer::create();
			videoPlayer->setContentSize( visibleSize );
			videoPlayer->setAnchorPoint( cocos2d::Vec2::ANCHOR_MIDDLE );
			videoPlayer->setPosition( visibleSize / 2 );
			videoPlayer->setFileName( "Video/video.mp4" );
			videoPlayer->setName( "VideoPlayer" );
			this->addChild( videoPlayer );
			videoPlayer->play();
		#endif

		const float k_fVideoDuration = 5.0f;
		this->scheduleOnce( schedule_selector( SplashScene::secondSplash ), k_fVideoDuration );
	});
	auto seq = Sequence::create( fadeIn, delay, fadeOut, CB_PlayVideo, nullptr );

	//Run first splash screen sequence
	m_cBackgroundSpriteA->runAction( seq );
}


void SplashScene::secondSplash( float dt )
{
	Sprite* SplashScreen = static_cast< Sprite* >( getChildByName("SplashB") );
	//Run second spash screen instance
	auto delay = DelayTime::create( 1.0f );
	auto delay2 = DelayTime::create( 0.5f );
	auto fadeIn = FadeIn::create( 1.0f );
	auto fadeOut = FadeOut::create( 1.5f );
	auto seq2 = Sequence::create( delay2, fadeIn, delay, fadeOut, nullptr );

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::experimental::ui::VideoPlayer* videoPlayer = static_cast< cocos2d::experimental::ui::VideoPlayer* >( getChildByName("VideoPlayer") );
	videoPlayer->stop();
	videoPlayer->setVisible( false );
#endif

	SplashScreen->runAction( seq2 );
	const float k_fSecondSplashDuration = 4.0f;

	//Wait for SPLASH_DURATION seconds to load main scene
	this->scheduleOnce( schedule_selector( SplashScene::finishSplash ), k_fSecondSplashDuration );
}

void SplashScene::finishSplash( float dt )
{
	// ... do whatever other initializations here
	// Show the actual main scene

	// create and initialize a label
	Label* pLabel = Label::createWithTTF( "FINISHED SPLASH", "fonts/arial.ttf", 24 );
	pLabel->setTextColor( Color4B( 255, 0, 0, 255 ) );
	pLabel->setPositionNormalized( Vec2(0.5f, 0.8f) );
	this->addChild( pLabel, 1 );

	Director::getInstance()->replaceScene( TransitionFade::create( 1.5f, CMenuLayer::scene() ) );
}
