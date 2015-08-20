/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include "PCIeManagerCore.h"



namespace PCIe 
{

	/*---------------------------------------------------------------------------*/
	// 
#include "FwDummyManager.h"

	CPCIeManagerCore::CPCIeManagerCore() : m_pfnPCIeManagerInit(nullptr)
										 , m_pfnPCIeManagerPoll(nullptr)
	{
		m_pfnPCIeManagerInit = DummyManager_Init;

		m_pfnPCIeManagerInit(0);
	}

	CPCIeManagerCore::~CPCIeManagerCore() 
	{


	}


} // end namespace PCIe
