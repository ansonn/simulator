/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#ifndef _LOOKUP_MANAGER_CORE1_H
#define _LOOKUP_MANAGER_CORE1_H
#include "HalBasedef.h"


namespace Lookup1 
{

	class CLookupManager1Core 
	{

	public:
		CLookupManager1Core();
		~CLookupManager1Core();



	private:
		ManagerInit m_pfnLookupManagerInit;
		ManagerPoll m_pfnLookupManagerPoll;
	};


};


#endif

