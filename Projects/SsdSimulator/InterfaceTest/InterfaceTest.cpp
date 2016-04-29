// InterfaceTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>


class fiInterface 
{
public:
    virtual void fiRead() = 0;
    virtual void fiWrite() = 0;
};


class fccInterface 
{
public:
    virtual void fccRead() = 0;
    virtual void fccWrite() = 0;
};


class nandInterface 
{
public:
    virtual void nandRead() = 0;
    virtual void nandWrite() = 0;
};


class fiInterfaceModel
{
public:
    fiInterfaceModel(fiInterface &pfiInf) : fiInf(pfiInf)
    {

    }

    fiInterface &fiInf;
    

};

class fiFccModel
{
public:
    fiFccModel(fccInterface &pfccInf) : fccInf(pfccInf)
    {

    }

    fccInterface &fccInf;
};

class fiCommon : public fiInterface, public fccInterface
{

public:
    void fiRead()
    {
        printf("fiCommon --> fiRead\n");
    }

    void fiWrite()
    {
        printf("fiCommon --> fiWrite\n");
    }

    void fccRead()
    {
        printf("fiCommon --> fccRead\n");
    }

    void fccWrite()
    {
        printf("fiCommon --> fccWrite\n");
    }
};



fiCommon gFiCommon;
fiFccModel gFiFccModel(gFiCommon);
fiInterfaceModel gFiInterfaceModel(gFiCommon);


int _tmain(int argc, _TCHAR* argv[])
{
    //gFiFccModel.fccInf.fccRead();
    //gFiInterfaceModel.fiInf.fiRead();

    int len = 8;
    unsigned char data[20] = {1,2,3,4,5,6,7,8};

    TCHAR tBuffer[256] = { 0 };
    for (int i = 0; i < len; i++)
    {
        _snwprintf_s(tBuffer + i * 2, len * 2, 2, _T("%02x"), data[i]);
    }
    system("pause");

	return 0;
}

