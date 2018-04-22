#ifndef _PAUSESCREENLAYER_H_
#define _PAUSESCREENLAYER_H_

#ifndef __COCOS2D_H__
	#include "cocos2d.h"
#endif

#include "ui/UISlider.h"

//fwd decl
class CGameLayerEmpty;

class CPauseScreenLayer
	: public cocos2d::LayerColor
{
public:
	CREATE_FUNC( CPauseScreenLayer );

	~CPauseScreenLayer();

	/// Initializes buttons with touch events
	virtual void Init();

	/// Apply all VFX / Actions and set visible
	/// Called indirectly upon pressing the pause button.
	void Activate();

	void Reset();

	void Toggle( bool bPauseStatus );

	void SetLevelInstance(CGameLayerEmpty& cLevelInstance);

private:
	const char* km_pszPauseIconIdle		= "PauseIdle.png";
	const char* km_pszPauseIconPressed	= "PausePressed.png";
	const char* km_pszPlayIconIdle		= "PauseButton.png";
	const char* km_pszPlayIconPressed	= "PauseButtonPressed.png";

	bool bIsPaused;
	CGameLayerEmpty* m_pcLevelInstance;

	//Slider callbacks.
	void CB_MusicSliderEvent( Ref* pSender, cocos2d::ui::Slider::EventType type );
	void CB_EffectSliderEvent( Ref* pSender, cocos2d::ui::Slider::EventType type );
};




#endif //_PAUSESCREENLAYER_H_
