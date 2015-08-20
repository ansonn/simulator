/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#include "LookupManager3Core.h"




namespace Lookup3 {

	/*---------------------------------------------------------------------------*/
	// 
#include "FwDummyManager.h"

	CLookupManager3Core::CLookupManager3Core() : m_pfnLookupManagerInit(nullptr)
											   , m_pfnLookupManagerPoll(nullptr)
	{

	}

	CLookupManager3Core::~CLookupManager3Core() 
	{


	}


} // end namespace Lookup3
