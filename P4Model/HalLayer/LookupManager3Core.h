/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#ifndef _LOOKUP_MANAGER_CORE3_H
#define _LOOKUP_MANAGER_CORE3_H
#include "HalBasedef.h"


namespace Lookup3
{

	class CLookupManager3Core 
	{

	public:
		CLookupManager3Core();
		~CLookupManager3Core();



	private:
		ManagerInit m_pfnLookupManagerInit;
		ManagerPoll m_pfnLookupManagerPoll;
	};


};


#endif

