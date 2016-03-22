// Quote.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"


class quoteTest{

    unsigned int *&tBase;
    char a;
    char b;
    char c;

public:
    quoteTest(unsigned int *&_base, char _p) : tBase(_base), b(a), a(_p), c(b)
    {

    }
    ~quoteTest()
    {

    }

    void printIdx(unsigned int idx)
    {
        printf("--->tBase[%d]=%d\n", idx, tBase[idx]);
        printf("--->a=%d,b=%d,c=%d\n", a, b, c);
    }

};

unsigned int buffer[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };

int _tmain(int argc, _TCHAR* argv[])
{
    unsigned int *ptr = buffer;

    quoteTest quoteTest(ptr, 9);

    quoteTest.printIdx(3);

    system("pause");

	return 0;
}

