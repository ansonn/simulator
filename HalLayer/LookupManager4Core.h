/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#ifndef _LOOKUP_MANAGER_CORE4_H
#define _LOOKUP_MANAGER_CORE4_H
#include "Basedef.h"



class CLookupManager4Core 
{

public:
	CLookupManager4Core();
	~CLookupManager4Core();



private:
	ManagerInit m_pfnLookupManagerInit;
	ManagerPoll m_pfnLookupManagerPoll;
};





#endif

