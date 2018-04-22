//This class is mostly a copy of the implementation of PauseScreenLayer with image changes
//TODO - reimplent the puasescreen as just pause button and add the options layer as a child to it to save on code bloating.
//Options screen doesn't have some additional 

#ifndef _OPTIONSSCREENLAYER_H_
#define _OPTIONSSCREENLAYER_H_

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#include "ui/UISlider.h"

//fwd decl
class CGameLayerEmpty;

class COptionsScreenLayer
	: public cocos2d::LayerColor
{
public:
	CREATE_FUNC( COptionsScreenLayer );

	~COptionsScreenLayer();

	/// Initializes buttons with touch events
	virtual void Init();

	/// Apply all VFX / Actions and set visible
	/// Called indirectly upon pressing the pause button.
	void Activate();

	void Reset();

	void Toggle( bool bPauseStatus );

private:
	const char* km_pszSettingsIconIdle = "SettingsIdle.png";
	const char* km_pszSettingsIconPressed = "SettingsPressed.png";

	bool m_bIsOptionsVisible;

	//Slider callbacks.
	void CB_MusicSliderEvent( Ref* pSender, cocos2d::ui::Slider::EventType type );
	void CB_EffectSliderEvent( Ref* pSender, cocos2d::ui::Slider::EventType type );
};




#endif //_OPTIONSSCREENLAYER_H_

