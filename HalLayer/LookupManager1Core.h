/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#ifndef _LOOKUP_MANAGER_CORE1_H
#define _LOOKUP_MANAGER_CORE1_H
#include "Basedef.h"


class CLookupManager1Core 
{

public:
	CLookupManager1Core();
	~CLookupManager1Core();



private:
	ManagerInit m_pfnLookupManagerInit;
	ManagerPoll m_pfnLookupManagerPoll;
};





#endif

