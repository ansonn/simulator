/*---------------------------------------------------------------------------*/
/*
**  shuke.wang@memblaze.com
**  2015/08~09
*----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "PCIeManagerCore.h"
#include "LookupManager1Core.h"
#include "SsdModel.h"

SsdModel gSsdModel;
/*
* char *argv[])
{
    CLookupManager1Core GCLookupManager1Core;
    CPCIeManagerCore GCPCIeManagerCore;


    GCPCIeManagerCore.TestAddAndPrintf();


	return 0;
}

*/
int main(int argc, char *argv[])
{
    CLookupManager1Core GCLookupManager1Core;
    CPCIeManagerCore GCPCIeManagerCore;


    GCPCIeManagerCore.TestAddAndPrintf(2);
    GCLookupManager1Core.TestSetPrintf(0);
    GCPCIeManagerCore.TestAddAndPrintf(2);
    GCLookupManager1Core.TestSetPrintf(0);
    GCPCIeManagerCore.TestAddAndPrintf(2);
    GCLookupManager1Core.TestSetPrintf(0);
    GCPCIeManagerCore.TestAddAndPrintf(2);
    GCLookupManager1Core.TestSetPrintf(0);
    GCPCIeManagerCore.TestAddAndPrintf(2);
    GCLookupManager1Core.TestSetPrintf(0);
    GCPCIeManagerCore.TestAddAndPrintf(2);
    GCLookupManager1Core.TestSetPrintf(0);

	return 0;
}

