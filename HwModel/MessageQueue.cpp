/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include "MessageQueue.h"



CMessageQueue::CMessageQueue()
{


}

CMessageQueue::~CMessageQueue() 
{


}


void CMessageQueue::SetConnection(MesgSig_T::slot_function_type subscriber)  
{  
	m_SigConn = m_MesgSig.connect(subscriber);  
}

void CMessageQueue::SetDisconnect(void)  
{  
	m_SigConn.disconnect();  
}  


