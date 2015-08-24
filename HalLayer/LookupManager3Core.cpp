/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include "LookupManager3Core.h"




namespace Lookup3 {

#include "DummyManager.c"

}


CLookupManager3Core::CLookupManager3Core() : m_pfnLookupManagerInit(nullptr)
                                           , m_pfnLookupManagerPoll(nullptr)
{

}

CLookupManager3Core::~CLookupManager3Core() 
{


}



