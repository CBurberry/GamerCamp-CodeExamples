#include "OptionsScreenLayer.h"
#include "ui/UIButton.h"
#include "GamerCamp/GameSpecific/GameManager.h"
#include "GamerCamp/GameSpecific/GameLayerEmpty.h"
#include "MenuScene.h"


COptionsScreenLayer::~COptionsScreenLayer()
{
	stopAllActions();
	removeAllChildren();
	removeFromParent();
}

void COptionsScreenLayer::Init()
{
	//Init superclass with a color (clear & transparent)
	if ( !LayerColor::initWithColor( Color4B( 0, 0, 0, 0 ) ) )
	{
		return;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	m_bIsOptionsVisible = false;

	//Add the pause button - visible during play
	ui::Button* pcOptionsButton = ui::Button::create(
		km_pszSettingsIconIdle,
		km_pszSettingsIconPressed,
		"",
		ui::Widget::TextureResType::PLIST );
	pcOptionsButton->addTouchEventListener( [&] ( Ref* sender, ui::Widget::TouchEventType type )
	{
		switch ( type )
		{
		case ui::Widget::TouchEventType::BEGAN:
			Toggle( m_bIsOptionsVisible );
			break;
		default:
			//Do nothing
			break;
		}
	} );

	pcOptionsButton->setPosition( Vec2 ( origin.x + ( visibleSize.width * 0.05f ),
		origin.y + ( visibleSize.height * 0.93f ) ) );
	pcOptionsButton->setCascadeOpacityEnabled( true );
	pcOptionsButton->ignoreContentAdaptWithSize( false );
	pcOptionsButton->setScale( Director::getInstance()->getContentScaleFactor() );
	pcOptionsButton->setTouchEnabled( true );
	pcOptionsButton->setVisible( true );
	pcOptionsButton->setName( "OptionsButton" );

	//Add pause screen visual elements
	Sprite* pcSprite = Sprite::create( "Loose/OptionsScreen.png" );
	pcSprite->setPosition( Vec2( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ) );
	pcSprite->setContentSize( visibleSize );
	pcSprite->setCascadeOpacityEnabled( true );
	pcSprite->setOpacity( 255 );
	pcSprite->setName( "Background" );
	pcSprite->setVisible( false );

	ui::Slider* pcMusicSlider = ui::Slider::create();
	pcMusicSlider->loadBarTexture( "SliderBar.png", ui::Widget::TextureResType::PLIST );
	pcMusicSlider->loadSlidBallTextureNormal( "Slider.png", ui::Widget::TextureResType::PLIST );
	pcMusicSlider->loadSlidBallTexturePressed( "SliderPressed.png", ui::Widget::TextureResType::PLIST );
	pcMusicSlider->loadProgressBarTexture( "SliderBarFilled.png", ui::Widget::TextureResType::PLIST );
	pcMusicSlider->setPosition( Vec2( visibleSize.width * 0.55f, visibleSize.height * 0.46f ) );
	pcMusicSlider->setTouchEnabled( true );
	pcMusicSlider->setCascadeOpacityEnabled( true );
	pcMusicSlider->ignoreContentAdaptWithSize( false );
	pcMusicSlider->setScale( Director::getInstance()->getContentScaleFactor() );
	pcMusicSlider->setVisible( false );
	pcMusicSlider->setName( "MusicSlider" );
	pcMusicSlider->setPercent( CGameManager::GetInstance().GetBackgroundVolume() * 100.0f );
	pcMusicSlider->addEventListener( CC_CALLBACK_2( COptionsScreenLayer::CB_MusicSliderEvent, this ) );

	ui::Slider* pcEffectSlider = ui::Slider::create();
	pcEffectSlider->loadBarTexture( "SliderBar.png", ui::Widget::TextureResType::PLIST );
	pcEffectSlider->loadSlidBallTextureNormal( "Slider.png", ui::Widget::TextureResType::PLIST );
	pcEffectSlider->loadSlidBallTexturePressed( "SliderPressed.png", ui::Widget::TextureResType::PLIST );
	pcEffectSlider->loadProgressBarTexture( "SliderBarFilled.png", ui::Widget::TextureResType::PLIST );
	pcEffectSlider->setPosition( Vec2( visibleSize.width * 0.55f, visibleSize.height * 0.575f ) );
	pcEffectSlider->setTouchEnabled( true );
	pcEffectSlider->setCascadeOpacityEnabled( true );
	pcEffectSlider->ignoreContentAdaptWithSize( false );
	pcEffectSlider->setScale( Director::getInstance()->getContentScaleFactor() );
	pcEffectSlider->setVisible( false );
	pcEffectSlider->setName( "EffectSlider" );
	pcEffectSlider->setPercent( CGameManager::GetInstance().GetSfxVolume() * 100.0f );
	pcEffectSlider->addEventListener( CC_CALLBACK_2( COptionsScreenLayer::CB_EffectSliderEvent, this ) );


	//Add all visual elements as children to the layer
	this->addChild( pcSprite, 1 );
	this->addChild( pcOptionsButton, 1 );
	this->addChild( pcMusicSlider, 2 );
	this->addChild( pcEffectSlider, 2 );
}

void COptionsScreenLayer::Activate()
{
	//Get UI elements
	ui::Button* pcSettingsButton = static_cast< ui::Button* >( getChildByName( "OptionsButton" ) );
	ui::Slider* pcMusicSlider = static_cast< ui::Slider* >( getChildByName( "MusicSlider" ) );
	ui::Slider* pcEffectSlider = static_cast< ui::Slider* >( getChildByName( "EffectSlider" ) );
	Sprite* pcBackground = static_cast< Sprite* >( getChildByName( "Background" ) );

	//Set paused elements visible/invisible
	pcMusicSlider->setVisible( m_bIsOptionsVisible );
	pcEffectSlider->setVisible( m_bIsOptionsVisible );
	pcBackground->setVisible( m_bIsOptionsVisible );

	//Pause specific behaviour
	if ( m_bIsOptionsVisible )
	{
		setOpacity( 175 );
	}
	else
	{
		setOpacity( 0 );
	}
}

void COptionsScreenLayer::Reset()
{
	Sprite* pcBackground = static_cast< Sprite* >( getChildByName( "Background" ) );
	ui::Slider* pcMusicSlider = static_cast< ui::Slider* >( getChildByName( "MusicSlider" ) );
	ui::Slider* pcEffectSlider = static_cast< ui::Slider* >( getChildByName( "EffectSlider" ) );
	pcMusicSlider->setVisible( false );
	pcEffectSlider->setVisible( false );
	pcBackground->setVisible( false );
	m_bIsOptionsVisible = false;
	setOpacity( 0 );
}

void COptionsScreenLayer::Toggle( bool bPauseStatus )
{
	//Invert the pause status
	m_bIsOptionsVisible = !bPauseStatus;
	//Here we assume that only CMenuLayer is the parent to the options screen
	CMenuLayer* pcMenuParent = static_cast< CMenuLayer* >( this->getParent() );
	pcMenuParent->SetMenuEnabled( bPauseStatus );
	Activate();
}

void COptionsScreenLayer::CB_MusicSliderEvent( Ref * pSender, ui::Slider::EventType type )
{
	if ( type == ui::Slider::EventType::ON_PERCENTAGE_CHANGED )
	{
		ui::Slider* pcSlider = static_cast<ui::Slider*>( pSender );
		float fNormalisedVolume = ( ( float ) pcSlider->getPercent() ) / 100.0f;
		CGameManager::GetInstance().SetBackgroundVolume( fNormalisedVolume );
	}
}

void COptionsScreenLayer::CB_EffectSliderEvent( Ref * pSender, ui::Slider::EventType type )
{
	if ( type == ui::Slider::EventType::ON_PERCENTAGE_CHANGED )
	{
		ui::Slider* pcSlider = static_cast<ui::Slider*>( pSender );
		float fNormalisedVolume = ( ( float ) pcSlider->getPercent() ) / 100.0f;
		CGameManager::GetInstance().SetSfxVolume( fNormalisedVolume );
	}
}