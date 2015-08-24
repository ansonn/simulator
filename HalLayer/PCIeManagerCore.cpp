/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include "PCIeManagerCore.h"
#include <P4Model.h>


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

    m_pfnPCIeManagerInit(0);
}

CPCIeManagerCore::~CPCIeManagerCore() 
{


}


