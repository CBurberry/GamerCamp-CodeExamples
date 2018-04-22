#ifndef _TUTORIALLAYER_H_
#define _TUTORIALLAYER_H_

#include "cocos2d.h"

class CTutorialLayer
	: public cocos2d::LayerColor
{
public:
	CREATE_FUNC( CTutorialLayer );

	~CTutorialLayer();

	//Initialise the tutorial image and touch listener
	void Init();
private:
	//On touch override
	virtual bool onTouchBegan( cocos2d::Touch* pcTouch, cocos2d::Event* pcEvent ) override;
};



#endif //_TUTORIALLAYER_H_
