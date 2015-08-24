/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#ifndef _LOOKUP_MANAGER_CORE2_H
#define _LOOKUP_MANAGER_CORE2_H
#include "Basedef.h"



class CLookupManager2Core 
{

public:
    CLookupManager2Core();
    ~CLookupManager2Core();



private:
    ManagerInit m_pfnLookupManagerInit;
    ManagerPoll m_pfnLookupManagerPoll;
};





#endif

