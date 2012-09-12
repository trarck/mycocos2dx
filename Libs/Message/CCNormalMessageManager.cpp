//
//  MessageManager.m
//  Message
//
//  Created by trarck on 11-11-27.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#include "CCNormalMessageManager.h"

NS_CC_BEGIN

CCNormalMessageManager* CCNormalMessageManager::s_sharedNormalMessageManagerInstance=NULL;

CCNormalMessageManager* CCNormalMessageManager::sharedNormalMessageManager(void)
{
	if (!s_sharedNormalMessageManagerInstance) {
		s_sharedNormalMessageManagerInstance=new CCNormalMessageManager();
		s_sharedNormalMessageManagerInstance->init();
	}
	return s_sharedNormalMessageManagerInstance;
}

void CCNormalMessageManager::dispatchMessage(Message* message)
{
	//NSAssert(message.type!=0,)
	//message for type
	CCDictionary* msgMap=(CCDictionary*)m_messages->objectForKey(message->getType());
	if (msgMap) {
		//parse for sender
		//如果sender不为空，则不要触发一次全局消息，只触发sender的消息。
		CCObject* sender=message->getSender();
		if (sender) {
			//执行注册到sender的消息的处理方法
			CCDictionary* senderMap=(CCDictionary*)msgMap->objectForKey(sender->m_uID);
			//如果注册则执行
			if (senderMap)   execRegisterWithSenderMap(senderMap ,message);
		}else {
			//执行注册到global的消息的处理方法
			CCDictionary* globalMap=(CCDictionary*) msgMap->objectForKey(m_globalObject->m_uID);
			//如果注册则执行
			if (globalMap)   execRegisterWithSenderMap(globalMap ,message);
		}
	}
}

NS_CC_END
