//
//  MessageManager.m
//  Message
//
//  Created by trarck on 11-11-27.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#include "CCSimpleMessageManager.h"

NS_CC_BEGIN

CCSimpleMessageManager* CCSimpleMessageManager::s_sharedSimpleMessageManagerInstance=NULL;

CCSimpleMessageManager* CCSimpleMessageManager::sharedSimpleMessageManager(void)
{
	if (!s_sharedSimpleMessageManagerInstance) {
		s_sharedSimpleMessageManagerInstance=new CCSimpleMessageManager();
		s_sharedSimpleMessageManagerInstance->init();
	}
	return s_sharedSimpleMessageManagerInstance;
}


CCSimpleMessageManager::CCSimpleMessageManager():m_messages(NULL),m_globalObject(NULL)
{

}

CCSimpleMessageManager::~CCSimpleMessageManager()
{
	CC_SAFE_RELEASE(m_messages);
	CC_SAFE_RELEASE(m_globalObject);
}



void CCSimpleMessageManager::init()
{
	m_messages=new CCDictionary();
	m_globalObject=new CCObject();
}


//使receiver可以接收sender发送过来的叫type的消息，并用handle来处理
//关注的对象是receiver
//type,sender,receiver,handle唯一，同一接收者的同一个消息只有一个处理函数
void CCSimpleMessageManager::registerReceiver(CCObject* receiver,SEL_MessageHandler handle ,MessageType type ,CCObject* sender ,CCObject*  handleObject)
{
	CCAssert(receiver!=NULL,"SimpleMessageManager:registerReceiver:receiver can't be null");
	CCAssert(handle!=NULL,"SimpleMessageManager:registerReceiver:handle");
	CCAssert(handleObject!=NULL,"SimpleMessageManager:registerReceiver:handleObject");
	
	//type等于0，则所有消息都会发送
	//register for type
	CCDictionary *msgMap=(CCDictionary*) m_messages->objectForKey(type);
	if (!msgMap) {
		msgMap=new CCDictionary();
		m_messages->setObject(msgMap,type);
		msgMap->release();
	}
	//register for sender
	//sender 为空，则注册到全局对象上
	sender=sender==NULL?m_globalObject:sender;
	unsigned int senderKey=sender->m_uID;
	CCDictionary *senderMap=(CCDictionary*)msgMap->objectForKey(senderKey);
	if (!senderMap) {
		senderMap=new CCDictionary();
	    msgMap->setObject(senderMap ,senderKey);
		senderMap->release();
	}
	//register for receiver
	//检查是否已经注册过
	CCMessageHandler* handler=(CCMessageHandler*)senderMap->objectForKey(receiver->m_uID);
	CCAssert(!handler,"SimpleMessageManager:registerReceiver:Handle has register");
	if(!handler){
		//register for handler
		handler=new CCMessageHandler();
		handler->initWithTarget(handleObject,handle);
		senderMap->setObject(handler ,receiver->m_uID);
		handler->release();
	}
}

void CCSimpleMessageManager::registerReceiver(CCObject* receiver,SEL_MessageHandler handle,MessageType type ,CCObject* sender)
{
	registerReceiver(receiver ,handle,type ,sender ,receiver);
}

void CCSimpleMessageManager::removeReceiver(CCObject* receiver ,SEL_MessageHandler handle ,MessageType type ,CCObject* sender)
{
	CCAssert(receiver!=NULL,"SimpleMessageManager:removeReceiver:receiver can't be null!");
	CCAssert(handle!=NULL,"SimpleMessageManager:registerReceiver:handle");
	
	//message for type
	CCDictionary* msgMap=(CCDictionary*)m_messages->objectForKey(type);
	if (msgMap) {
		//message for sender
		sender=sender==NULL?m_globalObject:sender;
		CCDictionary *senderMap=(CCDictionary*)msgMap->objectForKey(sender->m_uID);
		if (senderMap) {
			//message for receiver
			senderMap->removeObjectForKey(receiver->m_uID);
		}
	}
}

void CCSimpleMessageManager::execAllRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message)
{
	CCAssert(senderMap!=NULL,"SimpleMessageManager:execAllRegisterWithSenderMap:senderMap can't be null!");
	//send to all
	CCDictElement* pElement = NULL;
	CCDICT_FOREACH(senderMap,pElement){
		CCMessageHandler* handler=(CCMessageHandler*) pElement->getObject();
		handler->execute(message);
	}
}

void CCSimpleMessageManager::execRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message)
{
	CCAssert(senderMap!=NULL,"SimpleMessageManager:execRegisterWithSenderMap:senderMap can't be null!");
	CCObject* receiver=message->getReceiver();
	if (receiver) {
		//message for receiver
		CCMessageHandler* handler=(CCMessageHandler*)senderMap->objectForKey(receiver->m_uID);
		if(handler) handler->execute(message);
	}else {
		//send to all receiver
		execAllRegisterWithSenderMap(senderMap ,message);
	}
}

void CCSimpleMessageManager::execRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message,CCObject*  receiver)
{
	CCAssert(senderMap!=NULL,"SimpleMessageManager:execRegisterWithSenderMap:senderMap can't be null!");
	if (receiver) {
		//message for receiver
		CCMessageHandler* handler=(CCMessageHandler*)senderMap->objectForKey(receiver->m_uID);
		if(handler) handler->execute(message);
	}else {
		//send to all receiver
		execAllRegisterWithSenderMap(senderMap ,message);
	}
}

