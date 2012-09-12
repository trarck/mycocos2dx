//
//  MessageManager.h
//
//	每个对象只有一个处理消息的函数，和window的消息类似
//	hash(接收者)→hash(发送者)→handler
//  Created by trarck on 11-11-27.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#ifndef __CCSimple_Message_Manager_H__
#define __CCSimple_Message_Manager_H__

#include "cocos2d.h"
#include "CCMessage.h"
#include "CCMessageHandler.h"

#ifndef GlobalMessageType
#define GlobalMessageType 0
#endif

NS_CC_BEGIN

class CCSimpleMessageManager : public CCObject {
public:
	CCSimpleMessageManager();
    ~CCSimpleMessageManager();

    static CCSimpleMessageManager* sharedSimpleMessageManager();

//	static CCIMessageManager* sharedMessageManager();

    void init();

    /**
     * observer 消息接收者
     * sender   消息发送者
     */
    //receiver对sender发来的type消息可以有多个响应方法，实际中情况会很少
      
    void registerReceiver(CCObject* receiver,SEL_MessageHandler handle ,MessageType type ,CCObject* sender ,CCObject*  handleObject);

	void registerReceiver(CCObject* receiver,SEL_MessageHandler handle,MessageType type ,CCObject* sender);
                                                                                                                              
    void removeReceiver(CCObject* receiver ,SEL_MessageHandler handle ,MessageType type ,CCObject* sender);
                                                                                                                                                                      
	void execAllRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message);

    void execRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message);

	void execRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message,CCObject*  receiver);

	void dispatchMessage(CCMessage* message);
                                                                                             
    void dispatchMessageWithType(MessageType type ,CCObject* sender ,CCObject* receiver,CCDictionary* data);
                                                                                     
    void dispatchMessage(CCMessage* message ,CCObject*  receiver);

private:
    CCDictionary *m_messages;
	CCObject *m_globalObject;
	static CCSimpleMessageManager* s_sharedSimpleMessageManagerInstance;
};

NS_CC_END

#endif //__CCSimple_Message_Manager_H__
