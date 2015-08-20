/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#ifndef _LOOKUP_MANAGER_CORE4_H
#define _LOOKUP_MANAGER_CORE4_H
#include "HalBasedef.h"


namespace Lookup4
{

	class CLookupManager4Core 
	{

	public:
		CLookupManager4Core();
		~CLookupManager4Core();



	private:
		ManagerInit m_pfnLookupManagerInit;
		ManagerPoll m_pfnLookupManagerPoll;
	};


}


#endif

