#include "MyDraw.h"
USING_NS_CC;

void MyDraw::draw(void)
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	ccDrawColor4F(1.0f, 0.0f, 0.0f, 1.0f);

	ccDrawLine(ccp(0,0),ccp(size.width,size.height));
}
