/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------
#ifndef _PCIE_MANAGER_CORE_H
#define _PCIE_MANAGER_CORE_H
#include "HalBasedef.h"


namespace PCIe 
{

	class CPCIeManagerCore 
	{

	public:
		CPCIeManagerCore();
		~CPCIeManagerCore();



	private:
		ManagerInit m_pfnPCIeManagerInit;
		ManagerPoll m_pfnPCIeManagerPoll;
	};





} // end namespace PCIe

#endif
