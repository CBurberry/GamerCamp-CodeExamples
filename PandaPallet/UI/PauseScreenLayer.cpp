#include "PauseScreenLayer.h"
#include "ui/UIButton.h"
#include "GamerCamp/GameSpecific/GameManager.h"
#include "GamerCamp/GameSpecific/GameLayerEmpty.h"

CPauseScreenLayer::~CPauseScreenLayer()
{
	stopAllActions();
	removeAllChildren();
	removeFromParent();
}

void CPauseScreenLayer::Init()
{
	//Init superclass with a color (clear & transparent)
	if ( !LayerColor::initWithColor( Color4B( 0, 0, 0, 0 ) ) )
	{
		return;
	}

	GCASSERT( m_pcLevelInstance, "Level instance should be set for pause before calling Init()!" );

	bIsPaused = m_pcLevelInstance->bIsGamePaused();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//Add the pause button - visible during play
	ui::Button* pcPauseButton = ui::Button::create(
				km_pszPauseIconIdle,
				km_pszPauseIconPressed, 
				"",
				ui::Widget::TextureResType::PLIST );
	pcPauseButton->addTouchEventListener( [&] ( Ref* sender, ui::Widget::TouchEventType type )
	{
		switch ( type )
		{
		case ui::Widget::TouchEventType::BEGAN:
			Toggle( m_pcLevelInstance->bIsGamePaused() );
			break;
		default:
			//Do nothing
			break;
		}
	} );

	pcPauseButton->setPosition( Vec2 (	origin.x + ( visibleSize.width * 0.05f ), 
										origin.y + ( visibleSize.height * 0.93f ) ) );
	pcPauseButton->setCascadeOpacityEnabled( true );
	pcPauseButton->ignoreContentAdaptWithSize( false );
	pcPauseButton->setScale( Director::getInstance()->getContentScaleFactor() );
	pcPauseButton->setTouchEnabled( true );
	pcPauseButton->setVisible( true );
	pcPauseButton->setName("PauseButton");

	//Add pause screen visual elements
	Sprite* pcSprite = Sprite::create( "Loose/PauseScreenCode.png" );
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
	pcMusicSlider->addEventListener( CC_CALLBACK_2( CPauseScreenLayer::CB_MusicSliderEvent, this ) );

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
	pcEffectSlider->addEventListener( CC_CALLBACK_2( CPauseScreenLayer::CB_EffectSliderEvent, this ) );

	// Add all buttons
	ui::Button* pcRestartLevelButton = ui::Button::create(
		"RestartButton.png",
		"RestartButtonPressed.png",
		"",
		ui::Widget::TextureResType::PLIST );
	ui::Button* pcLevelSelectButton = ui::Button::create(
		"BackButton.png",
		"BackButtonPressed.png",
		"",
		ui::Widget::TextureResType::PLIST );

	// Position buttons
	pcRestartLevelButton->setPosition( Vec2( origin.x + ( visibleSize.width * 0.4f ),
		origin.y + ( visibleSize.height * 0.3f ) ) );
	pcLevelSelectButton->setPosition( Vec2( origin.x + ( visibleSize.width * 0.6f ),
		origin.y + ( visibleSize.height * 0.3f ) ) );

	// Resizing content
	pcRestartLevelButton->ignoreContentAdaptWithSize( false );
	pcLevelSelectButton->ignoreContentAdaptWithSize( false );

	//Apply new scale to button content 
	pcRestartLevelButton->setScale( Director::getInstance()->getContentScaleFactor() );
	pcLevelSelectButton->setScale( Director::getInstance()->getContentScaleFactor() * 1.34f );

	//Enable touch events
	pcRestartLevelButton->setTouchEnabled( true );
	pcLevelSelectButton->setTouchEnabled( true );

	//Add callbacks
	pcRestartLevelButton->addTouchEventListener( [&] ( Ref* sender, ui::Widget::TouchEventType type )
	{
		switch ( type )
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			Scene* pScene = CGameManager::GetInstance().GetActiveScene();
			CGameLayerEmpty* pGameLayer = static_cast<CGameLayerEmpty*>( pScene->getChildren().at( 0 ) );
			Director::getInstance()->resume();
			pGameLayer->Callback_OnResetButton( sender );
			break;
		}
		default:
			//Do nothing.
			break;
		}
	} );

	pcLevelSelectButton->addTouchEventListener( [&] ( Ref* sender, ui::Widget::TouchEventType type )
	{

		switch ( type )
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			Scene* pScene = CGameManager::GetInstance().GetActiveScene();
			CGameLayerEmpty* pGameLayer = static_cast<CGameLayerEmpty*>( pScene->getChildren().at( 0 ) );
			Director::getInstance()->resume();
			pGameLayer->Callback_OnQuitButton( sender );
			break;
		}
		default:
			//Do nothing.
			break;
		}
	} );

	//Set visibilities
	pcRestartLevelButton->setVisible( false );
	pcLevelSelectButton->setVisible( false );

	// Set names
	pcRestartLevelButton->setName( "RestartButton" );
	pcLevelSelectButton->setName( "LevelSelectButton" );

	//Add all visual elements as children to the layer
	this->addChild( pcSprite, 1 );
	this->addChild( pcPauseButton, 1 );
	this->addChild( pcMusicSlider, 2 );
	this->addChild( pcEffectSlider, 2 );
	this->addChild( pcRestartLevelButton, 2 );
	this->addChild( pcLevelSelectButton, 2 );
}

