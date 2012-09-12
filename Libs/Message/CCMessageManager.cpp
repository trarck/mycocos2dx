//
//  MessageManager.m
//  Message
//
//  Created by trarck on 11-11-27.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#include "CCMessageManager.h"

NS_CC_BEGIN


CCMessageManager::CCMessageManager():m_messages(NULL),m_globalObject(NULL)
{

}

CCMessageManager::~CCMessageManager()
{
	CC_SAFE_RELEASE(m_messages);
	CC_SAFE_RELEASE(m_globalObject);
}

void CCMessageManager::init()
{
	m_messages=new CCDictionary();
	m_globalObject=new CCObject();
}

void CCMessageManager::registerReceiver(CCObject* receiver,SEL_MessageHandler handle ,MessageType type ,CCObject* sender ,CCObject*  handleObject)
{
	CCAssert(receiver!=NULL,"MessageManage:registerReceiver:receiver can't be null");
	CCAssert(handle!=NULL,"MessageManage:registerReceiver:handle");
	CCAssert(handleObject!=NULL,"MessageManage:registerReceiver:handleObject");

	//type等于0，则所有消息都会发送
	//register for type
	CCDictionary *msgMap=(CCDictionary*) m_messages->objectForKey(type);
	if (msgMap==NULL) {
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
	unsigned int receiverKey=receiver->m_uID;
	CCArray *receiverList=(CCArray*)senderMap->objectForKey(receiverKey);
	if (!receiverList) {
		receiverList=new CCArray();
		senderMap->setObject(receiverList,receiverKey);
		receiverList->release();
	}
	
	//检查是否已经注册过
    bool isRegisted=false;
    CCObject* pObject = NULL;
    CCARRAY_FOREACH(receiverList,pObject){
        CCMessageHandler* handler=(CCMessageHandler*) pObject;
        if (handler->getHandle()==handle) {
			CCAssert(0,"Handle has register");
            isRegisted=true;
            break;
		}
    }

    if(!isRegisted){
        //注册时不需要时间
        //struct timeval now;
        //	gettimeofday(&now,NULL);
        
        //register for handler
        CCMessageHandler *handler=new CCMessageHandler();
        handler->initWithTarget(handleObject,handle);
        receiverList->addObject(handler);
        handler->release();
    }
}

//使receiver可以接收sender发送过来的叫type的消息，并用handle来处理
//关注的对象是receiver
void CCMessageManager::registerReceiver(CCObject* receiver,SEL_MessageHandler handle,MessageType type ,CCObject* sender)
{
	registerReceiver(receiver ,handle,type ,sender ,receiver);
}

void CCMessageManager::removeReceiver(CCObject* receiver ,SEL_MessageHandler handle ,MessageType type ,CCObject* sender)
{
	CCAssert(receiver!=NULL,"CompleteMessageManage:removeReceiver:receiver can't be null!");
	CCAssert(handle!=NULL,"CompleteMessageManage:registerReceiver:handle");
	
	//message for type
	CCDictionary* msgMap=(CCDictionary*)m_messages->objectForKey(type);
	if (msgMap) {
		//message for sender
		sender=sender?sender:m_globalObject;
		CCDictionary *senderMap=(CCDictionary*)msgMap->objectForKey(sender->m_uID);
		if (senderMap) {
			//message for receiver
			CCArray* receiverList=(CCArray*)senderMap->objectForKey(receiver->m_uID);
            CCObject* pObject = NULL;
            CCARRAY_FOREACH(receiverList,pObject){
                CCMessageHandler* handler=(CCMessageHandler*) pObject;
                if (handler->getHandle()==handle) {
                    receiverList->removeObject(handler);
                }
            }
		}
	}
}


void CCMessageManager::execRegisterReceiverList(CCArray* receiverList ,CCMessage* message)
{
	
	CCAssert(receiverList!=NULL,"CompleteMessageManage:execRegisterReceiverList:receiverList can't be null!");
	CCObject* pObject = NULL;
    CCARRAY_FOREACH(receiverList,pObject){
        CCMessageHandler* handler=(CCMessageHandler*) pObject;
		handler->execute(message);
    }
}

void CCMessageManager::execAllRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message)
{
	CCAssert(senderMap!=NULL,"CompleteMessageManage:execAllRegisterWithSenderMap:senderMap can't be null!");
	//send to all
	CCDictElement* pElement = NULL;
	CCDICT_FOREACH(senderMap,pElement){
		execRegisterReceiverList((CCArray*)pElement,message);
	}
}

void CCMessageManager::execRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message)
{
	CCObject* receiver=message->getReceiver();
	if (receiver) {
		//message for receiver
		CCArray* receiverList=(CCArray*)senderMap->objectForKey(receiver->m_uID);
		if(receiverList) execRegisterReceiverList(receiverList,message);
	}else {
		//send to all receiver
		execAllRegisterWithSenderMap(senderMap ,message);
	}
}

void CCMessageManager::execRegisterWithSenderMap(CCDictionary* senderMap,CCMessage* message,CCObject*  receiver)
{
	CCAssert(senderMap!=NULL,"CompleteMessageManage:execRegisterWithSenderMap:senderMap can't be null!");
	if (receiver) {
		//message for receiver
		CCArray* receiverList=(CCArray*)senderMap->objectForKey(receiver->m_uID);
		if(receiverList) execRegisterReceiverList(receiverList ,message);
	}else {
		//send to all receiver
		execAllRegisterWithSenderMap(senderMap ,message);
	}
}

//
void CCMessageManager::dispatchMessage(CCMessage* message)
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
		sender=message->getSender();
		//parse for sender
		//如果sender不为空，则还要触发一次全局消息。
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

void CCMessageManager::dispatchMessageWithType(MessageType type ,CCObject* sender ,CCObject* receiver,CCDictionary* data)
{
	CCMessage* message=new CCMessage();
	message->initWithType(type ,sender ,receiver ,data);
	dispatchMessage(message);
	message->release();
}

//适应message中没有receiver的情况
void CCMessageManager::dispatchMessage(CCMessage* message ,CCObject*  receiver)
{
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
