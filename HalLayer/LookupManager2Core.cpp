/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include "LookupManager2Core.h"




namespace Lookup2 
{

	/*---------------------------------------------------------------------------*/
	// 
#include "FwDummyManager.h"

	CLookupManager2Core::CLookupManager2Core() : m_pfnLookupManagerInit(nullptr)
											   , m_pfnLookupManagerPoll(nullptr)
	{

	}

	CLookupManager2Core::~CLookupManager2Core() 
	{


	}


} // end namespace Lookup2
