/*---------------------------------------------------------------------------*/
//	CopyRight: BJ-MEMBLAZE
//	2015-08-01
//
//-----------------------------------------------------------------------------


unsigned int gID = 1;

int DummyManager_Init(unsigned int managerID)
{

	gID = managerID;
	return 0;
}

int DummyManager_Poll(unsigned int managerID)
{
    gID += managerID;

	return 0;
}

