/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include "PCIeManagerCore.h"
#include "SsdModel.h"


namespace PCIe 
{
    // firmware
#include "DummyManager.c"


    // hal api
}


/**
*
**/
CPCIeManagerCore::CPCIeManagerCore() : m_pfnPCIeManagerInit(NULL)
                                     , m_pfnPCIeManagerPoll(NULL)
{
    m_pfnPCIeManagerInit = PCIe::DummyManager_Init;
    m_pfnPCIeManagerPoll = PCIe::DummyManager_Poll;

    m_pfnPCIeManagerInit(0);
    m_pfnPCIeManagerPoll(0);
}

CPCIeManagerCore::~CPCIeManagerCore() 
{


}

void CPCIeManagerCore::TestAddAndPrintf(u32 addValue)
{
    m_pfnPCIeManagerPoll(addValue);
    printf("PCIe gID=%d\n", PCIe::gID);
}

