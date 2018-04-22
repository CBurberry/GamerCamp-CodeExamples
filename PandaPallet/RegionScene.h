//Psuedocode
//Reuses code from original MenuScene.h

//Pass region variable so level selection data can be loaded
//Images/Progression/Filenames

//Passing of current region can be done through GameManager
// - Consider the ways the player can reach the LevelSelectionScene
//		Is the region always valid or set to valid before loading?

#ifndef _REGIONSCENE_H_
#define _REGIONSCENE_H_

#ifndef __COCOS2D_H__
	#include "cocos2d.h"
#endif

#include "GamerCamp/GameSpecific/GameManager.h"

class CRegionScene
	: public cocos2d::CCLayer
{
private:
	int m_iSelection;

public:

	// cocos2d-x classes all have a static create() function
	// This macro implements the default behaviour of create()
	// i.e. news an instance, calls init, and calls autorelease 
	CREATE_FUNC( CRegionScene );

	~CRegionScene();

	// Explicit constructor - called from the CREATE_FUNC() macro
	// n.b. this must back-chain calls to the base class versions of init()
	// or the behaviour of your code will be 'unexpected'
	virtual bool init();

	// The cocos2d convention is to have the root layer of a scene
	// be able to create and return a CCScene* (set to autorelease)
	// which has an instance of itself as a child
	static cocos2d::Scene* scene();

	//Callback Function to load level selection screen
	void CB_OpenLevelSelection( Ref* pSender, CLevelData::ERegion iSelection );

	//Callback Function to return to main menu screen
	void CB_ReturnToMenu( Ref* pSender );
};




#endif //_REGIONSCENE_H_