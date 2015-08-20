#pragma once

#include "resource.h"
#include "PCIeManagerCore.h"
#include "LookupManager1Core.h"
#include "LookupManager2Core.h"
#include "LookupManager3Core.h"
#include "LookupManager4Core.h"



/*---------------------------------------------------------------------------*/
//	PBlaze4 Model Class
//
class CPBlaze4Model {

	public:
		CPBlaze4Model();
		~CPBlaze4Model();


		void m_fnStartUp(int argc, TCHAR* argv[]);


	private:
		PCIe	::CPCIeManagerCore		m_CPCIeManagerCore;
		Lookup1	::CLookupManager1Core	m_CLookupManager1Core;
		Lookup2	::CLookupManager2Core	m_CLookupManager2Core;
		Lookup3	::CLookupManager3Core	m_CLookupManager3Core;
		Lookup4	::CLookupManager4Core	m_CLookupManager4Core;
};


