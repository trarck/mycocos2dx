//
//  Message.m
//  Message

//  Created by trarck on 11-11-27.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#include "CCMessage.h"

using namespace std;

NS_CC_BEGIN

CCMessage::CCMessage(void)
:m_type(0)
,m_sender(NULL)
,m_receiver(NULL)
,m_data(NULL)
,m_timeStamp(0.0f)
{
    CCLog("Message create");
}

CCMessage::~CCMessage(void)
{
    CCLog("Message destroy  begin");
    CC_SAFE_RELEASE(m_sender);
    CC_SAFE_RELEASE(m_receiver);
    CC_SAFE_RELEASE(m_data);
    CCLog("Message destroy end");
}

MessageType CCMessage::getType()
{
    return m_type;
}

void CCMessage::setType(MessageType type)
{
    m_type=type;
}

MessageParty CCMessage::getSender()
{
    return m_sender;
}
void CCMessage::setSender(MessageParty sender)
{
    CC_SAFE_RETAIN(sender);
    CC_SAFE_RELEASE(m_sender);
    m_sender=sender;
}

MessageParty CCMessage::getReceiver()
{
    return m_receiver;
}

void CCMessage::setReceiver(MessageParty receiver)
{
    CC_SAFE_RETAIN(receiver);
    CC_SAFE_RELEASE(m_receiver);
    m_receiver=receiver;
}

CCDictionary* CCMessage::getData()
{
    return m_data;
}

void CCMessage::setData(CCDictionary* data)
{
    CC_SAFE_RETAIN(data);
    CC_SAFE_RELEASE(m_data);
    m_data=data;
}


bool CCMessage::initWithType(MessageType type,MessageParty sender ,MessageParty receiver ,CCDictionary *data)
{

    m_type=type;
    setSender(sender);
    setReceiver(receiver);
    setData(data);
    
    return true;
}

bool CCMessage::initWithType(MessageType type,MessageParty sender,CCDictionary *data)
{
    return initWithType(type,sender,NULL,data);
}

NS_CC_END
