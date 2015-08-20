/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include "LookupManager4Core.h"




namespace Lookup4 {

	/*---------------------------------------------------------------------------*/
	// 
#include "FwDummyManager.h"

	CLookupManager4Core::CLookupManager4Core() : m_pfnLookupManagerInit(nullptr)
											   , m_pfnLookupManagerPoll(nullptr)
	{

	}

	CLookupManager4Core::~CLookupManager4Core() 
	{


	}


} // end namespace Lookup4
