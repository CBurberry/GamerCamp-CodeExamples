#ifndef __VIRTUALCONTROLLAYER_H__
#define __VIRTUALCONTROLLAYER_H__

#include "cocos2d.h"
#include "ui/UIButton.h"

//forward declaring
class CPlayerCustom;

//////////////////////////////////////////////////////////////////////////
/// \class CVirtualControlLayer 
/// \brief
/// Control layer that is responsible for player input on the screen.
/// Controllable by mouse or touch
//////////////////////////////////////////////////////////////////////////
class CVirtualControlLayer
	: public cocos2d::CCLayer
{
public:
	CREATE_FUNC(CVirtualControlLayer);

	~CVirtualControlLayer();

	/// Initializes buttons with touch events
	virtual bool init();

	/// Enable or disable all the buttons on the control layer
	void SetButtonsActive( bool bParam );

	CPlayerCustom* m_pPlayerPtr = nullptr; ///< Pointer to current player
private:
	cocos2d::Rect m_sScreenRect;
	cocos2d::Rect m_sLeftScreenRect;
	cocos2d::Rect m_sRightScreenRect;

	virtual bool onTouchBegan( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent ) override;
	virtual void onTouchMoved( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent ) override;
	virtual void onTouchEnded( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent ) override;
	virtual void onTouchCancelled( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent ) override;
};

#endif	//__VIRTUALCONTROLLAYER_H__
