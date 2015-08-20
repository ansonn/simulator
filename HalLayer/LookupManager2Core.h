/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#ifndef _LOOKUP_MANAGER_CORE2_H
#define _LOOKUP_MANAGER_CORE2_H
#include "HalBasedef.h"


namespace Lookup2 
{

	class CLookupManager2Core 
	{

	public:
		CLookupManager2Core();
		~CLookupManager2Core();



	private:
		ManagerInit m_pfnLookupManagerInit;
		ManagerPoll m_pfnLookupManagerPoll;
	};


};


#endif

