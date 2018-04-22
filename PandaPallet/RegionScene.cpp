#include "RegionScene.h"

USING_NS_CC;

CRegionScene::~CRegionScene()
{
	stopAllActions();
	removeAllChildren();
	removeFromParent();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
Scene* CRegionScene::scene()
{
	// 'scene' is an autorelease object
	Scene *scene = Scene::create();

	// 'layer' is an autorelease object
	CRegionScene *layer = CRegionScene::create();

	// add layer as a child to scene
	scene->addChild( layer );

	// return the scene
	return scene;
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool CRegionScene::init()
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

	Sprite* pcBackButton		= Sprite::createWithSpriteFrameName( "BackButtonMenu.png" );
	Sprite* pcBackButtonPressed = Sprite::createWithSpriteFrameName( "BackButtonMenuPressed.png" );

	// add a "close" icon to exit the progress. it's an autorelease object
	MenuItemImage* pOpenLevelSelection1 = MenuItemImage::create(
		"Loose/RegionSelect/CloudyMountains.png",
		"Loose/RegionSelect/CloudyMountainsPressed.png",
		CC_CALLBACK_1( CRegionScene::CB_OpenLevelSelection, this, CLevelData::ERegion::China ) );
	Vec2 v2Region1Position = Vec2( origin.x + ( visibleSize.width * 0.5f ),
		origin.y + ( visibleSize.height * 0.75f ) );
	pOpenLevelSelection1->setPosition( v2Region1Position );
	pOpenLevelSelection1->setScale( Director::getInstance()->getContentScaleFactor() - 0.1f );


	MenuItemImage* pOpenLevelSelection2 = nullptr;
	if( CGameManager::GetInstance().IsRegionUnlocked( 1 ) )
	{
		pOpenLevelSelection2 = MenuItemImage::create(
			"Loose/RegionSelect/BroadwalkBeach.png",
			"Loose/RegionSelect/BroadwalkBeachPressed.png",
			CC_CALLBACK_1( CRegionScene::CB_OpenLevelSelection, this, CLevelData::ERegion::France ) );
		

	}
	else
	{
		pOpenLevelSelection2 = MenuItemImage::create(
			"Loose/RegionSelect/BroadwalkLocked.png",
			"Loose/RegionSelect/BroadwalkLocked.png" );
	}

	Vec2 v2Region2Position = Vec2( origin.x + ( visibleSize.width * 0.5f ),
		origin.y + ( visibleSize.height * 0.25f ) );
	pOpenLevelSelection2->setPosition( v2Region2Position );
	pOpenLevelSelection2->setScale( Director::getInstance()->getContentScaleFactor() - 0.1f );

	MenuItemSprite* pReturnToMenu = MenuItemSprite::create(
		pcBackButton,
		pcBackButtonPressed,
		CC_CALLBACK_1( CRegionScene::CB_ReturnToMenu, this ) );

	pReturnToMenu->setScale( Director::getInstance()->getContentScaleFactor() );
	pReturnToMenu->setPosition( Vec2(	origin.x + ( visibleSize.width * 0.05f ),
										origin.y + ( visibleSize.height * 0.93f ) ) );

	// create menu, it's an autorelease object
	Menu* pMenu = Menu::create(  pReturnToMenu, pOpenLevelSelection1, pOpenLevelSelection2, NULL );
	pMenu->setPosition( Vec2::ZERO );
	this->addChild( pMenu, 1 );

	//// add "HelloWorld" splash screen"
	//Sprite* pSprite = Sprite::create( "Loose/HelloWorld.png" );

	//// position the sprite on the center of the screen
	//pSprite->setPosition( Vec2( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ) );

	//// add the sprite as a child to this layer
	//this->addChild( pSprite, 0 );

	return true;
}

//Callback Function to load level selection screen
void CRegionScene::CB_OpenLevelSelection( Ref* pSender, CLevelData::ERegion iSelection )
{
	CGameManager::GetInstance().LoadLevelSelectScene( iSelection );
}

//Callback Function to return to main menu screen
void CRegionScene::CB_ReturnToMenu( Ref* pSender )
{
	CGameManager::GetInstance().LoadMainMenuScene();
}
