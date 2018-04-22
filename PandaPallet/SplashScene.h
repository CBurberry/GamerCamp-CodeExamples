#ifndef _SPLASHSCENE_H_
#define _SPLASHSCENE_H_

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;


//////////////////////////////////////////////////////////////////////////
/// \class SplashScene
/// \brief
/// This scene handles the sequence when the game is first opened
/// Currently shows two logos with fade transitions
/// Video support is only available on mobile devices.
//////////////////////////////////////////////////////////////////////////
class SplashScene : public cocos2d::CCLayer
{
private:
	Sprite* m_cBackgroundSpriteA;	///< The first loaded logo sprite
	Sprite* m_cBackgroundSpriteB;	///< The second loaded logo sprite

public:
	// implement the "static create()" method manually
	CREATE_FUNC( SplashScene );

	~SplashScene();

	/// Static member function that returns it's own scene pointer with a child layer.
	static Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	/// Initialise the logo sprites and set opacity.
	virtual bool init();

	/// Run the action sequences for both logos, is called by Cocos2D after creation.
	void onEnter();

	//Don't access member pointers to assets, this funciton is loaded asynchronously.
	///Scheduled function for screen end sequence, loads next scene
	void finishSplash( float dt );
	
	///Scheduled function for splash after video.
	void secondSplash( float dt );

	
};


#endif //_SPLASHSCENE_H_
