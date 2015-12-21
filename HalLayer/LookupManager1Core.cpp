/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include "LookupManager1Core.h"




namespace Lookup1 {

#include "DummyManager.c"


}


/*---------------------------------------------------------------------------*/
CLookupManager1Core::CLookupManager1Core() : m_pfnLookupManagerInit(nullptr)
										   , m_pfnLookupManagerPoll(nullptr)
{
    m_pfnLookupManagerInit = Lookup1::DummyManager_Init;
    m_pfnLookupManagerPoll = Lookup1::DummyManager_Poll;

    m_pfnLookupManagerInit(5);
    m_pfnLookupManagerPoll(1);
}

CLookupManager1Core::~CLookupManager1Core() 
{


}

void CLookupManager1Core::TestSetPrintf(u32 setValue)
{
    m_pfnLookupManagerPoll(setValue);
    printf("Lookup1 gID=%d\n", Lookup1::gID);
}


