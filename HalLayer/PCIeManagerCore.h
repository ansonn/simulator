/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#ifndef _PCIE_MANAGER_CORE_H
#define _PCIE_MANAGER_CORE_H
#include "Basedef.h"


class CPCIeManagerCore 
{

public:
	CPCIeManagerCore();
	~CPCIeManagerCore();

    void TestAddAndPrintf(u32 addValue);

private:
	ManagerInit m_pfnPCIeManagerInit;
	ManagerPoll m_pfnPCIeManagerPoll;
};



#endif
