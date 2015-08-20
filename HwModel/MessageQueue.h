/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#ifndef _MESSAGE_QUEUE_H
#define _MESSAGE_QUEUE_H
#include "Basedef.h"
#include <boost/signals2/signal.hpp>
#include <boost/signals2/shared_connection_block.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>


class CMessageQueue
{

public:
	CMessageQueue();
	~CMessageQueue();

	// Signal
	typedef boost::signals2::signal<void ()>  MesgSig_T;
	typedef boost::signals2::connection		  SigConn_T;


	void SetConnection(MesgSig_T::slot_function_type subscriber);  
	void SetDisconnect(void);  

	virtual void SignalHandler(s8 *buffer, u32 size) = 0;


private:
	// Regs
	MesgSig_T		m_MesgSig;
	SigConn_T		m_SigConn;

};





#endif
