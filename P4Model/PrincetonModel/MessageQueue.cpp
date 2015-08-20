/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#include "MessageQueue.h"



CMessageQueue::CMessageQueue()
{


}

CMessageQueue::~CMessageQueue() 
{


}


void CMessageQueue::SetConnection(MesgSig_T::slot_function_type subscriber)  
{  
	m_SigConn = MesgSig_T.connect(subscriber);  
}

void CMessageQueue::SetDisconnect(void)  
{  
	m_SigConn.disconnect();  
}  