//
void CCSimpleMessageManager::dispatchMessage(CCMessage* message)
{
	CCObject* sender;
	//CCAssert(message.type!=0,)
	//如果message的type不为0，则需要执行一个type为global的所有消息
	if (message->getType()!=GlobalMessageType) {
		//message for global
		CCDictionary* msgMap=(CCDictionary*)m_messages->objectForKey(GlobalMessageType);
		if (msgMap) {
			//parse for sender
			//如果sender不为空，则还要触发一次全局消息。
			sender=message->getSender();
			if (sender) {
				//执行注册到sender的消息的处理方法
				CCDictionary* senderMap=(CCDictionary *)msgMap->objectForKey(sender->m_uID);
				//如果注册则执行
				if (senderMap)  execRegisterWithSenderMap(senderMap ,message);
				//执行注册到global的消息的处理方法
				CCDictionary* globalMap=(CCDictionary*)msgMap->objectForKey(m_globalObject->m_uID);
				//如果注册则执行
				if (globalMap)  execRegisterWithSenderMap(globalMap,message);
			}else {
				//执行注册到global的消息的处理方法
				CCDictionary* globalMap=(CCDictionary*)msgMap->objectForKey(m_globalObject->m_uID);
				//如果注册则执行
				if (globalMap)  execRegisterWithSenderMap(globalMap ,message);
			}
		}
	}
	//message for type
	CCDictionary* msgMap= (CCDictionary*)m_messages->objectForKey(message->getType());
	if (msgMap) {
		//parse for sender
		//如果sender不为空，则还要触发一次全局消息。
		sender=message->getSender();
		if (sender) {
			//执行注册到sender的消息的处理方法
			CCDictionary* senderMap=(CCDictionary *)msgMap->objectForKey(sender->m_uID);
			//如果注册则执行
			if (senderMap)  execRegisterWithSenderMap(senderMap,message);
			//执行注册到global的消息的处理方法
			CCDictionary* globalMap=(CCDictionary*)msgMap->objectForKey(m_globalObject->m_uID);
			//如果注册则执行
			if (globalMap)  execRegisterWithSenderMap(globalMap,message);
		}else {
			//执行注册到global的消息的处理方法
			CCDictionary* globalMap=(CCDictionary*)msgMap->objectForKey(m_globalObject->m_uID);
			//如果注册则执行
			if (globalMap)  execRegisterWithSenderMap(globalMap,message);
		}
	}
}

void CCSimpleMessageManager::dispatchMessageWithType(MessageType type ,CCObject* sender ,CCObject* receiver,CCDictionary* data)
{
	CCMessage* message=new CCMessage();
	message->initWithType(type ,sender ,receiver ,data);
	dispatchMessage(message);
	message->release();
}

//适应message中没有receiver的情况
void CCSimpleMessageManager::dispatchMessage(CCMessage* message ,CCObject*  receiver)
{
	//CCAssert(message.type!=0,)
	//如果message的type不为0，则需要执行一个type为global的所有消息
	if (message->getType()!=GlobalMessageType) {
		//message for global
		CCDictionary* msgMap=(CCDictionary*)m_messages->objectForKey(m_globalObject->m_uID);
		if (msgMap) {
			//parse for sender
			//如果sender不为空，则还要触发一次全局消息。
			CCObject* sender=message->getSender();
			if (sender) {
				//执行注册到sender的消息的处理方法
				CCDictionary* senderMap=(CCDictionary*)msgMap->objectForKey(sender->m_uID);
				//如果注册则执行
				if (senderMap)  execRegisterWithSenderMap(senderMap,message,receiver);
				//执行注册到global的消息的处理方法
				CCDictionary* globalMap=(CCDictionary*)msgMap->objectForKey(m_globalObject->m_uID);
				//如果注册则执行
				if (globalMap)  execRegisterWithSenderMap(globalMap ,message ,receiver);
			}else {
				//执行注册到global的消息的处理方法
				CCDictionary* globalMap=(CCDictionary*)msgMap->objectForKey(m_globalObject->m_uID);
				//如果注册则执行
				if (globalMap)  execRegisterWithSenderMap(globalMap ,message ,receiver);
			}
		}
	}
	//message for type
	CCDictionary* msgMap=(CCDictionary*)m_messages->objectForKey(message->getType());
	if (msgMap) {
		//parse for sender
		//如果sender不为空，则还要触发一次全局消息。
		CCObject* sender=message->getSender();
		if (sender) {
			//执行注册到sender的消息的处理方法
			CCDictionary* senderMap=(CCDictionary*)msgMap->objectForKey(sender->m_uID);
			//如果注册则执行
			if (senderMap)  execRegisterWithSenderMap(senderMap ,message ,receiver);
			//执行注册到global的消息的处理方法
			CCDictionary* globalMap=(CCDictionary*)msgMap->objectForKey(m_globalObject->m_uID);
			//如果注册则执行
			if (globalMap)  execRegisterWithSenderMap(globalMap ,message ,receiver);
		}else {
			//执行注册到global的消息的处理方法
			CCDictionary* globalMap=(CCDictionary*)msgMap->objectForKey(m_globalObject->m_uID);
			//如果注册则执行
			if (globalMap)  execRegisterWithSenderMap(globalMap ,message ,receiver);
		}
	}
}

NS_CC_END
