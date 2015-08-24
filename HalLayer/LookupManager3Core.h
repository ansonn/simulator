/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#ifndef _LOOKUP_MANAGER_CORE3_H
#define _LOOKUP_MANAGER_CORE3_H
#include "Basedef.h"


class CLookupManager3Core 
{

public:
	CLookupManager3Core();
	~CLookupManager3Core();



private:
	ManagerInit m_pfnLookupManagerInit;
	ManagerPoll m_pfnLookupManagerPoll;
};




#endif

