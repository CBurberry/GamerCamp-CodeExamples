#ifndef _LOADINGSCREENSCENE_H_
#define _LOADINGSCREENSCENE_H_

#include "2d/CCScene.h"
#include "2d/CCSprite.h"

//////////////////////////////////////////////////////////////////////////
/// \class CLoadingSceneScreen 
/// \brief
/// Scene that is used to notify a player that his current level 
/// is loading. Loading image is set to show it.
//////////////////////////////////////////////////////////////////////////
class CLoadingSceneScreen
: public cocos2d::Scene
{
private:
	const float mk_fLoadDuration = 2.0f;		///< Loading screen fixed duration
	const float mk_fFadeDuration = 0.5f;		///< Fade out effect duration
	cocos2d::Sprite* m_pcLoadingScreenImage;	///< Sprite shown as loading screen image

public:
	CREATE_FUNC( CLoadingSceneScreen );

	/// Default constructor
	CLoadingSceneScreen();

	~CLoadingSceneScreen();

	virtual void onEnter() override;

	/// Setup the level load layer child and start load process
	/// Note: Should be called immediately before or after replacing scene.
	void Init();

	/// Called by the load layer upon transition completion
	void LoadScreenEnded();
};

#endif // !_LOADINGSCREENSCENE_H_
