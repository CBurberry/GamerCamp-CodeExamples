#include "LoadingScreenScene.h"
#include "GameManager.h"
#include "ResourceManager.h"
#include "cocos2d.h"


USING_NS_CC;

CLoadingSceneScreen::CLoadingSceneScreen()
{
	//Initialise the image screen.
	m_pcLoadingScreenImage = Sprite::create( "Loose/LoadingScreen.png" );
	m_pcLoadingScreenImage->setOpacity( 255 );
	this->addChild( m_pcLoadingScreenImage, 0 );
}

CLoadingSceneScreen::~CLoadingSceneScreen()
{
	stopAllActions();
	removeAllChildren();
	removeFromParent();
}

void CLoadingSceneScreen::onEnter()
{
	Node::onEnter();
}

void CLoadingSceneScreen::Init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Set properties of the LOADING image
	m_pcLoadingScreenImage->setPosition( Vec2( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ) );
	m_pcLoadingScreenImage->setContentSize( visibleSize );
	m_pcLoadingScreenImage->setCascadeOpacityEnabled( true );

	// Determine the current region and load the associated assets.
	int iRegion = CGameManager::GetInstance().GetCurrentRegion();
	if ( iRegion == CLevelData::ERegion::China )
	{
		CResourceManager::LoadChinaAssets();
	}
	else if ( iRegion == CLevelData::ERegion::France )
	{
		CResourceManager::LoadFranceAssets();
	}

	//Run end load callback in sequence after successful loading.
	CallFunc* pcCallbackFunc = CallFunc::create( this, callfunc_selector( CLoadingSceneScreen::LoadScreenEnded ) );
	m_pcLoadingScreenImage->runAction( pcCallbackFunc );
}

void CLoadingSceneScreen::LoadScreenEnded()
{
	CGameManager::GetInstance().OpenLevelScene();
}

