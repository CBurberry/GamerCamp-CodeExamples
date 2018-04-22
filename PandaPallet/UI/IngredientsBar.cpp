#include "IngredientsBar.h"
#include "GamerCamp/GCCocosInterface/IGCGameLayer.h"
#include "GamerCamp/Core/GCTypes.h"
#include "GamerCamp/GameSpecific/Items/Ingredient.h"
#include "GamerCamp/GCCocosInterface/GCCocosHelpers.h"
#include "GamerCamp/GameSpecific/ResourceManager.h"

USING_NS_CC;

CIngredientsBar::CIngredientsBar()
	: m_iCurrentFoundIngredients( 0 )
	, m_acAttachedSprites()
	, m_bIsInitialised( false )
{
}


CIngredientsBar::~CIngredientsBar()
{
	this->stopAllActions();
	if ( m_pcIngredientsBar != nullptr )
	{
		m_pcIngredientsBar->removeFromParent();
	}
	ResetAttachedSpritesArray();
	this->removeFromParent();
}

bool CIngredientsBar::init()
{
	//Register the Ingredient collection event with the event registry.
	std::string strEventName = "IngredientTouched";
	IGCGameLayer::ActiveInstance()->GetEventRegistry()->RegisterListener( &strEventName, this, GetGCTypeIDOf( CIngredient* ) );

	//Initialise
	Reset();

	return Node::init();
}

void CIngredientsBar::Reset()
{
	m_iCurrentFoundIngredients = 0;
	Vec2 v2CurrentPosition = getPosition();

	//Setup the ingredients bar
	m_pcIngredientsBar = Sprite::createWithSpriteFrameName( km_pszIngredientsBar );
	m_pcIngredientsBar->setScale( Director::getInstance()->getContentScaleFactor() );
	//Set ingredients bar position to this (Node) position + half dimensions of content
	m_pcIngredientsBar->setPosition( v2CurrentPosition );
	this->addChild( m_pcIngredientsBar, 0 );

	//Either reset or initialise
	if ( m_bIsInitialised )
	{
		ReplaceSprites();
	}
	else 
	{
		InitialiseSprites();
	}
}

void CIngredientsBar::InitialiseSprites()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 v2CurrentPosition = getPosition();

	//const value definitions.
	const float k_fBarElementSpacing_X = 0.06f * visibleSize.width;
	const float k_fContentWidth = m_pcIngredientsBar->getContentSize().width * m_pcIngredientsBar->getScaleX();
	const Vec2 k_fIngredientBasePosition = 
		Vec2( v2CurrentPosition.x - ( k_fContentWidth * 0.125f ), v2CurrentPosition.y );
	const float k_fSpriteScale = Director::getInstance()->getContentScaleFactor() * 0.85f;

	//Set the empty plates and unknown ingredients sprites.
	Sprite* pcEmptyPlate = nullptr;
	Sprite* pcIngredient = nullptr;
	int iLoopInterations = EMaxUIElements / 2;
	ResetAttachedSpritesArray();
	for ( int iLoop = 0; iLoop < iLoopInterations; ++iLoop )
	{
		pcEmptyPlate = Sprite::createWithSpriteFrameName( km_pszEmptyDish );
		pcEmptyPlate->setScale( k_fSpriteScale );
		Vec2 v2IngredientPos = Vec2( k_fIngredientBasePosition.x + k_fBarElementSpacing_X * iLoop, k_fIngredientBasePosition.y );
		pcEmptyPlate->setPosition( v2IngredientPos );
		this->addChild( pcEmptyPlate, 1 );

		pcIngredient = Sprite::createWithSpriteFrameName( km_pszEmptyIngredientName );
		pcIngredient->setPosition( v2IngredientPos );
		pcIngredient->setScale( k_fSpriteScale );
		this->addChild( pcIngredient, 2 );

		//0-2 is the plates, 3-5 is the ingredients
		m_acAttachedSprites[iLoop] = pcEmptyPlate;
		m_acAttachedSprites[iLoop + iLoopInterations] = pcIngredient;
	}
	m_bIsInitialised = true;
}

void CIngredientsBar::ReplaceSprites()
{
	const float k_fSpriteScale = Director::getInstance()->getContentScaleFactor() * 0.85f;

	//Iterate through the attached ingredient elements and reset them to the unknown sprite.
	int iLoopInterationBase = EMaxUIElements / 2;
	for ( int iLoop = iLoopInterationBase; iLoop < EMaxUIElements; ++iLoop )
	{
		m_acAttachedSprites[iLoop]->stopAllActions();
		m_acAttachedSprites[iLoop]->setOpacity( 255 );
		m_acAttachedSprites[iLoop]->setScale( k_fSpriteScale );
		m_acAttachedSprites[iLoop]->setSpriteFrame( km_pszEmptyIngredientName );
	}
}

void CIngredientsBar::IngredientCollected()
{
	int iIngredientElementBase = EMaxUIElements / 2;

	// set an action that maxes out opacity
	// and and plays pop up animation
	m_acAttachedSprites[ iIngredientElementBase + m_iCurrentFoundIngredients ]->runAction(
		CCSpawn::create(
			FadeTo::create( 0.2f, 255 ),
			Sequence::create(
				ScaleBy::create( 0.2f, 1.5f ),
				ScaleBy::create( 0.2f, 1.5f )->reverse(),
				nullptr
			),
			nullptr
		)
	);

	++m_iCurrentFoundIngredients;
}

void CIngredientsBar::VOnEvent( void* pData )
{
	//This event expects a data of type CIngredient*
	//Get the sprite name / or data from the ingredient
	CIngredient* pcIngredientData = static_cast<CIngredient*>( pData );
	Sprite* pcSpriteData = nullptr;
	if ( pcIngredientData != nullptr )
	{
		pcSpriteData = pcIngredientData->GetSprite();
		int iIngredientElementBase = EMaxUIElements / 2;
		m_acAttachedSprites[iIngredientElementBase + m_iCurrentFoundIngredients]->setOpacity( km_iBlurOpacity );
		m_acAttachedSprites[iIngredientElementBase + m_iCurrentFoundIngredients]->setSpriteFrame( pcSpriteData->getSpriteFrame() );
		IngredientCollected();
	}
}

void CIngredientsBar::ResetAttachedSpritesArray()
{
	//Reset the attachedSprites array
	for ( int iLoop = 0; iLoop < EMaxUIElements; ++iLoop )
	{
		if ( m_acAttachedSprites[iLoop] != nullptr )
		{
			m_acAttachedSprites[iLoop]->stopAllActions();
			m_acAttachedSprites[iLoop]->removeFromParent();
			m_acAttachedSprites[iLoop] = nullptr;
		}
	}
}
