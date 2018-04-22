#include "VirtualControlLayer.h"
#include "GamerCamp/GameSpecific/Player/PlayerCustom.h"
#include "GamerCamp/GameSpecific/GameLayerEmpty.h"
#include "GamerCamp/GameSpecific/GameManager.h"

USING_NS_CC;

#define COCOS2D_DEBUG 1

CVirtualControlLayer::~CVirtualControlLayer()
{
	stopAllActions();
	removeAllChildren();
	removeFromParent();
}

bool CVirtualControlLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if( !Layer::init() )
	{
		return false;
	}

	Size visibleSize		= Director::getInstance()->getVisibleSize();
	Size halfVisibleSize	= Size( visibleSize.width / 2.0f, visibleSize.height );
	Vec2 origin				= Director::getInstance()->getVisibleOrigin();
	Vec2 rightOrigin		= Vec2( visibleSize.width / 2.0f, origin.y );
	m_sScreenRect			= Rect( origin, visibleSize );
	m_sLeftScreenRect		= Rect( origin, halfVisibleSize );
	m_sRightScreenRect		= Rect( rightOrigin, halfVisibleSize );

	//Add a screen listener with touch callbacks. 
	auto listenerScreen = EventListenerTouchOneByOne::create();
	listenerScreen->setEnabled( true );
	listenerScreen->setSwallowTouches( true );
	listenerScreen->onTouchBegan = CC_CALLBACK_2( CVirtualControlLayer::onTouchBegan, this );
	listenerScreen->onTouchMoved = CC_CALLBACK_2( CVirtualControlLayer::onTouchMoved, this );
	listenerScreen->onTouchEnded = CC_CALLBACK_2( CVirtualControlLayer::onTouchEnded, this );
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority( listenerScreen, this );
	return true;
}

bool CVirtualControlLayer::onTouchBegan( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent )
{
	Vec2 sTouchPoint			= pcTouch->getLocationInView();
	bool bIsScreenTouch			= m_sScreenRect.containsPoint( sTouchPoint );
	bool bIsLeftScreenTouch		= m_sLeftScreenRect.containsPoint( sTouchPoint );
	bool bIsRightScreenTouch	= m_sRightScreenRect.containsPoint( sTouchPoint );

	if ( bIsScreenTouch ) 
	{
		m_pPlayerPtr->SetJumpVirtual( true );
	}
	if ( bIsLeftScreenTouch )
	{
		m_pPlayerPtr->SetLeftVirtual( true );
		return true;
	}
	else if ( bIsRightScreenTouch ) 
	{
		m_pPlayerPtr->SetRightVirtual( true );
		return true;
	}

	return false;
}


void CVirtualControlLayer::onTouchMoved( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent )
{
	Vec2 sTouchPoint = pcTouch->getLocationInView();
	bool bIsScreenTouch = m_sScreenRect.containsPoint( sTouchPoint );
	bool bIsLeftScreenTouch = m_sLeftScreenRect.containsPoint( sTouchPoint );
	bool bIsRightScreenTouch = m_sRightScreenRect.containsPoint( sTouchPoint );

	m_pPlayerPtr->SetJumpVirtual( bIsScreenTouch );
	m_pPlayerPtr->SetLeftVirtual( bIsLeftScreenTouch );
	m_pPlayerPtr->SetRightVirtual( bIsRightScreenTouch );
}


void CVirtualControlLayer::onTouchEnded( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent )
{
	m_pPlayerPtr->SetJumpVirtual( false );
	m_pPlayerPtr->SetLeftVirtual( false );
	m_pPlayerPtr->SetRightVirtual( false );
}

void CVirtualControlLayer::onTouchCancelled( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent )
{
	m_pPlayerPtr->SetJumpVirtual( false );
	m_pPlayerPtr->SetLeftVirtual( false );
	m_pPlayerPtr->SetRightVirtual( false );
}


void CVirtualControlLayer::SetButtonsActive( bool bParam )
{
	for ( cocos2d::Node* elem : this->getChildren() )
	{
		ui::Button* pcButton = static_cast< ui::Button* >( elem );
		if ( pcButton )
		{
			pcButton->setTouchEnabled( bParam );
		}
	}
}