void CPauseScreenLayer::Activate()
{
	//Set game properties.
	m_pcLevelInstance->SetGamePaused( bIsPaused );
	m_pcLevelInstance->SetControlLayerEnabled( !bIsPaused );

	//Get UI elements
	ui::Button* pcPauseButton	= static_cast< ui::Button* >( getChildByName( "PauseButton" ) );
	ui::Button* pcRestartButton = static_cast< ui::Button* >( getChildByName( "RestartButton" ) );
	ui::Button* pcSelectButton	= static_cast< ui::Button* >( getChildByName( "LevelSelectButton" ) );
	ui::Slider* pcMusicSlider	= static_cast< ui::Slider* >( getChildByName( "MusicSlider" ) );
	ui::Slider* pcEffectSlider	= static_cast< ui::Slider* >( getChildByName( "EffectSlider" ) );
	Sprite* pcBackground		= static_cast< Sprite* >( getChildByName( "Background" ) );

	//Set paused elements visible/invisible
	pcMusicSlider->setVisible( bIsPaused );
	pcEffectSlider->setVisible( bIsPaused );
	pcBackground->setVisible( bIsPaused );
	pcRestartButton->setVisible( bIsPaused );
	pcSelectButton->setVisible( bIsPaused );

	//Pause specific behaviour
	if ( bIsPaused )
	{
		pcPauseButton->loadTextures( km_pszPlayIconIdle, km_pszPlayIconPressed, "", ui::Widget::TextureResType::PLIST );
		setOpacity( 150 );
		Director::getInstance()->pause();

		//Might have to try working around with the 4FPS pause limit if UI unresponsive
		//Director::getInstance()->setAnimationInterval(1.0f / 10.0f, SetIntervalReason::BY_DIRECTOR_PAUSE );
	}
	else 
	{
		pcPauseButton->loadTextures( km_pszPauseIconIdle, km_pszPauseIconPressed, "", ui::Widget::TextureResType::PLIST );
		setOpacity( 0 );
		Director::getInstance()->resume();
	}
}

void CPauseScreenLayer::Reset()
{
	Sprite* pcBackground = static_cast< Sprite* >( getChildByName( "Background" ) );
	ui::Slider* pcMusicSlider = static_cast< ui::Slider* >( getChildByName( "MusicSlider" ) );
	ui::Slider* pcEffectSlider = static_cast< ui::Slider* >( getChildByName( "EffectSlider" ) );
	ui::Button* pcRestartButton = static_cast< ui::Button* >( getChildByName( "RestartButton" ) );
	ui::Button* pcSelectButton = static_cast< ui::Button* >( getChildByName( "LevelSelectButton" ) );
	ui::Button* pcPauseButton = static_cast< ui::Button* >( getChildByName( "PauseButton" ) );
	pcPauseButton->loadTextures( km_pszPauseIconIdle, km_pszPauseIconPressed, "", ui::Widget::TextureResType::PLIST );
	pcMusicSlider->setVisible( false );
	pcEffectSlider->setVisible( false );
	pcBackground->setVisible( false );
	pcRestartButton->setVisible( false );
	pcSelectButton->setVisible( false );
	bIsPaused = false;
	setOpacity( 0 );
	Director::getInstance()->resume();
}

void CPauseScreenLayer::Toggle( bool bPauseStatus )
{
	//Invert the pause status
	bIsPaused = !bPauseStatus;
	Activate();
}

void CPauseScreenLayer::SetLevelInstance( CGameLayerEmpty & cLevelInstance )
{
	this->m_pcLevelInstance = &( cLevelInstance );
}

void CPauseScreenLayer::CB_MusicSliderEvent( Ref * pSender, ui::Slider::EventType type )
{
	if ( type == ui::Slider::EventType::ON_PERCENTAGE_CHANGED ) 
	{
		ui::Slider* pcSlider = static_cast<ui::Slider*>( pSender );
		float fNormalisedVolume = ( ( float ) pcSlider->getPercent() ) / 100.0f;
		CGameManager::GetInstance().SetBackgroundVolume( fNormalisedVolume );
	}
}

void CPauseScreenLayer::CB_EffectSliderEvent( Ref * pSender, ui::Slider::EventType type )
{
	if ( type == ui::Slider::EventType::ON_PERCENTAGE_CHANGED )
	{
		ui::Slider* pcSlider = static_cast<ui::Slider*>( pSender );
		float fNormalisedVolume = ( ( float ) pcSlider->getPercent() ) / 100.0f;
		CGameManager::GetInstance().SetSfxVolume( fNormalisedVolume );
	}
}
