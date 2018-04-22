#ifndef _INGREDIENTSBAR_H_
#define _INGREDIENTSBAR_H_

#include "GamerCamp/GameSpecific/EventSystem/IEventListener.h"
#include "2d/CCNode.h"
#include "2d/CCSprite.h"

class CIngredientsBar
	: public cocos2d::Node
	, public IEventListener
{
public:
	CREATE_FUNC( CIngredientsBar );

	CIngredientsBar();
	~CIngredientsBar();

	virtual bool init() override;

	//Reset all attributes of the Ingredients bar.
	void Reset();

	//Create the child sprites for the bar
	void InitialiseSprites();

	//Replace the child sprite images
	void ReplaceSprites();

	//Play collected item action sequence
	//N.B. this is a reimplementation of the old MarkIngredientsAsCollectedUI method.
	void IngredientCollected();

	//On Event override - virtual
	//This event expects a data of type CIngredient*
	virtual void VOnEvent( void* pData ) override;

protected:
	//Unparent each sprite element in the array and set to null.
	void ResetAttachedSpritesArray();

private:
	enum { EMaxUIElements = 6 };

	//Const members
	const int km_iMaxIngredients			= 3;
	const int km_iBlurOpacity				= 150;
	const char* km_pszIngredientsBar		= "IngredientsBar.png";
	const char* km_pszEmptyDish				= "EmptyDish.png";
	const char* km_pszEmptyIngredientName	= "_Unknown.png";

	//Non-const members
	bool m_bIsInitialised;
	int m_iCurrentFoundIngredients;
	cocos2d::Sprite* m_pcIngredientsBar;
	cocos2d::Sprite* m_acAttachedSprites[ EMaxUIElements ];
};

#endif //_INGREDIENTSBAR_H_
