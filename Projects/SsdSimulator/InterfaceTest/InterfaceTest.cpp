// InterfaceTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



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
    gFiFccModel.fccInf.fccRead();
    gFiInterfaceModel.fiInf.fiRead();

	return 0;
}

